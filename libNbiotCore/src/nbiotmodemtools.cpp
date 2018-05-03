/* ========================================================================
 * LibNbiot: nbiotmodemtools.cpp
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

#include "nbiottimer.h"
#include "nbiotstringlist.h"
#include "nbiotdebug.h"

#include "nbiotmodemtools.h"

const char* NbiotModemTools::cmdNRB = "AT+NRB";
const char* NbiotModemTools::cmdCFUN1 = "AT+CFUN=1";
const char* NbiotModemTools::cmdCGDCONT_arg = "AT+CGDCONT=1,\"IP\",\"%s\"\r";
const char* NbiotModemTools::cmdCOPS_arg = "AT+COPS=1,2,\"%s\"\r";
const char* NbiotModemTools::cmdCFUN_arg = "AT+CFUN=%d\r";
const char* NbiotModemTools::cmdCFUNquery = "AT+CFUN?";
const char* NbiotModemTools::cmdCGATT0 = "AT+CGATT=0";
const char* NbiotModemTools::cmdCGATTquery = "AT+CGATT?";
const char* NbiotModemTools::respCGATTquery = "+CGATT:";
const char* NbiotModemTools::cmdCIMI = "AT+CIMI";
const char* NbiotModemTools::respOK = "OK";

#ifdef DEBUG_MODEM
#define FIVE_MINUTES 300000
#endif

bool NbiotModemTools::sendAtCommand(const char* command, char* response, size_t len, unsigned short timeout)
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

bool NbiotModemTools::reboot()
{
    bool ok = true;
    #ifdef DEBUG_MODEM
    nbiot::Timer t = nbiot::Timer(FIVE_MINUTES);
    #endif

    if(!m_cmd.sendCommand(cmdNRB))
    {
        ok = false;
    }
    if(ok)
    {
        if(!m_cmd.readUntil(respOK, sixtyfiveSeconds))
        {
            ok = false;
        }
    }
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);
#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
    debugPrintf("modem restart: %s - %dms\r\n", ((ok)?"ok":"fail"), t.expired());
#endif
    return ok;
}

bool NbiotModemTools::initialize()
{
    bool ok = true;
    #ifdef DEBUG_MODEM
    nbiot::Timer t = nbiot::Timer(FIVE_MINUTES);
    #endif


    if(ok)
    {
        if(!m_cmd.sendCommand(cmdCFUN1))
        {
            ok = false;
        }
    }

    if(ok)
    {
        if(!m_cmd.readUntil(respOK, tenSeconds))
        {
            ok = false;
        }
    }
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);

    if(ok)
    {
        if(MODEM_OFF == isModemOn())
        {
            ok = false;
        }
    }

    if(ok)
    {
        if(m_cmd.sendCommand(nbiot::string::Printf(cmdCGDCONT_arg, apn.c_str())))
        {
            if(!m_cmd.readResponse(REPLY_OK))
            {
                ok = false;
            }
        }
        else
        {
            ok = false;
        }
    }
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);
#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
    debugPrintf("modem init: %s - %dms\r\n", ((ok)?"ok":"fail"), t.expired());
#endif
    return ok;
}

bool NbiotModemTools::attach()
{
    bool ret = true;

    if(m_cmd.sendCommand(nbiot::string::Printf(cmdCOPS_arg, plmn.c_str())))
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

bool NbiotModemTools::detach()
{
    bool ret = true;

    if(ret)
    {
        // detach from gprs service
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

bool NbiotModemTools::isAttached()
{
    return ( 1 == getAttachState() );
}

int NbiotModemTools::getAttachState()
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

bool NbiotModemTools::getImsi(nbiot::string& imsi)
{
    bool ret = false;

    // get IMSI
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

const char* NbiotModemTools::getDeviceId()
{
    if(m_deviceId.empty())
    {
        getImsi(m_deviceId);
    }
    return m_deviceId.c_str();
}

NbiotModemTools::ModemState NbiotModemTools::switchModem(ModemState state)
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

NbiotModemTools::ModemState NbiotModemTools::isModemOn()
{
    ModemState ret = MODEM_OFF;

    if(m_cmd.sendCommand(cmdCFUNquery))
    {
        if(m_cmd.readResponse(REPLY_ANY, fiveSeconds))
        {
            nbiot::string response = m_cmd.getResponse();
            nbiot::StringList list = response.split(":");
            if(1 < list.count())
            {
                int index = list.count() - 1;
                if(1 == atoi(list[index].c_str()))
                {
                    ret = MODEM_ON;
                }
            }
        }
    }
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);
#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
    debugPrintf("is modem on?: %s\r\n", ((ret)?"on":"off"));
#endif
    return ret;
}

