/* ========================================================================
 * LibNbiot: nbiotdebug.h
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

#ifndef NBIOTDEBUG_H
#define NBIOTDEBUG_H

//#ifndef DEBUG
#define DEBUG /*!< global debugging output on or off */
//#undef DEBUG
//#endif
#define DEBUGLINE_LENGTH (256u) /*!< maximum length of a single debugging output line */
#ifdef DEBUG

#define DEBUG_COLOR

// #define DEBUG_SERIAL /*!< debugging output for the class Serial */
// #define DEBUG_SERIAL_VERBOSE /*!< verbose debugging output for the method Serial::readLine */
// #define DEBUG_SERIAL_RAW /*!< debugging output for the method Serial::readRaw */
#define DEBUG_ATCOMMANDS /*!< debugging output for the class AtCommands */
#define DEBUG_MODEM /*!< debugging output for the classes NbiotModemTools and NbiotConnectivity */

#define DEBUG_MQTT /*!< debugging output for LibNbiot classes */

// #define DEBUG_STRING /*!< debugging output for the nbiot::string class */

#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG_STRING
extern unsigned long stringMem; /*!< global counter for the memory usage of all nbiot::string instances */
#endif

#ifdef __cplusplus
}
#endif

#include "debug.h"

#endif // NBIOTDEBUG_H
