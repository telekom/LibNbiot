/* ========================================================================
 * LibNbiot: debug.h
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

#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief setDebugWriteFunction registers an arbitrary function of the type \code void fn (const unsigned char* buffer, unsigned short len); \endcode
 * as writer-function for debugging output. The length will not exceed \sa DEBUGLINE_LENGTH
 */
void setDebugWriteFunction(void (*dbgwrite_fu)(const unsigned char*, unsigned short));

/*!
 * \brief debugPrintf printf-style function to write debugging messages. Will only work if there is a write function registered with \sa setDebugWriteFunction.
 * \param format printf-style format string
 */
void debugPrintf(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif // DEBUG_H

