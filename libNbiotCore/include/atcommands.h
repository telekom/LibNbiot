/* ========================================================================
 * LibNbiot: atcommands.h
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

#ifndef ATCOMMANDS_H
#define ATCOMMANDS_H
#include "nbiotstring.h"
#include "serial.h"

class Serial;

/*!
 * \brief The ExpectedReply enum is used to define settings for the \fn readResponse method
 */
typedef enum expectedReply
{
    REPLY_OK,            /*!< just "OK" */
    REPLY_ANY,           /*!< any response-line followed by "OK" */
    REPLY_COPY,          /*!< all response-lines will be copied to 'response' */
    REPLY_EXACT,         /*!< exactly one response-line that differs from "OK" */
    REPLY_IGNORE,        /*!< every response will be ignored */
    REPLY_NONE          /*!< there must be no response */
} ExpectedReply;

/*!
 * \brief The AtCommands class is an object to send and receive AT-commands and their responses via an underlaying serial connection.
 */
class AtCommands
{
public:
    /*!
     * \brief Constructs an AtCommands object with an existing serial connection.
     * \param s the underlaying serial object
     */
    explicit AtCommands(Serial& s);
    virtual ~AtCommands();

    /*!
     * \brief sendCommand Send an AT-command to the connected modem. This is an overloaded function.
     * \param command The AT-command
     * \return false on serial error or true otherwise
     */
    bool sendCommand(const char* command);
    /*!
     * \brief sendCommand Send an AT-command to the connected modem.
     * \param cmd The AT-command
     * \return false on serial error or true otherwise
     */
    bool sendCommand(const nbiot::string& cmd);
    /*!
     * \brief readResponse Reads the response from modem.
     * \param expected ExpectedReply enum defining the expected type of response \sa expectedReply
     * \param timeout_ms maximum time to wait for the response
     * \return true on success or false on error
     */
    bool readResponse(ExpectedReply expected, unsigned short timeout_ms=defaultTimeout);
    /*!
     * \brief readUntil Reads the response-lines from the underlaying serial connection, compares them to the given string \c expected and stops execution at a match or after timeout.
     * \param expected The string to wait for
     * \param timeout_ms maximum time to wait for a matching response line
     * \param exact If true the response and the string \c expected need to be identical to match. Otherwise the response need to contain the string \c expected.
     * \return true on success or false on error
     */
    bool readUntil(const char* expected, unsigned short timeout_ms=defaultTimeout, bool exact=false);
    /*!
     * \brief getResponse
     * \return
     */
    const char* getResponse() const { return response.c_str(); }

    template<class UHC>
    /*!
     * \brief addUrcFilter Adds a filter for an unsolicited response code (URC) to the underlaying serial connection
     * \param urc the unsolicited response code to find
     * \param uhc the class holding the handler method
     * \param urc_handler the handler method that will be called in case of a filter match. It will receive the response-line as parameter.
     * \return true if a new filter was added
     */
    bool addUrcFilter(const char* urc, UHC* uhc, void (UHC::*urc_handler)(const char*))
    {
        return serial.addUrcFilter(urc, uhc, urc_handler);
    }

    /*!
     * \brief addUrcFilter Adds a filter for an unsolicited response code (URC) to the underlaying serial connection
     * \param urc the unsolicited response code to find
     * \param urc_handler the handler function that will be called in case of a filter match. It will receive the response-line as parameter.
     * \return true if a new filter was added
     */
    bool addUrcFilter(const char* urc, void (*urc_handler)(const char*))
    {
        return serial.addUrcFilter(urc, urc_handler);
    }

    /*!
     * \brief removeUrcFilter Removes an URC-filter from the underlaying serial connection
     * \param urc the unsolicited response code of which the filter should be removed
     * \return true if the filter could be removed
     */
    bool removeUrcFilter(const char* urc)
    {
        return serial.removeUrcFilter(urc);
    }

    /*!
     * \brief clearFilter Deletes all URC-filters from the underlaying serial connection.
     */
    void clearFilter()
    {
        serial.clearFilter();
    }

    /*!
     * \brief hasFilter Checks if the the underlaying serial connection has one or more URC-filters installed
     * \return true if there are URC-filters installed
     */
    bool hasFilter() const
    {
        return serial.hasFilter();
    }

    //! @todo configurable response-size-limit for REPLY_COPY
    
    int dbg_prefix;
    Serial &serial;


private:
    AtCommands(const AtCommands&);
    AtCommands& operator=(const AtCommands&);


    nbiot::string response;

    static const unsigned short defaultTimeout = 500;
    static const unsigned long oneSecond = 1000;
    static const nbiot::string okReply;
    static const nbiot::string errReply;
    static const unsigned short replyMaxLen = 255;
    static const size_t PRINTF_BUFFER_SIZE = 80;
};

#endif // ATCOMMANDS_H

