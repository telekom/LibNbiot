/* ========================================================================
 * LibNbiot: nbiotcoreapp.cpp
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

#include "nbiotcoreapp.h"

static NbiotCoreApp* nbiotCoreAppInstance = nullptr;

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
    if(nullptr == nbiotCoreAppInstance)
    {
        nbiotCoreAppInstance = new NbiotCoreApp();
    }
    return *nbiotCoreAppInstance;
}

void NbiotCoreApp::destroy()
{
    if(nullptr != nbiotCoreAppInstance)
    {
        delete nbiotCoreAppInstance;
        nbiotCoreAppInstance = nullptr;
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

        if((nullptr != conf->plmn) && (0 < strlen(conf->plmn)))
        {
            m_modem.configSetPlmn(conf->plmn);
        }

        coreConfigError = result;
    }
    else
    {
        result = CoreErrorMultipleConfig;
    }

    return result;
}

