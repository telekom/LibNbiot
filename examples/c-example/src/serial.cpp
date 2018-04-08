/* ========================================================================
 * LibNbiot 'C' example: serial.cpp
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

#include "serialcom.h"
#include "serial.h"

SerialCom* pSerCom = NULL;

#ifdef __cplusplus
extern "C" {
#endif

unsigned char initSerial(const char* dev)
{
    unsigned char ret = 0;
    if(NULL == pSerCom)
    {
        if(NULL != dev)
        {
            pSerCom = new SerialCom(dev);
        }

        if((NULL != pSerCom) && (pSerCom->connect()))
        {
            ret = 1;
        }
    }
    else
    {
        if(pSerCom->isConnected())
        {
            ret = 1;
        }
    }
    return ret;
}

unsigned char readByte()
{
    unsigned char ret = 0;
    if(NULL != pSerCom)
    {
        ret = pSerCom->readByte();
    }
    return ret;
}

ReadStatus readStatus()
{
    ReadStatus ret = rx_empty;
    if(NULL != pSerCom)
    {
        ret = pSerCom->readStatus();
    }
    return ret;
}

void writeByte(unsigned char byte)
{
    if(NULL != pSerCom)
    {
        pSerCom->writeByte(byte);
    }
}

WriteStatus writeStatus()
{
    WriteStatus ret = tx_full;
    if(NULL != pSerCom)
    {
        ret = pSerCom->writeStatus();
    }
    return ret;
}

#ifdef __cplusplus
}
#endif

