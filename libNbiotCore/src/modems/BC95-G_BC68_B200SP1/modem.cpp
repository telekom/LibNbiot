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






