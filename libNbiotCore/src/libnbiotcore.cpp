/* ========================================================================
 * LibNbiot: libnbiotcore.cpp
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

#include "nbiottime.h"
#include "nbiotmodemtools.h"
#include "nbiotcoreapp.h"

#include "libnbiotcore.h"

void tick()
{
   NbiotCoreApp::getInstance().getTimeInstance().tick();
}

void delay(unsigned short ms)
{
    NbiotCoreApp::getInstance().getTimeInstance().delay(ms);
}

long long getMillis()
{
    return NbiotCoreApp::getInstance().getTimeInstance().getMillis();
}

void updateMillis(unsigned long ms)
{
     NbiotCoreApp::getInstance().getTimeInstance().addMillis(ms);
}

time_t getTime()
{
    return NbiotCoreApp::getInstance().getTimeInstance().getTime();
}

void setTime(time_t t)
{
    NbiotCoreApp::getInstance().getTimeInstance().setTime(t);
}

unsigned int nbiotCoreConfig(NbiotCoreConf* conf)
{
    return NbiotCoreApp::getInstance().nbiotCoreConfig(conf);
}

unsigned char sendAtCommand(const char* command, char* response, size_t len, unsigned short timeout)
{
    unsigned char ret = 0;
    if(1 == NbiotCoreApp::getInstance().getModemInstance().sendAtCommand(command, response, len, timeout))
    {
        ret = 1;
    }

    return ret;
}

unsigned char addUrcFilter(const char* urc, void (*urc_handler)(const char*))
{
    unsigned char ret = 0;
    if(1 == NbiotCoreApp::getInstance().addUrcFilter(urc, urc_handler))
    {
        ret = 1;
    }

    return ret;
}

unsigned char removeUrcFilter(const char* urc)
{
    unsigned char ret = 0;
    if(1 == NbiotCoreApp::getInstance().removeUrcFilter(urc))
    {
        ret = 1;
    }

    return ret;
}

unsigned char isNbiotAttached()
{
    unsigned char ret = 0;

    if(1 == NbiotCoreApp::getInstance().getModemInstance().isAttached())
    {
        ret = 1;
    }

    return ret;
}

unsigned char nbiotAttach()
{
    unsigned char ret = 0;

    if(1 == NbiotCoreApp::getInstance().getModemInstance().attach())
    {
        ret = 1;
    }

    return ret;
}

unsigned char nbiotDetach()
{
    unsigned char ret = 0;

    if(1 == NbiotCoreApp::getInstance().getModemInstance().detach())
    {
        ret = 1;
    }

    return ret;
}

unsigned char nbiotModemRestart()
{
    unsigned char ret = 0;

    if(1 == NbiotCoreApp::getInstance().getModemInstance().reboot())
    {
        ret = 1;
    }

    return ret;
}

unsigned char nbiotModemInit()
{
    unsigned char ret = 0;

    if(1 == NbiotCoreApp::getInstance().getModemInstance().initialize())
    {
        ret = 1;
    }

    return ret;
}

const char* getNbiotDeviceId()
{
    return NbiotCoreApp::getInstance().getModemInstance().getDeviceId();
}

unsigned char nbiotSwitchModem(unsigned char state)
{
    NbiotModemTools::ModemState ms = NbiotModemTools::MODEM_OFF;
    unsigned char ret = 0;

    if(state)
    {
        ms = NbiotCoreApp::getInstance().getModemInstance().switchModem(NbiotModemTools::MODEM_ON);
    }
    else
    {
        ms = NbiotCoreApp::getInstance().getModemInstance().switchModem(NbiotModemTools::MODEM_OFF);
    }
    if(NbiotModemTools::MODEM_ON == ms)
    {
        ret = 1;
    }
    return ret;
}

unsigned char isNbiotModemOn()
{
    unsigned char ret = 0;
    NbiotModemTools::ModemState ms = NbiotCoreApp::getInstance().getModemInstance().isModemOn();

    if(NbiotModemTools::MODEM_ON == ms)
    {
        ret = 1;
    }
    return ret;
}
