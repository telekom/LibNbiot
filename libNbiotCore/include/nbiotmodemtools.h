/* ==============================================================
 * File Name: nbiotmodemtools.h
 *
 * Author: Edgar Hindemith
 *
 * Copyright T-Systems International GmbH, 2017
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF T-Systems.
 *
 * ==============================================================
 */
#ifndef NBIOTMODEMTOOLS_H
#define NBIOTMODEMTOOLS_H

#include "atcommands.h"
#include "nbiotstring.h"
#include "imodem.h"

class NbiotModemTools : public IModem
{
public:
    explicit NbiotModemTools(Serial& serial) :
        m_cmd(serial),
        apn(),
        apnuser(),
        apnpwd(),
        operMccMnc(),
        m_imsi(),
        m_imsipwd(),
        m_deviceId()
    {}
    virtual ~NbiotModemTools(){}


    virtual bool reboot();
    virtual bool initialize();
    virtual bool attach();
    virtual bool detach();
    virtual bool isAttached();
    virtual bool sendAtCommand(const char* command, char* response, size_t len, unsigned short timeout);
    virtual bool getImsi(nbiot::string& imsi, bool usePw = false);
    virtual const char* getDeviceId();
    virtual ModemState switchModem(ModemState state);
    virtual ModemState isModemOn();

    void configSetApn(const char* confApn) { apn = confApn; }
    void configSetApnUser(const char* confApnUser) { apnuser = confApnUser; }
    void configSetApnPwd(const char* confApnPwd) { apnpwd = confApnPwd; }
    void configSetOperMccMnc(const char* confOperMccMnc) { operMccMnc = confOperMccMnc; }

    void configSetImsi(const char* confImsi) { m_imsi = confImsi; }
    void configSetImsiPwd(const char* confImsiPwd) { m_imsipwd = confImsiPwd; }

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
    nbiot::string operMccMnc;

    nbiot::string m_imsi;
    nbiot::string m_imsipwd;
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

