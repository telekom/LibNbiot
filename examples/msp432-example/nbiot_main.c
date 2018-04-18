/* ========================================================================
 * File Name: nbiot_main.c
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
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include "stdio.h"
#include "stdlib.h"
#include <string.h>

#include <debug.h>

#include "nbiot_time.h"
#include "nbiot_uart.h"
#include "bc_uart.h"

#include "nbiotcoreconf.h"
#include "libnbiot.h"
#include "libnbiotcore.h"

#include "nbiot_tirtos.h"
#include "nbiot_evtloop.h"
#include "nbiot_main.h"
#include "auth.h"


char* state[] = {"Invalid", "Initial", "Disconnected", "Error", "Connected", "Sleep", "Hibernate", "Sleep (DIS)", "Awake", "Wait for modem attach", "Wait for connect"};
char* action[] = {"Initialize", "ReInitialize", "Connect", "RegisterTopic", "Publish", "Subscribe", "Unsubscribe", "RemoveTopic", "Disconnect", "Sleep", "Hibernate", "Wakeup"};
char* retCode[] = {"ACCEPTED", "REJECTED_CONGESTED", "REJECTED_INVALID_TOPIC_ID", "RC_REJECTED_NOT_SUPPORTED"};
char* error[] = {"Ok", "Initializing error", "MODEM not attached", "IP not connected", "Connection error", "Not started error", "MaxTopicsExceeded", "LoopCtrlBusyError", "InvalidTopicID"};

int notification_id = 0;
void notificationHandler(const Notification *n)
{
    // Handle your notifications here
    notification_id++;

    debugPrintf("\033[0;32m[ Debug    ]\033[0m ");

    debugPrintf("----- New notification %03d -----\r\n", notification_id);

    debugPrintf("\033[1;34m[----------]\033[0m State: \033[27G");
    debugPrintf("%s\r\n", state[n->state]);

    debugPrintf("\033[1;34m[----------]\033[0m Action: \033[27G");
    debugPrintf("%s\r\n", action[n->action]);

    debugPrintf("\033[1;34m[----------]\033[0m ReturnCode: \033[27G");
    debugPrintf("%s\r\n", retCode[n->returnCode]);

    debugPrintf("\033[1;34m[----------]\033[0m ErrorNumber: \033[27G");
    debugPrintf("%s\r\n", error[n->errorNumber]);

}


unsigned char messageArrived = 0;
int message_counter = 0;
void subscriptionHandler(MessageData* msg)
{
    messageArrived = 1;
    message_counter++;

    debugPrintf("\033[0;32m[ Debug    ]\033[0m ");

    debugPrintf("----- New message %03d -----\r\n", message_counter);

    debugPrintf("\033[0;35m[----------]\033[0m QoS: \033[27G");
    debugPrintf("%d\r\n", msg->message->qos);

    debugPrintf("\033[0;35m[----------]\033[0m Id: \033[27G");
    debugPrintf("%d\r\n", msg->message->id);

    debugPrintf("\033[0;35m[----------]\033[0m Payload: \033[27G");
    debugPrintf("%s\r\n", (char *) msg->message->payload);

    if(msg->topicName)
    {
        debugPrintf("\033[0;35m[----------]\033[0m Topic: \033[27G");
        debugPrintf("%s\r\n", (char *) msg->topicName);
    }
}

unsigned char init(char* imsi, char* pw)
{
    unsigned char ret=0;

    NbiotCoreConf core_conf;
    core_conf.tickFrequency = 1000;
    core_conf.readstatus_fu = readStatus;
    core_conf.readbyte_fu = readByte;
    core_conf.putchar_fu = putChar;
    core_conf.writestatus_fu = writeStatus;
    core_conf.apn = "nb-cloud.ic.m2mportal.de";
    core_conf.apnUser = "";
    core_conf.apnPwd = "";
    core_conf.operMccMnc = "26201";
    core_conf.imsi = imsi;
    core_conf.imsiPwd = pw;

    unsigned int retCoreConf = nbiotCoreConfig(&core_conf);

    NbiotConf conf;
    conf.keepAliveInterval = 10000;
    conf.autoPollInterval = 300;
    conf.maxTopicCount = 3;
    conf.gateway = "172.25.102.151";
    conf.port = 1883;
    conf.notify_fu = notificationHandler;
    conf.pollInterval = 10000;

    unsigned int retConf = nbiotConfig(&conf);


    if ((NoError == retConf) && (NoError == (~(CoreWarnApnUser | CoreWarnApnPwd) & retCoreConf)))
    {
        // Setup the statemachine, initialize internal varibles.
        ret = nbiotStart();
    }

    return ret;
}


void *mainThread(void *arg0)
{
    int16 value = 0;

    startNbiotTimer();
    startNbiotUart();
    startBcUart();

    setDebugWriteFunction(bcDbgWrite);
    //debugwrite = bcDbgWrite;

    debugPrintf("Starting the NBIoT Application @%d\r\n", (int)getMillis());

    unsigned char ret = 0;
    ret = init(AUTH_IMSI, AUTH_PWD);

    if(1 == ret)
    {
        debugPrintf("\033[0;32m[ Debug    ]\033[0m ");
        debugPrintf("\033[0;32mInit Successful\033[0m \r\n");
    }
    else
    {
        debugPrintf("\033[0;31m[ Debug    ]\033[0m ");
        debugPrintf("\033[0;31mInit Error\033[0m \r\n");
    }

    // Set topics
    char topicCmd[TOPIC_BUFFER_SIZE];
    sprintf (topicCmd, "NBIoT/%s/CMD/MyCmd", getNbiotDeviceId());

    char topicInf[TOPIC_BUFFER_SIZE];
    sprintf(topicInf, "NBIoT/%s/INF/MyCmd", getNbiotDeviceId());

    char topicTemp[TOPIC_BUFFER_SIZE];
    sprintf(topicTemp, "NBIoT/%s/MES/1", getNbiotDeviceId());

    char payload[PAYLOAD_LENGTH];

    enum NbiotResult rc = LC_Pending;

    srand((unsigned)getMillis());

    debugPrintf("\033[0;32m[ Debug    ]\033[0m ");
    debugPrintf("create mutex\r\n");
    createNbMutex();

    debugPrintf("\033[0;32m[ Debug    ]\033[0m ");
    debugPrintf("start the event-loop\r\n");
    startEvtLoop();

    debugPrintf("\033[0;32m[ Debug    ]\033[0m ");
    debugPrintf("start the main-loop @%dms\r\n", (int)getMillis());

    for(;;)
    {
        // lock
        if(0 == tryLockNbMutex())
        {

            if (isNbiotConnected())
            {
                if (LC_Idle == rc)
                {
                    // If not subscribed already, subscribe to command topic.
                    if(!isNbiotSubscribedTo(topicCmd))
                    {
                        nbiotSubscribe(topicCmd, subscriptionHandler );
                    }

                    if(messageArrived)
                    {
                        nbiotPublish(topicInf, "0", 1, QOS0);
                        messageArrived = 0;
                    }
                    else
                    {
                        value = (int16)(20 + rand() / ((RAND_MAX + 1) / 16)); // random temperature = 20 +/- 15 degrees
                        sprintf(payload, "%d", value);

                        debugPrintf("\033[0;32m[ DEBUG    ]\033[0m ");
                        debugPrintf("Publish: %s\r\n", payload);

                        nbiotPublish(topicTemp, payload, strlen(payload), QOS0);
                    }
                }
            }
            else
            {
                if (LC_Idle == rc)
                {
                    nbiotConnect(1); // Connect with cleanSession=1
                }
            }

            rc = getNbiotEventLoopStatus();

            // unlock
            unlockNbMutex();
        }
        usleep(MAINTHREAD_SLEEP_US);
    }

    return (NULL);
}

