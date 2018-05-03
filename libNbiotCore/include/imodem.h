/* ========================================================================
 * LibNbiot: imodem.h
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

#ifndef IMODEM_H
#define IMODEM_H

namespace nbiot
{
class string;
}

/*!
 * \brief The IModem class
 */
class IModem
{
public:
    /*!
     * \brief The ModemState enum
     */
    enum ModemState
    {
        MODEM_OFF = 0,
        MODEM_ON
    };

    virtual ~IModem() {}

    /*!
     * \brief reboot
     * \return
     */
    virtual bool reboot() = 0;
    /*!
     * \brief initialize
     * \return
     */
    virtual bool initialize() = 0;
    /*!
     * \brief attach
     * \return
     */
    virtual bool attach() = 0;
    /*!
     * \brief detach
     * \return
     */
    virtual bool detach() = 0;
    /*!
     * \brief isAttached
     * \return
     */
    virtual bool isAttached() = 0;
    /*!
     * \brief sendAtCommand
     * \param command
     * \param response
     * \param len
     * \param timeout
     * \return
     */
    virtual bool sendAtCommand(const char* command, char* response, size_t len, unsigned short timeout) = 0;
    /*!
     * \brief getImsi
     * \param imsi
     * \param usePw
     * \return
     */
    virtual bool getImsi(nbiot::string& imsi) = 0;
    /*!
     * \brief getDeviceId
     * \return
     */
    virtual const char* getDeviceId() = 0;
    /*!
     * \brief switchModem
     * \param state
     * \return
     */
    virtual ModemState switchModem(ModemState state) = 0;
    /*!
     * \brief isModemOn
     * \return
     */
    virtual ModemState isModemOn() = 0;

};
#endif // IMODEM_H
