/* ========================================================================
 * LibNbiot: nbiotcoreconf.h
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

#ifndef NBIOTCORECONF_H
#define NBIOTCORECONF_H

#include "readstatus.h"
#include "writestatus.h"


#define FIFTY_HZ 50 /*!< TICKFREQUENCY: minimum one tick every 20 ms */

#define TEN_KHZ 10000 /*!< TICKFREQUENCY: maximum ten ticks per millisecond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief The NbiotConf struct is used to configure the library with nbiotConfig
 */
struct NbiotCoreConf
{
    unsigned long tickFrequency; /*!< sets the frequency (in Hz) of the central clock-tick */
    ReadStatus (*readstatus_fu)(); /*!< functionpointer of the ReadStatus function (UART) */
    unsigned char (*readbyte_fu)(); /*!< functionpointer of the 'readbyte' function (UART) */
    void (*putchar_fu)(unsigned char); /*!< functionpointer of the 'putchar' function (UART) */
    WriteStatus (*writestatus_fu)(); /*!< functionpointer of the WriteStatus function (UART) */
    const char* apn; /*!< The name of the used APN */
    const char* apnUser; /*!< The APN username */
    const char* apnPwd; /*!< The APN password */
    const char* operMccMnc;
    const char* imsi;
    const char* imsiPwd;
};

typedef struct NbiotCoreConf NbiotCoreConf;

#define NbiotCoreConf_initializer {1000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

/*!
 * \brief The NbiotConfigError enum enumerates the possible errors of the configuration ( nbiotConfig returns an OR'ed result )
 */
enum NbiotCoreConfigError
{
    CoreNoError = 0, /*!< No error, configuration successful */
    CoreErrorTickFreq = 1, /*!< Tick frequency invalid */
    CoreErrorReadStatusFu = 2, /*!< No ReadStatus function */
    CoreErrorReadByteFu = 4, /*!< No 'readbyte' function */
    CoreErrorPutCharFu = 16, /*!< No 'putchar' function */
    CoreErrorWriteStatusFu = 32, /*!< No 'WriteStatus' function */
    CoreErrorApn = 2048, /*!< No APN */
    CoreWarnApnUser = 4096, /*!< Warning: no APN-user (config is still valid) */
    CoreWarnApnPwd = 8192, /*!< Warning: no APN-password (config is still valid) */
    CoreErrorMultipleConfig = 16384, /*!< Configuration was called more than one time */
    CoreErrorNoConfig = 32768 /*!< Configuration was never called */
};

#ifdef __cplusplus
}
#endif

#endif // NBIOTCORECONF_H
