/* ========================================================================
 * File Name: bc_uart.c
 * This file is part of the TI-MSP432 LibNbiot example
 *
 * Created on: 02.08.2017
 * Author: Edgar Hindemith
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

/* Driver Header files */
#include <ti/drivers/UART.h>
#include "libnbiotcore.h"
#include "bc_uart.h"

/*
 *  UART baudrate.
 *
 *  Baudrate for the UART peripheral.
 *  Default is 9600.
 */
#define UART_BAUDRATE        9600

/*!
 *  Board UART index.
 *
 *  Consult Board.h to find the index of the UART
 *  peripherals available for your board.
 *  By default Board_UART0 is used (0).
 */
#define UART_INDEX              0

UART_Handle uartBc = 0l;

void startBcUart()
{
    UART_Params uartParams;

    /* Initialize and open UART */
    UART_Params_init(&uartParams);

    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = UART_BAUDRATE;

    uartBc = UART_open(UART_INDEX, (UART_Params *) &uartParams);
}

void bcPutChar(unsigned char byte)
{
    if(NULL != uartBc)
    {
        UART_write(uartBc, &byte, 1);
    }
}


void bcDbgWrite(const unsigned char* buf, unsigned short len)
{
    while(len)
    {
        bcPutChar(*(buf++));
        --len;
    }
}

