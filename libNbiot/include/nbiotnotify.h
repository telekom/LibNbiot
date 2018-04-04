/* ========================================================================
 * LibNbiot: nbiotnotify.h - struct definition for notifications
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

#ifndef NBIOTNOTIFY_H
#define NBIOTNOTIFY_H
#include "nbiotdef.h"

struct Notification
{
    enum StateIds state;
    enum NbiotAction action;
    enum MQTTSN_returnCodes returnCode;
    enum ErrorNumber errorNumber;
};

typedef struct Notification Notification;

typedef void (*notifyHandler)(const Notification*);

#endif // NBIOTNOTIFY_H

