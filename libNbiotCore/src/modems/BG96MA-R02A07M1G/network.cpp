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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ========================================================================
*/

#include <stdlib.h>

#include "atcommands.h"
#include "nbiottimer.h"
#include "nbiotstringlist.h"
#include "nbiotdebug.h"
#include "network.h"
#include "nbiotcircularbuffer.h"


const char* Network::cmdQIOPEN_arg = "AT+QIOPEN=1,%d,\"UDP\",\"%s\",%d\r";
const char* Network::respQIURCrecv_arg = "+QIURC: \"recv\",";
const char* Network::cmdQIRD_arg = "AT+QIRD=%d,%d\r";
const char* Network::respQIRD = "+QIRD:";
const char* Network::cmdQISENDEX_arg = "AT+QISENDEX=%d,\"";
const char* Network::respQISENDEX = "SEND OK";
const char* Network::cmdQICLOSE_arg = "AT+QICLOSE=%d\r";


Network::Network(Serial& serial) :
    m_cmd(serial),
    m_connectionNumber(-1),
    m_hostname(),
    m_port(0),
    m_bytesAvail(0)
{
    m_cmd.clearFilter();
}


bool Network::connect(const char* hostname, unsigned short port)
{
    bool ret = true;

    if(m_cmd.sendCommand(nbiot::string::Printf(cmdQIOPEN_arg, m_connectionNumber+1, hostname, port)))
    {
        if(!m_cmd.readResponse(REPLY_OK, threeSeconds))
        {
            ret = false;
        }
        else
        {
            if(!m_cmd.readUntil(nbiot::string::Printf("+QIOPEN: %d,0").c_str(), tenSeconds))
            {
                ret = false;
            }
	    else
	    {
                m_connectionNumber++;
            }
        }
    }
    else
    {
        ret = false;
    }

    if(ret)
    {
        nbiot::string response = m_cmd.getResponse();

        nbiot::StringList l = response.split(",");
        if(2 == l.count() && 0 == atoi(l[1].c_str()))
        {
            nbiot::StringList tokens = l[0].split(":");
            if(2 == tokens.count())
            {
                m_connectionNumber = atoi(tokens[1].c_str());
            }
	    else
            {
                ret = false;
            }
        }
        else
        {
            ret = false;
        }

        if((0 <= m_connectionNumber))
        {
            m_hostname = hostname;
            m_port = port;
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
        m_qiurc = nbiot::string::Printf(respQIURCrecv_arg, m_connectionNumber);
        m_cmd.addUrcFilter(m_qiurc.c_str(), this, &Network::parseFilterResult);
    }
    
#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
    debugPrintf("modem connect: %s\r\n", ((ret)?"ok":"fail"));
#endif
    return ret;
    
}

int Network::read(unsigned char* buffer, int len, unsigned short timeout_ms)
{
    int rc = -1;
    int reqLen = 0;
    int rb = -1;
    int bytes = m_readBuffer.size();

    nbiot::Timer timer(timeout_ms);
    nbiot::string data;

    if (len >= 0)
    {
        reqLen = len;

	// Try to read requested length (helps to make sure we didn't miss any URC
	if(!m_bytesAvail)
	{
	    rb = ipRead(m_readBuffer, reqLen, readInterval);
	    m_bytesAvail = 0;
	}

	if(0 < rb)
	{
	    bytes += rb;
	}

	// Loop and look for incoming messages until enough bytes available
	while (readInterval < timer.remaining() && m_readBuffer.size() < reqLen)
	{
	    m_cmd.readResponse(REPLY_IGNORE, readInterval);
	    if (m_bytesAvail)
	    {
		rb = ipRead(m_readBuffer, reqLen - bytes, readInterval);
		m_bytesAvail = 0;
		if(0 < rb)
		{
		    bytes += rb;
		}
	    }
	}

	if(0 < bytes)
	{
	    rc = 0;
	    unsigned char c;
	    while(!m_readBuffer.empty() && rc < reqLen)
	    {
		m_readBuffer.get(c);
		data.append((char*) &c, 1);
		rc++;
	    }

	    data.copy(buffer, static_cast<size_t>(rc));
	    
#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
	    debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
	    debugPrintf("r(%d): ", rc);
	    for(size_t i = 0; i < static_cast<size_t>(rc); ++i)
	    {
		debugPrintf("%02X ", buffer[i]);
	    }
	    debugPrintf("\r\n");
#endif
	}
    }
    
    return rc;
}

unsigned short Network::ipAvailable()
{
    return static_cast<unsigned short>((m_bytesAvail & byteCountMask));
}

// TODO: not really required, should be refactored
int Network::ipRead(nbiot::CircularBuffer<READ_BUFFER_SIZE>& data, int len, unsigned short timeout_ms)
{
    int avail = -1;
    int rb = 0;

    nbiot::Timer timer(timeout_ms);
    if(m_cmd.sendCommand(nbiot::string::Printf(cmdQIRD_arg, m_connectionNumber, len)))
    {
        if(m_cmd.readUntil(respQIRD, timer.remaining()))
        {
            nbiot::string response = m_cmd.getResponse();
            if(!(response == "ERROR"))
            {
                size_t pos = response.find(':');
                if(nbiot::string::npos != pos)
                {
                    pos++;
                    while (' ' == response.at(pos)) // eat up spaces if there are any
                    {
                        pos++;
                    }
                    if (pos < response.size())
                    {
                        nbiot::string number = response.substr(pos, response.size());
                        avail = atoi(number.c_str());
                    }
                }
            }
            if(0 < avail)
            {
                for(int i = 0; i < avail; ++i)
                {
                    unsigned char c = -1;
                    if(m_cmd.serial.readRaw(&c, static_cast<unsigned short>(timer.remaining())))
                    {
                        data.put(c);
                        rb++;
                    }
                }
                m_cmd.readResponse(REPLY_OK, static_cast<unsigned short>(timer.remaining()));
            }
        }
    }
    m_cmd.readResponse(REPLY_IGNORE, static_cast<unsigned short>(timer.remaining()));

    return rb;
}

bool Network::write(unsigned char* buffer, unsigned long len, unsigned short timeout)
{
    bool ret = false;

    if((!m_hostname.empty()) && (0 < m_port))
    {
        nbiot::string data = nbiot::string::Printf(cmdQISENDEX_arg, m_connectionNumber);
        nbiot::string hex = nbiot::string((char*)(buffer), len).toHex();
        data += hex;
        data += "\"";
	
#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
        debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
        debugPrintf("data(%d) = %s\n", len, hex.c_str());
#endif

        if(m_cmd.sendCommand(data.c_str()))
        {
            if(m_cmd.readResponse(REPLY_EXACT, timeout))
            {
                nbiot::string response = m_cmd.getResponse();
                if(response == respQISENDEX)
                {
                    ret = true;
                }
            }
        }
    }
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);

    return ret;
}

void Network::parseFilterResult(const char *strFilterResult)
{
    nbiot::string response = strFilterResult;    
#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
        debugPrintf("\n\033[0;32m[ MODEM    ]\033[0m ");
#endif
        debugPrintf("Filter: ");
        debugPrintf(response.c_str());
        debugPrintf("\r\n");
#endif
    size_t pos = response.find(',');

    if(nbiot::string::npos != pos)
    {
        nbiot::string number = response.substr((pos + 1));
        m_bytesAvail = 1;
    }
}

bool Network::disconnect()
{
    bool ret = true;

    if(0 <= m_connectionNumber)
    {
        if(m_cmd.sendCommand(nbiot::string::Printf(cmdQICLOSE_arg, m_connectionNumber)))
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
        m_cmd.removeUrcFilter(respQIURCrecv_arg);
    }
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);

    return ret;
}
