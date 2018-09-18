/* ========================================================================
 * LibNbiot: SimpleExample for Arduino
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

#include <libnbiot.h>
#include <libnbiotcore.h>
#include <nbiotstring.h>
#include <nbiottimer.h>
#include <Serial.h>
#include <TimerOne.h>

// Type in your Username and Password for authentication at the MQTT Broker here
// In case you are using Deutsche Telekom's Cloud of Things, the IMSI of the SIM Card is used as the Username

#define AUTH_IMSI "111111111111111"
#define AUTH_PWD "PASSWORD"

unsigned char m_char_buffer;

unsigned char readByte() {
    char ret;
    ret = 0;
    if (Serial1) {
        int byteRead = Serial1.readBytes(&ret, 1);
        if (1 != byteRead) {
            ret = 0;
        }
    }
    return ret;
}

void writeByte(unsigned char buf) {
    if (Serial1) {
        Serial1.write(buf);
    }
}

ReadStatus readStatus() {
    ReadStatus ret = rx_empty;
    if (Serial1) {
        if(0 < Serial1.readBytes(&m_char_buffer, 1))
            ret = rx_avail;
    }
    return ret;
}

WriteStatus writeStatus() {
    WriteStatus ret = tx_full;
    if (Serial1) {
        ret = tx_empty;
    }
    return ret;
}


int notification_id = 0;

void notificationHandler(const Notification *n) {
    // Handle your notifications here
    notification_id++;

    debugPrintf("[ Debug    ]");

    debugPrintf("----- New notification %03d -----\r\n", notification_id);

    debugPrintf("[----------] State: ");
    debugPrintf("%d\r\n", n->state);

    debugPrintf("[----------] Action: ");
    debugPrintf("%d\r\n", n->action);

    debugPrintf("[----------] ReturnCode: ");
    debugPrintf("%d\r\n", n->returnCode);

    debugPrintf("[----------] ErrorNumber: ");
    debugPrintf("%d\r\n", n->errorNumber);
}

unsigned char messageArrived = 0;
int message_counter = 0;

void subscriptionHandler(MessageData *msg) {
    messageArrived = 1;
    message_counter++;

    debugPrintf("[ Debug    ] ");

    debugPrintf("----- New message %03d -----\r\n", message_counter);

    debugPrintf("[----------] QoS: ");
    debugPrintf("%d\r\n", msg->message->qos);

    debugPrintf("[----------] Id: ");
    debugPrintf("%d\r\n", msg->message->id);

    debugPrintf("[----------] Payload: ");
    debugPrintf("%s\r\n", (char *) msg->message->payload);

    if (msg->topicName) {
        debugPrintf("[----------] Topic: ");
        debugPrintf("%s\r\n", (char *) msg->topicName);
    }
}


void dbgWrite(const unsigned char *data, unsigned short len) {
    for (int i = 0; i < len; ++i) {
        Serial.write(data[i]);
    }
}


unsigned char init(char *imsi, char *pw) {
    unsigned char ret = 0;

    Serial.begin(9600);
    Serial1.begin(9600);

    Timer1.initialize(1000);
    Timer1.attachInterrupt(tick);

    if (Serial1) {
        ret = 1;
    }

    if (1 == ret) {
        NbiotCoreConf core_conf;

        core_conf.tickFrequency = 1000;
        core_conf.readstatus_fu = readStatus;
        core_conf.readbyte_fu = readByte;
        core_conf.putchar_fu = writeByte;
        core_conf.writestatus_fu = writeStatus;
        core_conf.apn = "nb-cloud.ic.m2mportal.de";
        core_conf.apnUser = "";
        core_conf.apnPwd = "";
        core_conf.plmn = "26201";

        NbiotConf conf;

        conf.keepAliveInterval = 10000;
        conf.autoPollInterval = 300;
        conf.maxTopicCount = 3;
        conf.gateway = "172.25.102.151";
        conf.port = 1883;
        conf.notify_fu = notificationHandler;
        conf.login = imsi;
        conf.password = pw;
        conf.maxResend = 10;

        unsigned int retCoreConf = nbiotCoreConfig(&core_conf);
        unsigned int retConf = nbiotConfig(&conf);

        if ((NoError == retConf) && (NoError == (~(CoreWarnApnUser | CoreWarnApnPwd) & retCoreConf))) {
            // Setup the statemachine, initialize internal varibles.
            ret = nbiotStart();
            if (1 == ret) {
                debugPrintf("[ Debug    ] ");
                debugPrintf("Init Successfull \r\n");
            }
        } else {
            ret = 0;
        }
    }
    if (0 == ret) {
        debugPrintf("[ Debug    ] ");
        debugPrintf("Init Error \r\n");
    }

    return ret;
}


#define MAX_VAL 20
#define MIN_VAL 0

unsigned char reverseFlag = 0;
int valueCounter = MIN_VAL;

int getSensorValue() {
    if (MAX_VAL == valueCounter) {
        reverseFlag = 1;
    } else if (MIN_VAL == valueCounter) {
        reverseFlag = 0;
    }
    return (reverseFlag) ? valueCounter-- : valueCounter++;
}

enum NbiotResult rc = LC_Pending;
unsigned char retInit = 0;
const char topicCmd[32];
const char topicInf[32];
const char topicTemp[32];
char payload[11];

void setup() {
    // put your setup code here, to run once:

    setDebugWriteFunction(dbgWrite);

    const char *imsi = AUTH_IMSI;
    const char *pw = AUTH_PWD;

    // Set topics

    sprintf(topicCmd, "NBIoT/%s/CMD/MyCmd", imsi);

    sprintf(topicInf, "NBIoT/%s/INF/MyCmd", imsi);

    sprintf(topicTemp, "NBIoT/%s/MES/1", imsi);

    // Initialize library
    retInit = init(imsi, pw);

}

void loop() {
    if (retInit) {
        if (isNbiotConnected()) {
            if (LC_Idle == rc) {
                // If not subscribed already, subscribe to command topic.
                if (!isNbiotSubscribedTo(topicCmd)) {
                    nbiotSubscribe(topicCmd, subscriptionHandler);
                }

                if (messageArrived) {
                    nbiotPublish(topicInf, "0", 1, QOS0);
                    messageArrived = 0;
                } else {
                    sprintf(payload, "%d", getSensorValue());

                    debugPrintf("[ DEBUG    ] ");
                    debugPrintf("Publish: %s\r\n", payload);

                    nbiotPublish(topicTemp, payload, strlen(payload), QOS0);
                }
            }
        } else {
            if (LC_Idle == rc) {
                nbiotConnect(1); // Connect with cleanSession=1
            }
        }

        rc = nbiotEventLoop(EventModeActive);
        delay(50);
    }
}
