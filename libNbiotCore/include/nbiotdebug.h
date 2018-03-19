#ifndef NBIOTDEBUG_H
#define NBIOTDEBUG_H

#define DEBUG /*!< global debugging output on or off */
#define DEBUGLINE_LENGTH (256u) /*!< maximum length of a single debugging output line */
#ifdef DEBUG

#define DEBUG_SERIAL /*!< debugging output for the class Serial */
#undef DEBUG_SERIAL

#define DEBUG_SERIAL_VERBOSE /*!< verbose debugging output for the method Serial::readLine */
#undef DEBUG_SERIAL_VERBOSE

#define DEBUG_SERIAL_RAW /*!< debugging output for the method Serial::readRaw */
#undef DEBUG_SERIAL_RAW

#define DEBUG_ATCOMMANDS /*!< debugging output for the class AtCommands */
#undef DEBUG_ATCOMMANDS

#define DEBUG_MODEM /*!< debugging output for the classes NbiotModemTools and NbiotConnectivity */
// #undef DEBUG_MODEM

#define DEBUG_MQTT /*!< debugging output for LibNbiot classes */
#undef DEBUG_MQTT

#define DEBUG_STRING /*!< debugging output for the nbiot::string class */
#undef DEBUG_STRING
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
