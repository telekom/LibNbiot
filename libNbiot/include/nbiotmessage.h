/* ==============================================================
 * File Name: nbiotmessage.h
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
#ifndef NBIOTMESSAGE_H
#define NBIOTMESSAGE_H
#include "stdio.h" /* for size_t */

enum QoS { QOS0, QOS1, QOS2 };

typedef struct NbiotMessage NbiotMessage;

typedef struct MessageData MessageData;

struct NbiotMessage
{
    enum QoS qos;
    char retained;
    char dup;
    unsigned short id;
    void *payload;
    size_t payloadlen;
};

struct MessageData
{
    NbiotMessage* message;
    const char* topicName;
};

typedef void (*messageHandler)(MessageData*);

#endif // NBIOTMESSAGE_H

