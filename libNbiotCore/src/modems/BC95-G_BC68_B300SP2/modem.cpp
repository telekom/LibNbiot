/* ========================================================================
 * LibNbiot: modem.cpp
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

#include "nbiottimer.h"
#include "nbiotdebug.h"

#include "modem.h"


const char* Modem::cmdNRB = "AT+NRB";
const char* Modem::cmdNSONMI3 = "AT+NSONMI=3";
const char* Modem::cmdCEDRXS0 = "AT+CEDRXS=0,5";
const char* Modem::cmdNPSMR1 = "AT+NPSMR=1";
const char* Modem::respNPSMR = "+NPSMR:";
const char* Modem::cmdCSCON1 = "AT+CSCON=1";
const char* Modem::respCSCON = "+CSCON:";
const char* Modem::cmdQREGSWT2 = "AT+QREGSWT=2";
const char* Modem::cmdQREGSWTquery = "AT+QREGSWT?";
const char* Modem::respQREGSWTquery = "+QREGSWT:";
const char* Modem::cmdCOPS0 = "AT+COPS=0";
#ifdef MANUAL_CONTEXT
const char* Modem::cmdCGACT_arg = "AT+CGACT=%d,%d\r";
#endif

#ifdef DEBUG_MODEM
#define FIVE_MINUTES 300000
#endif



bool Modem::reboot()
{
    bool ok = true;
    #ifdef DEBUG_MODEM
    nbiot::Timer t = nbiot::Timer(FIVE_MINUTES);
    #endif

    if(!checkQRegSwt())
    {
        ok = setQRegSwt2();
    }

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

bool Modem::initialize()
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

    // switching some URCs
    if(ok)
    {
        // enable (new) socket-data-receive URC (including the data itself)
        if(!m_cmd.sendCommand(cmdNSONMI3))
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
    if(ok)
    {
        // switching off eDRX and the according URC
        if(!m_cmd.sendCommand(cmdCEDRXS0))
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

    // BC95-G and BC68 have an AUTOCONNECT feature, enabled by default
    // it reads all parameters from SIM
    // AT+CGDCONT is not necessary
    //
    // if you want to use another APN and to configure it manually with enabled AUTOCONNECT feature:
    // - use AT+CGDCONT as usual (cid:1)
    // - deactivate default-context (cid:0) with AT+CGACT=0,0
    // - activate the new PDP context with AT+CGACT=1,1
#ifdef MANUAL_CONTEXT
    if(ok)
    {
        if(m_cmd.sendCommand(nbiot::string::Printf(cmdCGDCONT_arg, m_apn.c_str())))
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
        if(ok)
        {
            if(m_cmd.sendCommand(nbiot::string::Printf(cmdCGACT_arg, 0,0)))
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
        if(ok)
        {
            if(m_cmd.sendCommand(nbiot::string::Printf(cmdCGACT_arg, 1,1)))
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
    }
#endif
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);
#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
    debugPrintf("modem init: %s - %dms\r\n", ((ok)?"ok":"fail"), t.expired());
#endif
    return ok;
}

bool Modem::attach()
{
    bool ret = true;

    // AT+COPS=0 activates PLMN autoselection
    if(m_cmd.sendCommand(cmdCOPS0))
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
        if( isAttached() )
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

bool Modem::setupPmUrcs()
{
    bool ok = true;

    // enable PSM mode-change URC
    if(!m_cmd.sendCommand(cmdNPSMR1))
    {
        ok = false;
    }
    if(ok)
    {
        if(!m_cmd.readUntil(respOK, tenSeconds))
        {
            ok = false;
        }
    }

    if(ok)
    {
        // enable connection mode-change URC
        if(!m_cmd.sendCommand(cmdCSCON1))
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

    if(ok)
    {
        m_cmd.addUrcFilter(respNPSMR, this, &Modem::parseNpsmrUrc);
        m_cmd.addUrcFilter(respCSCON, this, &Modem::parseCsconUrc);
    }
    return ok;
}

void Modem::parseNpsmrUrc(const char* strNpsmrUrc)
{
    if(m_pmHandler.attached())
    {
        // +NPSMR: urc reporting power saving mode status
        // +---------------------------------------------+
        // | +NPSMR:<mode> Integer indicating power mode |
        // |               0 Normal mode                 |
        // |               1 Power saving mode           |
        // +---------------------------------------------+
        nbiot::string response = strNpsmrUrc;

        size_t pos = response.find(':');
        if(nbiot::string::npos != pos)
        {
            pos++;
            while(' ' == response.at(pos)) // eat up spaces if there're any
            {
                pos++;
            }
            if(response.size() > pos)
            {
                nbiot::string number = response.substr(pos);
                m_pmResponse.psmStatus = static_cast<bool>(atoi(number.c_str()));
                m_pmResponse.changeFlag = CF_PSM;

                m_pmHandler(m_pmResponse);
            }
        }
    }
}

void Modem::parseCsconUrc(const char* strCsconUrc)
{
    if(m_pmHandler.attached())
    {
        // +CSCON: urc reporting signalling connection status
        // +-----------------------------------------------------+
        // | +CSCON:<mode> Int. The signalling connection status |
        // |               0 Idle                                |
        // |               1 Connected                           |
        // +-----------------------------------------------------+
        nbiot::string response = strCsconUrc;

        size_t pos = response.find(':');
        if(nbiot::string::npos != pos)
        {
            pos++;
            while(' ' == response.at(pos)) // eat up spaces if there're any
            {
                pos++;
            }
            if(response.size() > pos)
            {
                nbiot::string number = response.substr(pos);
                m_pmResponse.conStatus = static_cast<bool>(atoi(number.c_str()));
                m_pmResponse.changeFlag = CF_CON;

                m_pmHandler(m_pmResponse);
            }
        }
    }
}


bool Modem::checkQRegSwt()
{
    bool ret = false;
    if(m_cmd.sendCommand(cmdQREGSWTquery))
    {
        if(m_cmd.readUntil(respQREGSWTquery, tenSeconds))
        {
            nbiot::string response = m_cmd.getResponse();
#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
            debugPrintf("QREGSWT-Response: %s\r\n", response.c_str());
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
                    ret = (QREGSWT2 == atoi(number.c_str()));
                }
            }
        }
    }
#ifdef DEBUG_MODEM
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MODEM    ]\033[0m ");
#endif
    debugPrintf("Huawei IOT platform mode: %s\r\n", ret?"disabled":"enabled");
#endif
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);
    return ret;
}

bool Modem::setQRegSwt2()
{
    bool ok = true;

    if(!m_cmd.sendCommand(cmdQREGSWT2))
    {
        ok = false;
    }

    if(ok)
    {
        if(!m_cmd.readUntil(respOK, tenSeconds))
        {
            ok = false;
        }
    }
    m_cmd.readResponse(REPLY_IGNORE, oneSecond);

    return ok;
}





