/* ========================================================================
 * LibNbiot: nbiotdebug.cpp
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

#include <stdarg.h>
#include <stdio.h> // for size_t

#include "nbiotdebug.h"

#ifdef DEBUG_STRING
unsigned long stringMem = 0;
#endif

static void (*debugwrite)(const unsigned char*, unsigned short) = 0l;

void setDebugWriteFunction(void (*dbgwrite_fu)(const unsigned char*, unsigned short))
{
    debugwrite = dbgwrite_fu;
}

void debugPrintf(const char* format, ...)
{
    unsigned char dbuf[DEBUGLINE_LENGTH];
    va_list vl;
    if(0 != debugwrite)
    {
        va_start(vl, format);
        int n = vsnprintf((char*)dbuf, (size_t)DEBUGLINE_LENGTH, format, vl);
        va_end(vl);
        if((0 < n) && (DEBUGLINE_LENGTH > (unsigned int)n))
        {
            debugwrite(dbuf, n);
        }
    }
}
