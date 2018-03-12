/* ==============================================================
 * File Name: debug.h
 *
 * Author: Edgar Hindemith
 *
 * Copyright T-Systems & Ingenieurbuero Edgar Hindemith, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF T-Systems.
 *
 * ==============================================================
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

