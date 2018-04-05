/* ========================================================================
 * LibNbiot 'C' example: main.c
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

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "debug.h"

#include "libnbiotcore.h"
#include "libnbiot.h"
#include "serial.h"

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN
#define NANOS_PER_MILLI 1000000

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

static void handler(int sig, siginfo_t *si, void *uc)
{
    (void) sig;
    (void) si;
    (void) uc;

    tick();
}

timer_t start_timer()
{
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;
    sigset_t mask;
    struct sigaction sa;

    /* Establish handler for timer signal */

    printf("Establishing handler for signal %d\n", SIG);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIG, &sa, NULL) == -1)
        errExit("sigaction");

    /* Block timer signal temporarily */

    printf("Blocking signal %d\n", SIG);
    sigemptyset(&mask);
    sigaddset(&mask, SIG);
    if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
        errExit("sigprocmask");

    /* Create the timer */

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIG;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCKID, &sev, &timerid) == -1)
        errExit("timer_create");

    printf("timer ID is 0x%lx\n", (long) timerid);

    /* Start the timer */

    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = NANOS_PER_MILLI;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    if (timer_settime(timerid, 0, &its, NULL) == -1)
         errExit("timer_settime");

    /* Unlock the timer signal, so that timer notification
       can be delivered */

    printf("Unblocking signal %d\n", SIG);
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
        errExit("sigprocmask");

    return timerid;
}

void dbgWrite(const unsigned char* data, unsigned short len)
{
    int  i;
    for(i = 0; i < len; ++i)
    {
        putchar((int)data[i]);
    }
}

int notification_id = 0;
void notificationHandler(const Notification *n)
{
    // Handle your notifications here
    notification_id++;

    debugPrintf("\033[0;32m[ Debug    ]\033[0m ");

    debugPrintf("----- New notification %03d -----\r\n", notification_id);

    debugPrintf("\033[1;34m[----------]\033[0m State: \033[27G");
    debugPrintf("%d\r\n", n->state);

    debugPrintf("\033[1;34m[----------]\033[0m Action: \033[27G");
    debugPrintf("%d\r\n", n->action);

    debugPrintf("\033[1;34m[----------]\033[0m ReturnCode: \033[27G");
    debugPrintf("%d\r\n", n->returnCode);

    debugPrintf("\033[1;34m[----------]\033[0m ErrorNumber: \033[27G");
    debugPrintf("%d\r\n", n->errorNumber);
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


unsigned char init(char* imsi, char* pw, char* ser)
{
    unsigned char ret=0;

    ret = initSerial(ser);

    if(1 == ret)
    {
        NbiotCoreConf core_conf;
        core_conf.tickFrequency = 1000;
        core_conf.readstatus_fu = readStatus;
        core_conf.readbyte_fu = readByte;
        core_conf.putchar_fu = writeByte;
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
            if(1 == ret)
            {
                debugPrintf("\033[0;32m[ Debug    ]\033[0m ");
                debugPrintf("\033[0;32mInit Successfull\033[0m \r\n");
            }
        }
        else
        {
            ret = 0;
        }
    }
    if (0 == ret)
    {
        debugPrintf("\033[0;31m[ Debug    ]\033[0m ");
        debugPrintf("\033[0;31mInit Error\033[0m \r\n");
    }

    return ret;
}

#define MAX_VAL 20
#define MIN_VAL 0
unsigned char reverseFlag = 0;
int valueCounter = MIN_VAL;
int getSensorValue()
{
    if (MAX_VAL == valueCounter)
    {
        reverseFlag = 1;
    }
    else if (MIN_VAL == valueCounter)
    {
        reverseFlag = 0;
    }
    return (reverseFlag) ? valueCounter-- : valueCounter++;
}

void usage()
{
    debugPrintf("Usage: \r\n");
    debugPrintf("\t-h Print usage and exit.\r\n");
    debugPrintf("\t-i imsi - mandatory parameter 15 digit imsi\r\n");
    debugPrintf("\t-p password - mandatory parameter 8 characters\r\n");
    debugPrintf("\t-s serial - mandatory parameter, path to serial device (e.g. /dev/ttyACM0)\r\n");
}

int main(int argc, char** argv)
{
    setDebugWriteFunction(dbgWrite);

    char* imsi = NULL;
    char* pw = NULL;
    char* ser = NULL;

    int c = -1;
    while((c = getopt (argc, argv, "hi:p:s:")) != -1)
    {
        switch(c)
        {
        case 'i':
        {
            imsi = optarg;
            if(15 == strlen(imsi))
            {
                debugPrintf("\033[0;32m[ Debug    ]\033[0m ");
                debugPrintf("imsi: %s\r\n", imsi);
            }
            else
            {
                debugPrintf("\033[0;31m[ Debug    ]\033[0m ");
                debugPrintf("imsi: invalid value (imsi has to be of length 15) - aborting\r\n");
                imsi = NULL;
            }
            break;
        }
        case 'p':
        {
            pw = optarg;
            if(8 == strlen(pw))
            {
                debugPrintf("\033[0;32m[ Debug    ]\033[0m ");
                debugPrintf("pw: %s\r\n", pw);
            }
            else
            {
                debugPrintf("\033[0;31m[ Debug    ]\033[0m ");
                debugPrintf("pw: invalid value (pw has to be of length 8) - aborting\r\n");
                pw = NULL;
            }
            break;
        }
        case 's':
        {
            ser = optarg;
            if(-1 != access(ser, F_OK))
            {
                debugPrintf("\033[0;32m[ Debug    ]\033[0m ");
                debugPrintf("serial node: %s\r\n", ser);
            }
            else
            {
                debugPrintf("\033[0;31m[ Debug    ]\033[0m ");
                debugPrintf("serial: no device node with path \"%s\" exists - aborting\r\n", ser);
                ser = NULL;
            }
            break;
        }
        case 'h':
        {
            usage();
            exit(EXIT_SUCCESS);
        }
        default:
            debugPrintf("skipping unsupported option: %c\r\n", (char)c);
            break;
        }
    }

    if(NULL == imsi || NULL == pw || NULL == ser)
    {
        debugPrintf("\033[0;31m[ Error    ]\033[0m ");
        debugPrintf("parameters imsi, pw and serial, have to be set as described in usage: \r\n\r\n");

        usage();
        exit(EXIT_FAILURE);
    }

    // Set topics
    char topicCmd[32];
    sprintf (topicCmd, "NBIoT/%s/CMD/MyCmd", imsi);

    char topicInf[32];
    sprintf(topicInf, "NBIoT/%s/INF/MyCmd", imsi);

    char topicTemp[32];
    sprintf(topicTemp, "NBIoT/%s/MES/1", imsi);

    char payload[11]; // max-length: 32-Bit decimal + terminating zero

    // Start timer.
    start_timer();


    // Initialize library
    unsigned char retInit = init(imsi, pw, ser);

    enum NbiotResult rc = LC_Pending;

    while(retInit)
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
                    sprintf(payload, "%d", getSensorValue());

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

        rc = nbiotEventLoop(EventModeActive);
        delay(50);
    }

    exit(EXIT_SUCCESS);
}

