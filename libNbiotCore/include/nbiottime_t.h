/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef NBIOTTIME_T_H
#define NBIOTTIME_T_H
#include <time.h>
    
struct NbiotTime_t
{
    unsigned short millis;
    time_t timestamp;
};

typedef struct NbiotTime_t NbiotTime_t;

#endif /* NBIOTTIME_T_H */
/* [] END OF FILE */
