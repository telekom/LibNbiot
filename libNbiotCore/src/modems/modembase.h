/* ========================================================================
 * LibNbiot: modembase.h
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

#ifndef MODEMBASE_H
#define MODEMBASE_H

#include <stdlib.h>

#include "imodem.h"
#include "atcommands.h"
#include "nbiotstring.h"



/*!
 * \brief The ModemBase class
 */
class ModemBase : public IModem
{
public:

    /*!
     * \brief C'tor
     */
    explicit ModemBase(Serial& serial);

    /*!
     * \brief D'tor
     */  
    virtual ~ModemBase() noexcept {}

    /*!
     * \brief sendAtCommand
     * \param command
     * \param response
     * \param len
     * \param timeout
     * \return
     */
    bool sendAtCommand(const char* command, char* response, size_t len, unsigned short timeout);

    /*!
     * \brief attach
     * \return
     */   
    virtual bool attach();

    /*!
     * \brief detach
     * \return
     */

    virtual bool detach();

    /*!
     * \brief isAttached
     * \return
     */
    virtual bool isAttached();

    /*!
     * \brief Return the International Mobile Subscription Identifier (IMSI) of the device.
     * \return
     */
    virtual bool getImsi(nbiot::string& imsi);


    /*!
     * \brief Return the DeviceId.
     * \return
     */ 
    virtual const char* getDeviceId();

    /*!
     * \brief switchModule
     * \param state
     * \return
     */
    virtual ModemBase::ModemState switchModem(IModem::ModemState state);

    /*!
     * \brief isModemOn
     * \return
     */
    virtual ModemBase::ModemState isModemOn();

    /*!
     * \brief configSetApn
     * \param confApn
     */
    void configSetApn(const char* confApn) { m_apn = confApn; }
    /*!
     * \brief configSetApnUser
     * \param confApnUser
     */
    void configSetApnUser(const char* confApnUser) { m_apnUser = confApnUser; }
    /*!
     * \brief configSetApnPwd
     * \param confApnPwd
     */
    void configSetApnPwd(const char* confApnPwd) { m_apnPwd = confApnPwd; }
    /*!
     * \brief configSetOperMccMnc
     * \param confOperMccMnc
     */
    void configSetPlmn(const char* confPlmn) { m_plmn = confPlmn; }    

protected:

    AtCommands m_cmd;    

    nbiot::string m_plmn;
    nbiot::string m_apn;
    nbiot::string m_apnUser;
    nbiot::string m_apnPwd;
    nbiot::string m_deviceId;

    static const unsigned int oneSecond = 1000;
    static const unsigned int threeSeconds = 3000;
    static const unsigned int fiveSeconds = 5000;
    static const unsigned int tenSeconds = 10000;
    static const unsigned int sixtyfiveSeconds = 65000;

    static const char* cmdCFUN1;
    static const char* cmdCGDCONT_arg;
    static const char* cmdCOPS_arg;
    static const char* cmdCFUN_arg;
    static const char* cmdCFUNquery;
    static const char* cmdCGATT0;
    static const char* cmdCGATTquery;
    static const char* respCGATTquery;
    static const char* cmdCIMI;
    static const char* respOK;
    static const char* cmdCGMIquery; 
    static const char* respCFUNquery;       

private:

    ModemBase() = delete;
    ModemBase(const ModemBase&) = delete;
    ModemBase& operator=(const ModemBase&) = delete;

    int getAttachState();

};
#endif // MODEMBASE_H
