#include "nbiotcoreapp.h"

static NbiotCoreApp* nbiotCoreAppInstance = 0l;

NbiotCoreApp::NbiotCoreApp():
    m_time(),
    m_serial(serialDebugPrefix),
    m_modem(m_serial),
    m_network(m_serial),
    coreConfigError(CoreErrorNoConfig)
{}

NbiotCoreApp::~NbiotCoreApp() {}

NbiotCoreApp& NbiotCoreApp::getInstance()
{
    if(0l == nbiotCoreAppInstance)
    {
        nbiotCoreAppInstance = new NbiotCoreApp();
    }
    return *nbiotCoreAppInstance;
}

void NbiotCoreApp::destroy()
{
    if(0l != nbiotCoreAppInstance)
    {
        delete nbiotCoreAppInstance;
        nbiotCoreAppInstance = 0l;
    }
}

unsigned int NbiotCoreApp::nbiotCoreConfig(NbiotCoreConf* conf)
{
    unsigned int result = CoreNoError;

    if(CoreErrorNoConfig == coreConfigError)
    {
        if((FIFTY_HZ <= conf->tickFrequency) && (TEN_KHZ >= conf->tickFrequency))
        {
            m_time.setTickFrequency(conf->tickFrequency);
        }
        else
        {
            result |= CoreErrorTickFreq;
        }

        if(nullptr != conf->readstatus_fu)
        {
            m_serial.configSetReadStatus(conf->readstatus_fu);
        }
        else
        {
            result |= CoreErrorReadStatusFu;
        }

        if(nullptr != conf->readbyte_fu)
        {
            m_serial.configSetReadByte(conf->readbyte_fu);
        }
        else
        {
            result |= CoreErrorReadByteFu;
        }

        if(nullptr != conf->putchar_fu)
        {
            m_serial.configSetPutChar(conf->putchar_fu);
        }
        else
        {
            result |= CoreErrorPutCharFu;
        }

        if(nullptr != conf->writestatus_fu)
        {
            m_serial.configSetWriteStatus(conf->writestatus_fu);
        }
        else
        {
            result |= CoreErrorWriteStatusFu;
        }

        if((nullptr != conf->apn) && (0 < strlen(conf->apn)))
        {
            m_modem.configSetApn(conf->apn);
        }
        else
        {
            result |= CoreErrorApn;
        }

        if((nullptr != conf->apnUser) && (0 < strlen(conf->apnUser)))
        {
            m_modem.configSetApnUser(conf->apnUser);
        }
        else
        {
            result |= CoreWarnApnUser;
        }

        if((nullptr != conf->apnPwd) && (0 < strlen(conf->apnPwd)))
        {
            m_modem.configSetApnPwd(conf->apnPwd);
        }
        else
        {
            result |= CoreWarnApnPwd;
        }

        if((nullptr != conf->operMccMnc) && (0 < strlen(conf->operMccMnc)))
        {
            m_modem.configSetOperMccMnc(conf->operMccMnc);
        }

        if((nullptr != conf->imsi) && (0 < strlen(conf->imsi)))
        {
            m_modem.configSetImsi(conf->imsi);
        }

        if((nullptr != conf->imsiPwd) && (0 < strlen(conf->imsiPwd)))
        {
            m_modem.configSetImsiPwd(conf->imsiPwd);
        }

        coreConfigError = result;
    }
    else
    {
        result = CoreErrorMultipleConfig;
    }

    return result;
}

