/* ========================================================================
 * LibNbiot: nbiotconnectivity.cpp
 *
 * Copyright (c) 2018, Edgar Hindemith, Yassine Amraue, Thorsten
 * Krautscheid, Kolja Vornholt, T-Systems International GmbH
 * contact: libnbiot@t-systems.com, opensource@telekom.de
 *
 * This file is distributed under the conditions of the Apache License,
 * Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * For details see the file LICENSE at the toplevel.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either expressed or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ========================================================================
*/

#include <stdlib.h>
#include "atcommands.h"
#include "nbiottimer.h"
#include "nbiotstringlist.h"
#include "nbiotdebug.h"

#include "nbiotconnectivity.h"

const char* NbiotConnectivity::cmdNSOCR_arg = "AT+NSOCR=DGRAM,17,%d\r";
const char* NbiotConnectivity::respNSONMI_arg = "+NSONMI:%d,";
const char* NbiotConnectivity::cmdNSORF_arg = "AT+NSORF=%d,%d\r";
const char* NbiotConnectivity::cmdNSOST_arg = "AT+NSOST=%d,%s,%d,%d,";
const char* NbiotConnectivity::cmdNSOCL_arg = "AT+NSOCL=%d\r";

NbiotConnectivity::NbiotConnectivity(Serial& serial) :
    m_cmd(serial),
    m_connectionNumber(-1),
    m_hostname(),
    m_port(0),
    m_bytesAvail(0),
    m_nsonmi(),
    m_lastListenPort(listenPortBase-1)
{
    m_cmd.clearFilter();
}


bool NbiotConnectivity::connect(const char* hostname, unsigned short port)
{
    bool ret = true;

    unsigned short listenPort = m_lastListenPort;
    if((0 <= m_connectionNumber) && (maxSocket > m_connectionNumber))
    {
        listenPort = (listenPortBase + m_connectionNumber);
    }

    if(m_cmd.sendCommand(nbiot::string::Printf(cmdNSOCR_arg, listenPort)))
    {
        if(!m_cmd.readResponse(REPLY_ANY, threeSeconds))
        {
            ret = false;
        }
    }
    else
    {
        ret = false;
    }
    if(ret)
    {
        nbiot::string response = m_cmd.getResponse();
        m_connectionNumber = atoi(response.c_str());
        if((0 <= m_connectionNumber) && (maxSocket > m_connectionNumber))
        {
            m_hostname = hostname;
            m_port = port;
            m_lastListenPort = listenPort;
        }
        else
        {
            m_hostname = "";
            m_port = 0;
            m_connectionNumber = -1;
            ret = false;
        }
    }
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);
    if(ret)
    {
        m_nsonmi = nbiot::string::Printf(respNSONMI_arg, m_connectionNumber);
        m_cmd.addUrcFilter(m_nsonmi.c_str(), this, &NbiotConnectivity::parseFilterResult);
    }
#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
    debugPrintf("modem connect: %s\r\n", ((ret)?"ok":"fail"));
#endif
    return ret;
}

#define MQTTSN_MIN_PACKET_LEN 1
int NbiotConnectivity::read(unsigned char* buffer, int len, unsigned short timeout_ms)
{
    int rc = -1;
    int dgmLen = 0;
    int reqLen = MQTTSN_MIN_PACKET_LEN;

    if(MQTTSN_MIN_PACKET_LEN < len)
    {
        reqLen = len;
    }

    nbiot::Timer timer(timeout_ms);
#ifdef DEBUG_MODEM
    bool dbgLine = false;
#endif
    nbiot::string data;
    int bytes = 0;
    int rb = -1;
    while (readInterval < timer.remaining())
    {
        dgmLen = ipAvailable();
        if((reqLen <= (dgmLen + bytes)) || ((0 < bytes) && (0 < dgmLen)))
        {
            break;
        }
        rb = ipRead(data, 1, readInterval);// also poll the input stream
        if(0 < rb)
        {
            bytes += rb;
        }
#ifdef DEBUG_MODEM
        if(0 == dgmLen)
        {
#ifdef DEBUG_COLOR
            if (!dbgLine)
                debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
            debugPrintf(".");
            dbgLine = true;
        }
#endif
    }
#ifdef DEBUG_MODEM
    if(dbgLine)
    {
        debugPrintf("\r\n");
    }
#endif
    //Quick Fix: Make sure that dgmLen is updated in all cases, especially if while-loop is left because )timer.remaining <= readInterval)
    dgmLen = ipAvailable();
    if(0 < dgmLen)
    {
#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
        debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
        debugPrintf("datagram-len: %d\r\n", dgmLen);
#endif
        unsigned short to = (readInterval < timer.remaining())?static_cast<unsigned short>(timer.remaining()):readInterval;
        rc = ipRead(data, dgmLen, to) + bytes;
    }
    else
    {
        rc = bytes;
    }
#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
    debugPrintf("r(%d): ", rc);
#endif
    if(0 < rc)
    {
        data.copy(buffer, static_cast<size_t>(rc));
#ifdef DEBUG_MODEM
        for(size_t i = 0; i < static_cast<size_t>(rc); ++i)
        {
            debugPrintf("%02X ", buffer[i]);
        }
#endif
    }
#ifdef DEBUG_MODEM
    debugPrintf("\r\n");
#endif
    return rc;
}

