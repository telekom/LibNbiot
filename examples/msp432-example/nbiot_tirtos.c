/* ========================================================================
 * File Name: nbiot_tirtos.c
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
#include "nbiot_tirtos.h"

pthread_mutex_t nbiot_mutex;
unsigned char nbmInitialized = 0;

void createNbMutex()
{
    pthread_mutexattr_t attr;

    if(0 == nbmInitialized)
    {
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_NONE);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);

        pthread_mutex_init(&nbiot_mutex, &attr);

        nbmInitialized = 1;
    }
}

int lockNbMutex()
{
    return pthread_mutex_lock(&nbiot_mutex);
}

int tryLockNbMutex()
{
    return pthread_mutex_trylock(&nbiot_mutex);
}

int unlockNbMutex()
{
    return pthread_mutex_unlock(&nbiot_mutex);
}
