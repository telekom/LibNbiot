/* ========================================================================
 * LibNbiot: serial.h
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

#ifndef SERIAL_H
#define SERIAL_H
#include <stdio.h> // for size_t and NULL
#include "readstatus.h"
#include "writestatus.h"

#include "nbiotdebug.h"
#include "nbiotstring.h"
#include "urcfilterlist.h"

/*!
 * \brief The Serial class
 */
class Serial
{
public:
    /*!
     * \brief The SerialError enum
     */
    enum SerialError
    {
        NoError = 0,
        ReadError,
        WriteError,
        LineParsingError,
        ReadTimeout,
        NoReadFunction,
        NoWriteFunction
    };

    /*!
     * \brief The SerialReadMode enum
     */
    enum SerialReadMode
    {
        Normal,
        Simple,
        Ignore
    };

    /*!
     * \brief Serial
     */
    Serial();
    /*!
     * \brief Serial
     * \param dbgprf
     */
    Serial(int dbgprf);
    /*!
     * \brief Serial
     * \param other
     */
    Serial(const Serial& other);

    /*!
     * \brief ~Serial
     */
    virtual ~Serial() {}

    /*!
     * \brief operator =
     * \param other
     * \return
     */
    Serial& operator=(const Serial& other);

    /*!
     * \brief configSetReadStatus
     */
    void configSetReadStatus(ReadStatus (*readstatus_fu)());
    /*!
     * \brief configSetReadByte
     */
    void configSetReadByte(unsigned char (*readbyte_fu)());
    /*!
     * \brief configSetPutChar
     */
    void configSetPutChar(void (*putchar_fu)(unsigned char));
    /*!
     * \brief configSetWriteStatus
     */
    void configSetWriteStatus(WriteStatus (*writestatus_fu)());

    /*!
     * \brief getError
     * \return
     */
    int getError() const { return serial_error; }
    /*!
     * \brief clearError
     */
    void clearError() { setError(NoError); }

    /*!
     * \brief readLine
     * \param line
     * \param timeout_ms
     * \param readMode
     * \return
     */
    size_t readLine(nbiot::string& line, unsigned short timeout_ms, SerialReadMode readMode=Normal);
    /*!
     * \brief readRaw
     * \param buffer
     * \param timeout_ms
     * \return
     */
    size_t readRaw(unsigned char* buffer, unsigned short timeout_ms);

    /*!
     * \brief write
     * \param c
     * \return
     */
    size_t write(char c);

    /*!
     * \brief write
     * \param buffer
     * \param size
     * \return
     */
    size_t write(const char* buffer, size_t size);
    /*!
     * \brief write
     * \param str
     * \return
     */
    size_t write(const nbiot::string& str);

    template<class UHC>
    /*!
     * \brief addUrcFilter
     * \param urc
     * \param uhc
     * \return
     */
    bool addUrcFilter(const char* urc, UHC* uhc, void (UHC::*urc_handler)(const char*))
    {
        int index = -1;
        bool ret = false;

        if(0 > (index = m_filterList.indexOf(urc)))
        {
            nbiot::UrcFilter filter(urc);
            filter.setHandler(uhc, urc_handler);
            m_filterList.append(filter);
            ret = true;
#ifdef DEBUG
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;35m[ FILTER   ]\033[0m ");
#endif
            debugPrintf("append URC \"%s\" to filter-list\r\n", urc);
#endif
        }
        else
        {
            m_filterList[index].setHandler(uhc, urc_handler);
#ifdef DEBUG
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;35m[ FILTER   ]\033[0m ");
#endif
            debugPrintf("replace handler for URC \"%s\"@%d in filter-list\r\n", urc, index);
#endif
        }

        return ret;
    }

    /*!
     * \brief addUrcFilter
     * \param urc
     * \return
     */
    bool addUrcFilter(const char* urc, void (*urc_handler)(const char*));
    /*!
     * \brief removeUrcFilter
     * \param urc
     * \return
     */
    bool removeUrcFilter(const char* urc);

#ifdef DEBUG
    void dumpFilterList()
    {
        //for(int i = 0; i < m_filterList.count(); ++i)
        for(util::ListIterator<nbiot::UrcFilter> it = m_filterList.cbegin(); it != m_filterList.cend(); ++it)
        {
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;35m[ FILTER   ]\033[0m ");
#endif

            debugPrintf("URC: %s\r\n", (((*it).isValid())?((*it).urc.c_str()):"[INVALID]"));
        }
    }
#endif

    /*!
     * \brief clearFilter
     */
    void clearFilter() { m_filterList.clear(); }
    /*!
     * \brief hasFilter
     * \return
     */
    bool hasFilter() const { return !m_filterList.isEmpty(); }

    /*!
     * \brief setDebugPrefix
     * \param prefix
     */
    void setDebugPrefix(int prefix) { dbg_prefix = prefix; }
    /*!
     * \brief getDebugPrefix
     * \return
     */
    int getDebugPrefix() const { return dbg_prefix; }

private:
    ReadStatus (*serialReadStatus)();
    unsigned char (*serialReadByte)();
    void (*serialPutChar)(unsigned char);
    WriteStatus (*serialWriteStatus)();

    void setError(int err) { serial_error = err; }
    void waitForTx() const;

    int serial_error;

    nbiot::UrcFilterList m_filterList;

    int dbg_prefix;

    static const unsigned long txBufferWaitTimeout = 1000;
};

#endif // SERIAL_H
