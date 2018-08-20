/* ========================================================================
 * File Name: nbiot_uart.c
 * This file is part of the TI-MSP432 LibNbiot example
 *
 * Created on: 24.07.2017
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

#include <string.h> // for memset
#include <unistd.h>

/* POSIX Header files */
#include <pthread.h>

/* Driver Header files */
#include <ti/drivers/UART.h>
#include "libnbiotcore.h"
#include "nbiot_tirtos.h"
#include "nbiot_uart.h"


#define NBU_BUF_SIZE 64

#define RD_SLEEP_US 100

/*
 *  UART baudrate.
 *
 *  Baudrate for the UART peripheral.
 *  Default is 9600.
 */
#define UART_BAUDRATE        9600

/*
 *  Thread stack size.
 *
 *  This is the stack size of the uart thread.
 *  Default value is 768.
 */
#define UARTThread_STACKSIZE    1024

/*!
 *  Board UART index.
 *
 *  Consult Board.h to find the index of the UART
 *  peripherals available for your board.
 *  By default Board_UART0 is used (0).
 */
#define UART_INDEX              1

typedef struct nbiot_uart
{
    ReadStatus nbiot_readStatus;
    WriteStatus nbiot_writeStatus;
    unsigned char input[NBU_BUF_SIZE];
    unsigned char iRPtr;
    unsigned char iWPtr;
    unsigned char output;
} NbiotUart;

typedef enum nbu_ptr_type
{
    input_read,
    input_write
} NbuPtrType;

NbiotUart nbiotUart;

UART_Handle uart = 0l;

void incPtr(NbuPtrType ptr);
void* UART_taskFxn(void *arg0);


ReadStatus readStatus()
{
    usleep(RD_SLEEP_US);
    return nbiotUart.nbiot_readStatus;
}

WriteStatus writeStatus()
{
    return nbiotUart.nbiot_writeStatus;
}

unsigned char readByte()
{
    unsigned char ret = 0;
    if(rx_empty != nbiotUart.nbiot_readStatus)
    {
        if(rx_avail == nbiotUart.nbiot_readStatus)
        {
            ret = nbiotUart.input[nbiotUart.iRPtr];
        }
        incPtr(input_read);
    }
    return ret;
}

void putChar(unsigned char byte)
{
    if(NULL != uart)
    {
        nbiotUart.output = byte;
        nbiotUart.nbiot_writeStatus = tx_full;

        UART_write(uart, &nbiotUart.output, 1);

        nbiotUart.output = 0;
        nbiotUart.nbiot_writeStatus = tx_empty;
    }
}

void incPtr(NbuPtrType ptr)
{
    switch(ptr)
    {
    case input_read:
        if(nbiotUart.iRPtr == nbiotUart.iWPtr)
        {
            nbiotUart.nbiot_readStatus = rx_empty;
        }
        else
        {
            if((NBU_BUF_SIZE - 1) > nbiotUart.iRPtr)
            {
                ++nbiotUart.iRPtr;
            }
            else
            {
                nbiotUart.iRPtr=0;
            }
            if(nbiotUart.iRPtr == nbiotUart.iWPtr)
            {
                nbiotUart.nbiot_readStatus = rx_empty;
            }
        }
        break;
    case input_write:
        if((NBU_BUF_SIZE - 1) > nbiotUart.iWPtr)
        {
            ++nbiotUart.iWPtr;
        }
        else
        {
            nbiotUart.iWPtr=0;
        }
        if(nbiotUart.iRPtr == nbiotUart.iWPtr)
        {
            nbiotUart.nbiot_readStatus = rx_availwitherror;
        }
        else
        {
            nbiotUart.nbiot_readStatus = rx_avail;
        }
        break;
    default:
        break;
    }
}

void startNbiotUart()
{
    nbiotUart.nbiot_readStatus = rx_empty;
    nbiotUart.nbiot_writeStatus = tx_empty;
    memset(nbiotUart.input, 0, NBU_BUF_SIZE);
    nbiotUart.iRPtr = 0;
    nbiotUart.iWPtr = 0;
    nbiotUart.output = 0;

    pthread_t           uartThread;
    pthread_attr_t      attrs;
    struct sched_param  priParam;
    int                 retc;

    /* Call driver init functions */
    UART_init();

    /* Create UARTMon_taskFxn thread */
    pthread_attr_init(&attrs);

    /* Set priority and stack size attributes */
    retc = pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    if (retc != 0) {
        /* pthread_attr_setdetachstate() failed */
        while (1);
    }

    retc |= pthread_attr_setstacksize(&attrs, UARTThread_STACKSIZE);
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }

    /* Create UARTMon_taskFxn thread */
    priParam.sched_priority = sched_get_priority_min(0);
    pthread_attr_setschedparam(&attrs, &priParam);

    retc = pthread_create(&uartThread, &attrs, UART_taskFxn, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }
}

void* UART_taskFxn(void *arg0)
{
    UART_Params uartParams;

    /* Initialize and open UART */
    UART_Params_init(&uartParams);

    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = UART_BAUDRATE;

    uart = UART_open(UART_INDEX, (UART_Params *) &uartParams);

    if (uart == NULL)
    {
        /* UART_open() failed */
        while (1);
    }

    for (;;)
    {
        unsigned char avail = 0;
        UART_control(uart, UART_CMD_ISAVAILABLE, &avail);
        if(avail)
        {
            if(0 < UART_read(uart, &nbiotUart.input[nbiotUart.iWPtr], 1))
            {
                incPtr(input_write);
            }
        }
        usleep(RD_SLEEP_US);
    }

    return (NULL);
}

