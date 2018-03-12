/* ==============================================================
 * File Name: readstatus.h
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
#ifndef READSTATUS_H
#define READSTATUS_H

typedef enum rxStatus
{
    rx_empty = 0,
    rx_avail = 1,
    rx_error = 2,
    rx_availwitherror = 3
} ReadStatus;

#endif /* READSTATUS_H */
/* [] END OF FILE */
