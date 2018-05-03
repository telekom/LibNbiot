/* ========================================================================
 * LibNbiot: nbiotmodemtools.h
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

#ifndef NBIOTMODEMTOOLS_H
#define NBIOTMODEMTOOLS_H

#include "atcommands.h"
#include "nbiotstring.h"
#include "imodem.h"

/*!
 * \brief The NbiotModemTools class
 */
class NbiotModemTools : public IModem
{
public:
    /*!
     * \brief NbiotModemTools
     * \param serial
     */
    explicit NbiotModemTools(Serial& serial) :
        m_cmd(serial),
        apn(),
        apnuser(),
        apnpwd(),
        plmn(),
        m_deviceId()
    {}
    /*!
     * \brief ~NbiotModemTools
     */
    virtual ~NbiotModemTools(){}


    /*!
     * \brief reboot
     * \return
     */
    virtual bool reboot();
    /*!
     * \brief initialize
     * \return
     */
    virtual bool initialize();
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
     * \brief sendAtCommand
     * \param command
     * \param response
     * \param len
     * \param timeout
     * \return
     */
    virtual bool sendAtCommand(const char* command, char* response, size_t len, unsigned short timeout);
    /*!
     * \brief getImsi
     * \param imsi
     * \param usePw
     * \return
     */
    virtual bool getImsi(nbiot::string& imsi);
    /*!
     * \brief getDeviceId
     * \return
     */
    virtual const char* getDeviceId();
    /*!
     * \brief switchModem
     * \param state
     * \return
     */
    virtual ModemState switchModem(ModemState state);
    /*!
     * \brief isModemOn
     * \return
     */
    virtual ModemState isModemOn();

    /*!
     * \brief configSetApn
     * \param confApn
     */
    void configSetApn(const char* confApn) { apn = confApn; }
    /*!
     * \brief configSetApnUser
     * \param confApnUser
     */
    void configSetApnUser(const char* confApnUser) { apnuser = confApnUser; }
    /*!
     * \brief configSetApnPwd
     * \param confApnPwd
     */
    void configSetApnPwd(const char* confApnPwd) { apnpwd = confApnPwd; }
    /*!
     * \brief configSetOperMccMnc
     * \param confOperMccMnc
     */
    void configSetPlmn(const char* confPlmn) { plmn = confPlmn; }

    //! @todo error struct and lastError() method

private:
    NbiotModemTools();
    NbiotModemTools(const NbiotModemTools&);
    NbiotModemTools& operator=(const NbiotModemTools&);

    int getAttachState();

    AtCommands m_cmd;

    nbiot::string apn;
    nbiot::string apnuser;
    nbiot::string apnpwd;
    nbiot::string plmn;

    nbiot::string m_deviceId;

    static const unsigned int oneSecond = 1000;
    static const unsigned int threeSeconds = 3000;
    static const unsigned int fiveSeconds = 5000;
    static const unsigned int tenSeconds = 10000;
    static const unsigned int sixtyfiveSeconds = 65000;

    static const char* cmdNRB;
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
};

#endif // NBIOTMODEMTOOLS_H

