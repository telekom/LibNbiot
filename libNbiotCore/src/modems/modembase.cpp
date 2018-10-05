/* ========================================================================
 * LibNbiot: modembase.cpp
 * 
 * Copyright (c) 2018, Edgar Hindemith, Yassine Amraue, Thorsten
 * Krautscheid, Kolja Vornholt, T-Systems International GmbH
 * contact: libnbiot@t-systems.com, opensource@telekom.de
 * 
 * This file is distributed under the conditions of the Apache License,
 * Version 2.0 (the ",License");
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

#include "modembase.h"

#include "nbiotstringlist.h"
#include "nbiotdebug.h"


const char* ModemBase::cmdCIMI = "AT+CIMI";
const char* ModemBase::cmdCGATT0 = "AT+CGATT=0";
const char* ModemBase::cmdCGATTquery = "AT+CGATT?";
const char* ModemBase::respCGATTquery = "+CGATT:";
const char* ModemBase::cmdCFUN1 = "AT+CFUN=1";
const char* ModemBase::cmdCGDCONT_arg = "AT+CGDCONT=1,\"IP\",\"%s\"\r";
const char* ModemBase::cmdCOPS_arg = "AT+COPS=1,2,\"%s\"\r";
const char* ModemBase::cmdCFUN_arg = "AT+CFUN=%d\r";
const char* ModemBase::cmdCFUNquery = "AT+CFUN?";
const char* ModemBase::respCFUNquery = "+CFUN:";
const char* ModemBase::respOK = "OK";


ModemBase::ModemBase(Serial& serial) : 
    m_cmd(serial),
    m_plmn(),
    m_apn(),
    m_deviceId()
{}


bool ModemBase::sendAtCommand(const char* command, char* response, size_t len, unsigned short timeout)
{
    bool ret = false;

    if(m_cmd.sendCommand(command))
    {
        if( m_cmd.readResponse(REPLY_COPY, timeout ))
        {
            nbiot::string res = m_cmd.getResponse();
            if(res.size() <= len)
            {
                res.copy(response, len);
                ret = true;
            }
        }
    }
    return ret;
}


bool ModemBase::attach()
{
    bool ret = true;

    if(m_cmd.sendCommand(nbiot::string::Printf(cmdCOPS_arg, m_plmn.c_str())))
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

#ifdef DEBUG_MODEM
    bool state = false;

    if(ret)
    {
        if( 1 == getAttachState() )
        {
            state = true;
        }
    }
#endif

    m_cmd.readResponse(REPLY_IGNORE, oneSecond);

#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
    debugPrintf("attach: %s, state = %s\r\n", ((ret)?"ok":"fail"), ((state)?"attached":"detached"));
#endif

    return ret;
}

bool ModemBase::detach()
{
    bool ret = true;

    if(ret)
  {
        // detach the LTE session
        if(!m_cmd.sendCommand(cmdCGATT0))
        {
            ret = false;
        }
    }
    if(ret)
    {
        if(!m_cmd.readResponse(REPLY_OK, tenSeconds))
        {
            ret = false;
        }
    }
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);

#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
    debugPrintf("detach: %s\r\n", ((ret)?"ok":"fail"));
#endif

    return ret;
}


bool ModemBase::isAttached()
{
    return ( 1 == getAttachState() );
}


bool ModemBase::getImsi(nbiot::string& imsi)
{
    bool ret = false;

    if(m_cmd.sendCommand(cmdCIMI))
    {
        if(m_cmd.readResponse(REPLY_ANY, threeSeconds))
        {
            imsi = m_cmd.getResponse();
            ret = true;
#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
            debugPrintf("IMSI: %s\r\n", imsi.c_str());
#endif
        }
    }
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);
    return ret;
}


const char* ModemBase::getDeviceId()
{
    if(m_deviceId.empty())
    {
        getImsi(m_deviceId);
    }
    return m_deviceId.c_str();
}


IModem::ModemState ModemBase::switchModem(IModem::ModemState state)
{
    ModemState ret = isModemOn();
    if(ret != state)
    {
        if(m_cmd.sendCommand(nbiot::string::Printf(cmdCFUN_arg, ((MODEM_ON == state)?1:0))))
        {
            if(m_cmd.readResponse(REPLY_OK, tenSeconds))
            {
                ret = state;
            }
        }
        m_cmd.readResponse(REPLY_IGNORE, oneSecond);
    }
#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
    debugPrintf("switch modem: %s\r\n", ((ret)?"on":"off"));
#endif
    return ret;    
}


IModem::ModemState ModemBase::isModemOn()
{
    IModem::ModemState ret = IModem::MODEM_OFF;

    if(m_cmd.sendCommand(cmdCFUNquery))
    {
        if(m_cmd.readUntil(respCFUNquery, tenSeconds))
        {
            nbiot::string response = m_cmd.getResponse();
            #ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
            debugPrintf("CFUN-Response: %s\r\n", response.c_str());
            #endif
            size_t pos = response.find(':');
            if(nbiot::string::npos != pos)
            {
                pos++;
                while(' ' == response.at(pos)) // eat up spaces if there's any
                {
                    pos++;
                }
                if(response.size() > pos)
                {
                    nbiot::string number = response.substr(pos);
                    ret = atoi(number.c_str()) == 0 ? ModemState::MODEM_OFF : ModemState::MODEM_ON;
                }
            }
        }
    }
    #ifdef DEBUG_MODEM
    #ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
    #endif
    debugPrintf("Attachment-State: %d\r\n", ret);
    #endif
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);
    return ret;
}


int ModemBase::getAttachState()
{
    int ret = -1;

    // get GPRS attachement-state
    if(m_cmd.sendCommand(cmdCGATTquery))
    {
        if(m_cmd.readUntil(respCGATTquery, tenSeconds))
        {
            nbiot::string response = m_cmd.getResponse();
            #ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
            debugPrintf("CGATT-Response: %s\r\n", response.c_str());
            #endif
            size_t pos = response.find(':');
            if(nbiot::string::npos != pos)
            {
                pos++;
                while(' ' == response.at(pos)) // eat up spaces if there's any
                {
                    pos++;
                }
                if(response.size() > pos)
                {
                    nbiot::string number = response.substr(pos);
                    ret = atoi(number.c_str());
                }
            }
        }
    }
    #ifdef DEBUG_MODEM
    #ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
    #endif
    debugPrintf("Attachment-State: %d\r\n", ret);
    #endif
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);
    return ret;
}
