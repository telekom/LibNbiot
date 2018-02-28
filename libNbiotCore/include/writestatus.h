/* ==============================================================
 * File Name: writestatus.h
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
#ifndef WRITESTATUS_H
#define WRITESTATUS_H

typedef enum txStatus
{
    tx_empty = 0,
    tx_hasBytes = 1,
    tx_full = 2
} WriteStatus;

#endif /* WRITESTATUS_H */
/* [] END OF FILE */
