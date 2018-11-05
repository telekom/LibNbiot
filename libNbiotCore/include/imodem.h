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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ========================================================================
*/

#ifndef IMODEM_H
#define IMODEM_H

#include "responses.h"
#include "imodemnotify.h"

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
     * 
     * TODO: has to be removed/exchanged. Not all modules can be rebooted by AT-Commands.
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
    /*!
     * \brief configSetApn
     * \param confApn
     */
    virtual void configSetApn(const char* confApn) = 0;
    /*!
     * \brief configSetApnUser
     * \param confApnUser
     */
    virtual void configSetApnUser(const char* confApnUser) = 0;
    /*!
     * \brief configSetApnPwd
     * \param confApnPwd
     */
    virtual void configSetApnPwd(const char* confApnPwd) = 0;
    /*!
     * \brief configSetOperMccMnc
     * \param confPlmn
     */
    virtual void configSetPlmn(const char* confPlmn) = 0;

    /*!
     * \brief installCeregHandler
     * \param hc
     * \param ceregHandler
     */
    virtual void installCeregHandler(IModemNotify* hc, void (IModemNotify::*ceregHandler) (const CeregResponse&)) = 0;
    /*!
     * \brief setupCeregUrc
     * \return
     */
    virtual bool setupCeregUrc() = 0;

    /*!
     * \brief installPmHandler
     * \param hc
     * \param pmHandler
     */
    virtual void installPmHandler(IModemNotify* hc, void (IModemNotify::*pmHandler) (const PmResponse&)) = 0;
    /*!
     * \brief setupPmUrcs
     * \return
     */
    virtual bool setupPmUrcs() = 0;

protected:

};
#endif // IMODEM_H
