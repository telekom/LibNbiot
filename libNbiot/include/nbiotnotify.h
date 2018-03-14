/* ==============================================================
 * File Name: nbiotnotify.h
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

