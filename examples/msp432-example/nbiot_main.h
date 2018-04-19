/* ========================================================================
 * File Name: nbiot_main.h
 * This file is part of the TI-MSP432 LibNbiot example
 *
 * Created on: 09.08.2017
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

#ifndef NBIOT_MAIN_H_
#define NBIOT_MAIN_H_

#include "nbiotnotify.h"
#include "nbiotmessage.h"

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;
typedef unsigned long long uint64;
typedef long long int64;

#define MAINTHREAD_SLEEP_US 1000

#define TOPIC_BUFFER_SIZE   (32u)
#define PAYLOAD_LENGTH      (12u) /* string size: max int, sign, terminating 0 */

#define LOOP_STEPS 5
#define LOOP_STEP_LONG_SLEEP 5
#define LOOP_STEP_LOOP_RESET 6


#define ONE_SECOND 1000
#define LOOP_INTERVAL 15
#define LONG_INTERVAL 60
#define LOOP_TIME (LOOP_INTERVAL * ONE_SECOND)
#define LONG_TIME (LONG_INTERVAL * ONE_SECOND)
#ifdef LINUX_HOST
#define RECONNECT_TIME 30000
#else
#define RECONNECT_TIME 0
#endif

void notificationHandler(const Notification *n);

void subscriptionHandler(MessageData* msg);

void *mainThread(void *arg0);

#endif /* NBIOT_MAIN_H_ */
