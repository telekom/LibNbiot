/* ========================================================================
 * LibNbiot: serial.cpp
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

#include "nbiottimer.h"
#include "serial.h"

#include "nbiotdebug.h"

typedef enum ResponseLine
{
    line_empty,
    line_data,
    line_complete
} LineStatus;

Serial::Serial() :
    serialReadStatus(nullptr),
    serialReadByte(nullptr),
    serialPutChar(nullptr),
    serialWriteStatus(nullptr),
    serial_error(NoError),
    m_filterList(),
    dbg_prefix(0)
{
}

Serial::Serial(int dbgprf) :
    serialReadStatus(nullptr),
    serialReadByte(nullptr),
    serialPutChar(nullptr),
    serialWriteStatus(nullptr),
    serial_error(NoError),
    m_filterList(),
    dbg_prefix(dbgprf)
{
}

Serial::Serial(const Serial& other) :
    serialReadStatus(other.serialReadStatus),
    serialReadByte(other.serialReadByte),
    serialPutChar(other.serialPutChar),
    serialWriteStatus(other.serialWriteStatus),
    serial_error(other.serial_error),
    m_filterList(other.m_filterList),
    dbg_prefix(0)
{
}

Serial& Serial::operator=(const Serial& other)
{
    serialReadStatus = other.serialReadStatus;
    serialReadByte = other.serialReadByte;
    serialPutChar = other.serialPutChar;
    serialWriteStatus = other.serialWriteStatus;
    serial_error = other.serial_error;
    m_filterList = other.m_filterList;
    dbg_prefix = other.dbg_prefix;

    return *this;
}

void Serial::configSetReadStatus(ReadStatus (*readstatus_fu)())
{
    if(nullptr != readstatus_fu)
    {
        serialReadStatus = readstatus_fu;
    }
    else
    {
        setError(NoReadFunction);
    }
}

void Serial::configSetReadByte(unsigned char (*readbyte_fu)())
{
    if(nullptr != readbyte_fu)
    {
        serialReadByte = readbyte_fu;
    }
    else
    {
        setError(NoReadFunction);
    }
}

void Serial::configSetPutChar(void (*putchar_fu)(unsigned char))
{
    if(nullptr != putchar_fu)
    {
        serialPutChar = putchar_fu;
    }
    else
    {
        setError(NoWriteFunction);
    }
}

void Serial::configSetWriteStatus(WriteStatus (*writestatus_fu)())
{
    if(nullptr != writestatus_fu)
    {
        serialWriteStatus = writestatus_fu;
    }
}

void Serial::waitForTx() const
{
    if(nullptr != serialWriteStatus)
    {
        if(tx_empty != serialWriteStatus())
        {
            nbiot::Timer timer = nbiot::Timer(txBufferWaitTimeout);
            unsigned long expired = timer.expired();
            while(0 < timer.remaining())
            {
                if(expired < timer.expired())
                {
                    expired = timer.expired();
                    if(tx_empty == serialWriteStatus())
                    {
                        break;
                    }
                }
            }
        }
    }
}

size_t Serial::readLine(nbiot::string& line, unsigned short timeout_ms, SerialReadMode readMode)
{
    size_t index = 0;

    #ifdef DEBUG_SERIAL_VERBOSE
    debugPrintf("%d:RL ", dbg_prefix);
    #endif
    if((nullptr != serialReadStatus) && (nullptr != serialReadByte))
    {
        nbiot::Timer timer = nbiot::Timer(timeout_ms);
        LineStatus lineStatus = line_empty;
        if(Simple == readMode)
        {
            lineStatus = line_data;
        }
        waitForTx();
        while(0 < timer.remaining())
        {
            unsigned char uartInChar;
            size_t n = readRaw(&uartInChar, static_cast<unsigned short>(timer.remaining()));

            /* If byte received */
            if(1 == n && 0u != uartInChar)
            {
                #ifdef DEBUG_SERIAL_VERBOSE
                if((0x20 < uartInChar) && (0x7f > uartInChar))
                {
                    debugPrintf("[%d]:%c ", index, uartInChar);
                }
                else
                {
                    debugPrintf("[%d]:%02X ", index, uartInChar);
                }
                #endif
                if('\r' == uartInChar )
                {
                    /* ignore */
                }
                else if('\n' == uartInChar && line_empty == lineStatus)
                {
                    lineStatus = line_data;
                }
                else if('\n' == uartInChar && line_data == lineStatus)
                {
                    if(0 < index) // ignore empty lines
                    {
                        lineStatus = line_complete;
                        if(hasFilter())
                        {
                            #ifdef DEBUG_SERIAL
                            debugPrintf("\r\n%d FILTER(s)\r\n", m_filterList.count());
                            #endif
                            int filterIndex = -1;
                            if(0 <= (filterIndex = m_filterList.indexOf(line)))
                            {
                                // FILTER for unsolicited messages
                                //
                                // the incoming line matches and is filtered:
                                // - the filter-urc-handler is called with the matching line
                                // - the line is emptied
                                // - reading continues with the next incoming line
                                nbiot::UrcFilter filter = m_filterList[filterIndex];
                                #ifdef DEBUG_SERIAL
                                debugPrintf("MATCH: %s found in %s\r\n", filter.urc.c_str(), line.c_str());
                                #endif
                                filter.handler(line.c_str());
                                line = "";
                                index = 0;
                                lineStatus = line_data;
                            }
                            else
                            {
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else if(line_data == lineStatus)
                {
                    line.push_back(static_cast<char>(uartInChar));
                    index++;
                }
                else
                {
                    /* ignore data before 1st linefeed */
                }
            }
            if(0 == timer.remaining())
            {
                setError(ReadTimeout);
                break;
            }
        }
    }
    else
    {
        setError(NoReadFunction);
    }

    if(Ignore == readMode)
    {
        line = "";
    }

    #ifdef DEBUG_SERIAL_VERBOSE
    debugPrintf("%d:[%d] (%d) \r\n", dbg_prefix, index, timeout_ms);
    #endif
    return index;
}

size_t Serial::readRaw(unsigned char* buffer, unsigned short timeout_ms)
{
    size_t result = 0;
    unsigned char uartInChar;

    #ifdef DEBUG_SERIAL_RAW
    debugPrintf("%d:raw(%d): ", dbg_prefix, timeout_ms);
    #endif
    if((nullptr != serialReadStatus) && (nullptr != serialReadByte))
    {
        nbiot::Timer timer = nbiot::Timer(timeout_ms);
        while(0 < timer.remaining())
        {
            ReadStatus status = serialReadStatus();
            if(0 != (rx_avail & status))
            {
                uartInChar = serialReadByte();
                #ifdef DEBUG_SERIAL_RAW
                debugPrintf("%c", uartInChar);
                #endif
                if(0 == (rx_error & status))
                {
                    if(nullptr != buffer)
                    {
                        buffer[0] = uartInChar;
                    }
                    result = 1;
                    /* one byte buffer full: break; */
                    break;
                }
            }
            if(0 == timer.remaining())
            {
                setError(ReadTimeout);
                #ifdef DEBUG_SERIAL_RAW
                debugPrintf("timeout");
                #endif
                break;
            }
        }
    }
    else
    {
        setError(NoReadFunction);
    }

    #ifdef DEBUG_SERIAL_RAW
    debugPrintf(" %d:[%d]\r\n", dbg_prefix, result);
    #endif
    return result;
}

size_t Serial::write(char c)
{
    size_t rc = 0;
    if(nullptr != serialPutChar)
    {
        serialPutChar(static_cast<unsigned char>(c));
        rc = 1;
    }
    else
    {
        setError(NoWriteFunction);
    }
    return rc;
}

size_t Serial::write(const char* buffer, size_t size)
{
    size_t n = 0;

    while(size--)
    {
        if(write(*buffer++))
        {
            n++;
        }
        else
        {
            break;
        }
    }
    return n;
}

size_t Serial::write(const nbiot::string& str)
{
    size_t n = 0;

    size_t size = str.size();
    while(size--)
    {
        if(write((str.at(n))))
        {
            n++;
        }
        else
        {
            break;
        }
    }
    return n;
}

bool Serial::addUrcFilter(const char* urc, void (*urc_handler)(const char*))
{
    int index = -1;
    bool ret = false;

    if(0 > (index = m_filterList.indexOf(urc)))
    {
        m_filterList.append(nbiot::UrcFilter(urc, urc_handler));
        ret = true;
#ifdef DEBUG_FILTER
#ifdef DEBUG_COLOR
        debugPrintf("\033[0;35m[ FILTER   ]\033[0m ");
#endif
        debugPrintf("append URC \"%s\" to filter-list\r\n", urc);
#endif
    }
    else
    {
        m_filterList[index].setHandler(urc_handler);
#ifdef DEBUG_FILTER
#ifdef DEBUG_COLOR
        debugPrintf("\033[0;35m[ FILTER   ]\033[0m ");
#endif
        debugPrintf("replace handler for URC \"%s\"@%d in filter-list\r\n", urc, index);
#endif
    }
    dumpFilterList();
    return ret;
}

bool Serial::removeUrcFilter(const char* urc)
{
    int index = -1;
    bool ret = false;

#ifdef DEBUG_FILTER
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;35m[ FILTER   ]\033[0m ");
#endif
    debugPrintf("remove URC-Filter: %s\r\n", urc);
#endif

    if(0 <= (index = m_filterList.indexOf(urc)))
    {
        m_filterList.remove(index);
        ret = true;
    }
    dumpFilterList();
    return ret;
}

void Serial::dumpFilterList()
{
#ifdef DEBUG_FILTER
    for(util::ListIterator<nbiot::UrcFilter> it = m_filterList.cbegin(); it != m_filterList.cend(); ++it)
    {
#ifdef DEBUG_COLOR
        debugPrintf("\033[0;35m[ FILTER   ]\033[0m ");
#endif

        debugPrintf("URC: %s\r\n", (((*it).isValid())?((*it).urc.c_str()):"[INVALID]"));
    }
#endif
}
