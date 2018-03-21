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
