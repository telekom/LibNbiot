/* ========================================================================
 * LibNbiot: libnbiotcore.h
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

#ifndef LIBNBIOTCORE_H
#define LIBNBIOTCORE_H
#include <stdio.h> // for size_t
#include <time.h> // for time_t
#include "nbiotcoreconf.h"
#include "debug.h"

/*! \mainpage
 *
 * \section intro Introduction
 *
 * The nbIOTCore library provides data and functions shared by libNbiot and libNbiotEmu.
 *
 * \section details Details
 * 
 * The provided functions are declared in the \link libnbiotcore.h \endlink header.
 *
 */


#ifdef __cplusplus
extern "C" {
#endif


/* timer functions of lib-nbiotEmu ------------------------------------------- */

/*!
 * \brief tick need to be called by timer-ISR
 */
void tick();

/*!
 * \brief getMillis
 * \returns the milliseconds counted by tick()
 */
long long getMillis();

/*!
 * \brief updateMillis used to add missed milliseconds after PSOC-sleep
 * \param ms milliseconds to add
 */
void updateMillis(unsigned long ms);

/*!
 * \brief getTime get the current time based on the millisecond count
 * \returns the unix-timestamp part of the counted milliseconds
 */
time_t getTime();

/*!
 * \brief setTime sets the unix-timestamp part. used to sync with modem-time
 * \param t timestamp
 */
void setTime(time_t t);


/* /timer functions ---------------------------------------------------------- */

/*!
 * \brief nbiotCoreConfig configures the library
 * \param conf pointer to NbiotCoreConf struct
 * \return OR'ed combination of NbiotCoreConfigError values or NoError in case of success
 */
unsigned int nbiotCoreConfig(NbiotCoreConf* conf);

/*!
 * \brief sendAtCommand Sends a single AT-command and waits for the response.
 * \param command the at-command
 * \param response the buffer for the response
 * \param len the length of the response-buffer
 * \param timeout maximum time in ms
 * \return true in case of success
 */
unsigned char sendAtCommand(const char* command, char* response, size_t len, unsigned short timeout);

/*!
 * \brief addUrcFilter Adds an URC filter. Wrapper function for NbiotModemTools method not part of IModem interface
 * \param urc unsolicited response code to be filtered
 * \param urc_handler notification handler for URC; receives the complete response line in its parameter
 * \return true in case of success
 */
unsigned char addUrcFilter(const char* urc, void (*urc_handler)(const char*));
/*!
 * \brief removeUrcFilter Removes an URC filter. Wrapper function for NbiotModemTools method not part of IModem interface
 * \param urc URC to be removed from the list of filters
 * \return true in case of success
 */
unsigned char removeUrcFilter(const char* urc);

/*!
 * \brief isNbiotAttached
 * \returns 1 if the NBIoT modem is attached to the network or 0 otherwise
 */
unsigned char isNbiotAttached();

/*!
 * \brief nbiotAttach attaches the modem to nbiot network
 * \returns 1 if the NBIoT modem is attached to the network or 0 otherwise
 */
unsigned char nbiotAttach();

/*!
 * \brief nbiotDetach detaches the modem from nbiot network
 * \returns 1 if the NBIoT modem is detached from the network or 0 otherwise
 */
unsigned char nbiotDetach();

/*!
 * \brief nbiotModemRestart reboots the nbiot modem
 * \returns 1 if restart was successful or 0 otherwise
 */
unsigned char nbiotModemRestart();

/*!
 * \brief nbiotModemInit (re)initializes the nbiot modem
 * \returns 1 if restart was successful or 0 otherwise
 */
unsigned char nbiotModemInit();

/*!
 * \brief getNbiotDeviceId
 * \returns the IMSI
 */
const char* getNbiotDeviceId();

/*!
 * \brief nbiotSwitchModem switches the radio on or off
 * \param state 0 switches radio off; 1 switches radio on
 * \return 1 if radio is switched on or 0 otherwise
 */
unsigned char nbiotSwitchModem(unsigned char state);

/*!
 * \brief isNbiotModemOn checks if the radio is on or off
 * \return 1 if radio is switched on or 0 otherwise
 */
unsigned char isNbiotModemOn();

#ifdef __cplusplus
}
#endif

#endif // LIBNBIOTCORE_H