unsigned short NbiotConnectivity::ipAvailable()
{
    return static_cast<unsigned short>((m_bytesAvail & byteCountMask));
}

int NbiotConnectivity::ipRead(nbiot::string& data, int len, unsigned short timeout_ms)
{
    int avail = -1;

    nbiot::Timer timer(timeout_ms);
    if(m_cmd.sendCommand(nbiot::string::Printf(cmdNSORF_arg, m_connectionNumber, len)))
    {
        if(m_cmd.readResponse(REPLY_ANY, timeout_ms))
        {
            nbiot::string response = m_cmd.getResponse();
            if(0 == response.find("OK"))
            {
                avail = 0;
            }
            else
            {
                nbiot::StringList list = response.split(',');
                if(6 == list.count())
                {
                    if(m_connectionNumber == atoi(list[nsorfRespIdx_connNum].c_str()))
                    {
                        avail = atoi(list[nsorfRespIdx_bytesAvail].c_str());
                        if((0 < avail) && (avail <= len))
                        {
                            data.append(nbiot::string::fromHex(list[nsorfRespIdx_data].c_str()));
                            m_bytesAvail = static_cast<size_t>(atoi(list[nsorfRespIdx_bytesRem].c_str()));
                        }
                    }
                }
            }
        }
    }

    m_cmd.readResponse(REPLY_IGNORE, static_cast<unsigned short>(timer.remaining()));

    return avail;
}

bool NbiotConnectivity::write(unsigned char* buffer, unsigned long len, unsigned short timeout)
{
    bool ret = false;

    if((!m_hostname.empty()) && (0 < m_port))
    {
        nbiot::string data = nbiot::string::Printf(cmdNSOST_arg, m_connectionNumber, m_hostname.c_str(), m_port, len);
        nbiot::string hex = nbiot::string((char*)(buffer), len).toHex();
        data += hex;
        #ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
        debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
        debugPrintf("data(%d) = %s\n", len, hex.c_str());
        #endif

        if(m_cmd.sendCommand(data.c_str()))
        {
            if(m_cmd.readResponse(REPLY_ANY, timeout))
            {
                ret = true;
            }
        }
    }
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);

    return ret;
}

void NbiotConnectivity::parseFilterResult(const char *strFilterResult)
{
    nbiot::string response = strFilterResult;
    size_t pos = response.find(',');
    if(nbiot::string::npos != pos)
    {
        nbiot::string number = response.substr((pos + 1));
        m_bytesAvail = static_cast<size_t>(atoi(number.c_str()));
        #ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
        debugPrintf("\n\033[0;32m[ MODEM    ]\033[0m ");
#endif
        debugPrintf("available bytes:: %d\r\n", m_bytesAvail);
        #endif
    }
}

bool NbiotConnectivity::disconnect()
{
    bool ret = true;

    if(0 <= m_connectionNumber)
    {
        if(m_cmd.sendCommand(nbiot::string::Printf(cmdNSOCL_arg, m_connectionNumber)))
        {
            if(!m_cmd.readResponse(REPLY_OK, tenSeconds))
            {
                ret = false;
            }
        }
        else
        {
            ret = false;
        }
    }
    if(ret)
    {
        m_hostname = "";
        m_port = 0;
        m_connectionNumber = -1;
        m_cmd.removeUrcFilter(m_nsonmi.c_str());
        m_nsonmi = "";
    }
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);

    return ret;
}
