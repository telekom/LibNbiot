/* ========================================================================
 * File Name: nbiot_evtloop.h
 * This file is part of the TI-MSP432 LibNbiot example
 *
 * Created on: 11.08.2017
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
/* POSIX Header files */
#include <pthread.h>
#include <unistd.h>

#include "libnbiot.h"

#include "nbiot_tirtos.h"
#include "nbiot_evtloop.h"

void startEvtLoop()
{
    pthread_t           evtloop_thread;
    pthread_attr_t      attrs;
    struct sched_param  priParam;
    int                 retc;

    /* Create UARTMon_taskFxn thread */
    pthread_attr_init(&attrs);

    /* Set priority and stack size attributes */
    retc = pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    if (retc != 0) {
        /* pthread_attr_setdetachstate() failed */
        while (1);
    }

    retc |= pthread_attr_setstacksize(&attrs, EVTLOOPTHREAD_STACKSIZE);
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }

    /* Create UARTMon_taskFxn thread */
    priParam.sched_priority = sched_get_priority_min(0);
    pthread_attr_setschedparam(&attrs, &priParam);

    retc = pthread_create(&evtloop_thread, &attrs, evtLoopThread, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }
}

void *evtLoopThread(void* arg)
{
    while(1)
    {
        if(0 == tryLockNbMutex())
        {
            nbiotEventLoop(EventModeActive);
            unlockNbMutex();
        }
        usleep(EVTLOOPTHREAD_SLEEP_US);
    }
}
