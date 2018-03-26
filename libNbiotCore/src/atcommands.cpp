/* ========================================================================
 * LibNbiot: atcommands.cpp
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

#include "nbiottimer.h"
#include "nbiotdebug.h"
#include "atcommands.h"

const nbiot::string AtCommands::okReply = "OK";
const nbiot::string AtCommands::errReply = "ERROR";

AtCommands::AtCommands(Serial &s) :
    dbg_prefix(s.getDebugPrefix()),
    serial(s),
    response()
{
}

AtCommands::~AtCommands()
{
}

bool AtCommands::sendCommand(const char* command)
{
    nbiot::string cmd = command;
    cmd += '\r';
    return sendCommand(cmd);
}

bool AtCommands::sendCommand(const nbiot::string& cmd)
{
    bool result = false;

    serial.clearError();

    size_t sz = serial.write(cmd);
    #ifdef DEBUG_ATCOMMANDS
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ATCOMMANDS]\033[0m ");
#endif
    debugPrintf("%d:s(%d): %s\r\n", dbg_prefix, sz, cmd.c_str());
    #endif
    if((cmd.size() == sz) && (Serial::NoError == serial.getError()))
    {
        result = true;
    }
    return result;
}


bool AtCommands::readResponse(ExpectedReply expected, unsigned short timeout_ms)
{
    bool result = false;

    serial.clearError();
    response = "";

    switch(expected)
    {
        case REPLY_OK:
        {
            size_t n = serial.readLine(response, timeout_ms);
            #ifdef DEBUG_ATCOMMANDS
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ATCOMMANDS]\033[0m ");
#endif
            debugPrintf("%d:r(%d): %s\r\n", dbg_prefix, n, response.c_str());
            #endif
            if((okReply.size() == n) && (Serial::NoError == serial.getError()))
            {
                result = (response == okReply);
            }
            break;
        }
        case REPLY_ANY:
        {
            size_t i = serial.readLine(response, timeout_ms);
            #ifdef DEBUG_ATCOMMANDS
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ATCOMMANDS]\033[0m ");
#endif
            debugPrintf("%d:r(%d): %s e=%d\r\n", dbg_prefix, i, response.c_str(), serial.getError());
            #endif
            if((0 < i) && (Serial::NoError == serial.getError()))
            {
                nbiot::string ok;
                size_t n = serial.readLine(ok, timeout_ms, Serial::Simple);
                #ifdef DEBUG_ATCOMMANDS
#ifdef DEBUG_COLOR
                debugPrintf("\033[0;32m[ATCOMMANDS]\033[0m ");
#endif
                debugPrintf("%d:r(%d): %s\r\n", dbg_prefix, n, ok.c_str());
                #endif
                if((okReply.size() == n) && (Serial::NoError == serial.getError()))
                {
                    result = (ok == okReply);
                }
            }
            if(!result)
            {
                response = "";
            }
            break;
        }
        case REPLY_COPY:
        {
            nbiot::string resp;
            nbiot::Timer timer = nbiot::Timer(timeout_ms);
            do
            {
                resp = "";
                size_t i = serial.readLine(resp, static_cast<unsigned short>(timer.remaining()), Serial::Simple);
                if(0 < i)
                {
#ifdef DEBUG_ATCOMMANDS
#ifdef DEBUG_COLOR
                    debugPrintf("\033[0;32m[ATCOMMANDS]\033[0m ");
#endif
                    debugPrintf("%d:r(%d): %s e=%d\r\n", dbg_prefix, i, resp.c_str(), serial.getError());
#endif
                    result = true;
                    response += resp;
                    response += "\r\n";
                    if((resp == okReply) || (resp == errReply))
                    {
                        break;
                    }
                    timer.clear();
                    timer.start(oneSecond);
                }
                else
                {
                    break;
                }
            } while(0 < timer.remaining());
            break;
        }
        case REPLY_EXACT:
        {
            size_t i = serial.readLine(response, timeout_ms);
            #ifdef DEBUG_ATCOMMANDS
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ATCOMMANDS]\033[0m ");
#endif
            debugPrintf("%d:r(%d): %s e=%d\r\n", dbg_prefix, i, response.c_str(), serial.getError());
            #endif
            if((0 < i) && (Serial::NoError == serial.getError()))
            {
                result = true;
            }
            break;
        }
        case REPLY_NONE:
        {
            // this will take the full timeout time to wait for possible input or break after the first char
            int n = serial.readRaw(nullptr, timeout_ms);
            if((0 == n) && ((Serial::NoError == serial.getError()) || (Serial::ReadTimeout == serial.getError())))
            {
                result = true;
            }
            break;
        }
        case REPLY_IGNORE:
        // FALLTHRU
        default:
        {
            nbiot::string resp;
            nbiot::Timer timer = nbiot::Timer(timeout_ms);
            while(0 < serial.readLine(resp, timer.remaining(), Serial::Ignore))
            {
                if(Serial::NoError != serial.getError())
                {
                    break;
                }
            }
            if(Serial::NoError == serial.getError())
            {
                result = true;
            }
            break;
        }
    }
    return result;
}

bool AtCommands::readUntil(const char* expected, unsigned short timeout_ms, bool exact)
{
    bool result = false;

    serial.clearError();
    response = "";

    #ifdef DEBUG_ATCOMMANDS
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ATCOMMANDS]\033[0m ");
#endif
    debugPrintf("%d:AtCommands::readUntil(%s, %d)\r\n", dbg_prefix, expected, timeout_ms);
    #endif

    nbiot::Timer timer = nbiot::Timer(timeout_ms);
    while((0 < timer.remaining()) && (Serial::NoError == serial.getError()))
    {
        serial.readLine(response, static_cast<unsigned short>(timer.remaining()), Serial::Simple);
        if( ((!exact) && (0 == response.find(expected))) || ((exact) && (response == expected)) )
        {
            #ifdef DEBUG_ATCOMMANDS
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ATCOMMANDS]\033[0m ");
#endif
            debugPrintf("%d:r(%d)*: %s\r\n", dbg_prefix, response.size(), response.c_str());
            #endif
            result = true;
            break;
        }
        else
        {
            #ifdef DEBUG_ATCOMMANDS
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ATCOMMANDS]\033[0m ");
#endif
            debugPrintf("%d:r(%d)-: %s\r\n", dbg_prefix, response.size(), response.c_str());
            #endif
        }
        response = "";
    }

    #ifdef DEBUG_ATCOMMANDS
    if(0 == timer.remaining())
        debugPrintf("%d:AtCommands::readUntil(%s, %d) - TIMEOUT\r\n", dbg_prefix, expected, timeout_ms);
    #endif

    return result;
}

