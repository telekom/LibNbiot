/* ========================================================================
 * LibNbiot: main.cpp
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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ========================================================================
*/

// Project 
#include "libnbiot.h"
#include "libnbiotcore.h"
#include "nbiottimer.h"
#include "serialcom.h"
#include "nbiotstring.h"

// STD C++
#include <thread>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>

// Unix
#include <getopt.h>
#include <unistd.h>

std::ostream &operator<<(std::ostream &out, const StateIds value) {
    const char *s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
    switch (value) {
        PROCESS_VAL(InvalidState);
        PROCESS_VAL(InitialState);
        PROCESS_VAL(DisconnectedState);
        PROCESS_VAL(ErrorState);
        PROCESS_VAL(ConnectedState);
        PROCESS_VAL(ConnectedSleepState);
        PROCESS_VAL(DeepSleepState);
        PROCESS_VAL(DisconnectedSleepState);
        PROCESS_VAL(ConnectedAwakeState);
        PROCESS_VAL(WaitForAttachState);
        PROCESS_VAL(WaitForConnectState);
    }

#undef PROCESS_VAL

    return out << s;
}

std::ostream &operator<<(std::ostream &out, const NbiotAction value) {
    const char *s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
    switch (value) {
        PROCESS_VAL(Initialize);
        PROCESS_VAL(ReInitialize);
        PROCESS_VAL(Connect);
        PROCESS_VAL(RegisterTopic);
        PROCESS_VAL(Publish);
        PROCESS_VAL(Subscribe);
        PROCESS_VAL(Unsubscribe);
        PROCESS_VAL(RemoveTopic);
        PROCESS_VAL(Disconnect);
        PROCESS_VAL(Sleep);
        PROCESS_VAL(Hibernate);
        PROCESS_VAL(Wakeup);
    }

#undef PROCESS_VAL

    return out << s;
}

std::ostream &operator<<(std::ostream &out, const MQTTSN_returnCodes value) {
    const char *s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
    switch (value) {
        PROCESS_VAL(RC_ACCEPTED);
        PROCESS_VAL(RC_REJECTED_CONGESTED);
        PROCESS_VAL(RC_REJECTED_INVALID_TOPIC_ID);
        PROCESS_VAL(RC_REJECTED_NOT_SUPPORTED);
    }

#undef PROCESS_VAL

    return out << s;
}

std::ostream &operator<<(std::ostream &out, const ErrorNumber value) {
    const char *s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
    switch (value) {
        PROCESS_VAL(Success);
        PROCESS_VAL(InitializeError);
        PROCESS_VAL(GprsNotConnected);
        PROCESS_VAL(IpNotConnected);
        PROCESS_VAL(ConnectionError);
        PROCESS_VAL(NotStartedError);
        PROCESS_VAL(MaxTopicsExceeded);
        PROCESS_VAL(LoopCtrlBusyError);
        PROCESS_VAL(InvalidTopicID);
    }

#undef PROCESS_VAL

    return out << s;
}

extern SerialCom *pSerCom;

void dbgWrite(const unsigned char *data, unsigned short len) {

    for (int i = 0; i < len; ++i) {
        putchar(static_cast<int>(data[i]));
    }
}


bool ticking = true;

void invokeTick() {
    const std::chrono::duration<long long, std::ratio<1LL, 1000LL> >
            tickFrequencyMs = std::chrono::milliseconds(1);

    while (ticking) {
        tick();
        std::this_thread::sleep_for(tickFrequencyMs);
    }
}


bool reverseFlag = false;
int valueCounter = 0;

int getSensorValue() {
    if (valueCounter == 20) {
        reverseFlag = true;
    } else if (valueCounter == 0) {
        reverseFlag = false;
    }
    return (reverseFlag) ? valueCounter-- : valueCounter++;
}


int notification_id = 0;

void notificationHandler(const Notification *n) {
    // Handle your notifications here
    notification_id++;

    std::cout << std::setw(9) << "\033[0;34m[ NOTIFY   ]\033[0m ";

    std::cout << "----- New notification "
              << std::setw(3)
              << std::setfill('0')
              << std::right
              << notification_id
              << " -----" << std::endl;

    std::cout << std::setfill(' ');

    std::cout << std::setw(12)
              << "\033[0;34m[----------]\033[0m "
              << std::setw(14)
              << std::left
              << "State:"
              << n->state
              << std::endl;

    std::cout << std::setw(12)
              << "\033[0;34m[----------]\033[0m "
              << std::setw(14)
              << std::left
              << "Action:"
              << n->action
              << std::endl;

    std::cout << std::setw(12)
              << "\033[0;34m[----------]\033[0m "
              << std::setw(14)
              << std::left
              << "ReturnCode:"
              << n->returnCode
              << std::endl;

    std::cout << std::setw(12)
              << "\033[0;34m[----------]\033[0m "
              << std::setw(14)
              << std::left
              << "ErrorNumber:"
              << n->errorNumber
              << std::endl;
}

bool messageArrived = false;
int message_counter = 0;

void subscriptionHandler(MessageData *msg) {
    messageArrived = true;
    message_counter++;

    std::cout << std::setw(9)
              << "\033[0;35m[ MSG      ]\033[0m ";

    std::cout << "----- New message "
              << std::setw(3)
              << std::setfill('0')
              << std::right
              << message_counter
              << " -----"
              << std::endl;

    std::cout << std::setfill(' ');
    std::cout << std::setw(12)
              << "\033[0;35m[----------]\033[0m "
              << std::setw(14)
              << std::left
              << "QoS:"
              << msg->message->qos
              << std::endl;

    std::cout << std::setw(12)
              << "\033[0;35m[----------]\033[0m "
              << std::setw(14)
              << std::left
              << "Id:"
              << msg->message->id
              << std::endl;

    std::cout << std::setw(12)
              << "\033[0;35m[----------]\033[0m "
              << std::setw(14)
              << std::left
              << "Payload:"
              << (char *) msg->message->payload
              << std::endl;

    std::cout << std::setw(12)
              << "\033[0;35m[----------]\033[0m "
              << std::setw(14)
              << std::left
              << "Topic:"
              << (char *) msg->topicName
              << std::endl;
}


unsigned char init(char *imsi, char *pw, char *ser) {
    setDebugWriteFunction(dbgWrite);

    pSerCom = new SerialCom(ser);
    pSerCom->connect();

    NbiotCoreConf core_conf;
    core_conf.tickFrequency = 1000;
    core_conf.readstatus_fu = readStatus;
    core_conf.readbyte_fu = readByte;
    core_conf.putchar_fu = writeByte;
    core_conf.writestatus_fu = writeStatus;
    core_conf.apn = "";
    core_conf.apnUser = "";
    core_conf.apnPwd = "";
    core_conf.plmn = "26201";

    unsigned int retCoreConf = nbiotCoreConfig(&core_conf);
    if (NoError != (~(CoreWarnApnUser | CoreWarnApnPwd) & retCoreConf))
    {
        std::cerr << std::setw(12)
                  << "\033[0;31m[ DEBUG    ]\033[0m "
                  << std::setw(14)
                  << std::left
                  << "\033[0;31mError in core config: "
                  << retCoreConf
                  << "\033[0m "
                  << std::endl;
    }

    NbiotConf conf;
    conf.keepAliveInterval = 10000;
    conf.autoPollInterval = 300;
    conf.maxTopicCount = 3;
    conf.gateway = "172.25.102.151";
    conf.port = 1883;
    conf.notify_fu = notificationHandler;
    conf.pollInterval = 10000;
    conf.maxResend = 5;
    conf.login = imsi;
    conf.password = pw;

    unsigned int retConf = nbiotConfig(&conf);
    if (NoError != retConf) {
        std::cerr << std::setw(12)
                  << "\033[0;31m[ DEBUG    ]\033[0m "
                  << std::setw(14)
                  << std::left
                  << "\033[0;31mError in config: "
                  << retConf
                  << "\033[0m "
                  << std::endl;
    }

    unsigned char ret = 0;
    if (retConf == NoError && !(~(CoreWarnApnUser | CoreWarnApnPwd) & retCoreConf)) {
        // Setup the statemachine, initialize internal varibles.
        ret = pSerCom->isConnected() ? nbiotStart() : 0;
    } else {
        ret = 0;
    }

    if (ret == 1) {
        std::cout << std::setw(12)
                  << "\033[0;32m[ DEBUG    ]\033[0m "
                  << std::setw(14)
                  << std::left
                  << "\033[0;32mInit Successfull\033[0m "
                  << std::endl;
    } else {
        std::cerr << std::setw(12)
                  << "\033[0;31m[ DEBUG    ]\033[0m "
                  << std::setw(14)
                  << std::left
                  << "\033[0;31mInit Error:\033[0m "
                  << std::endl;
    }

    return ret;
}


void usage() {
    std::cout << "Usage: \r\n"
              << "\t-h Print usage and exit.\r\n"
              << "\t-i imsi - mandatory parameter 15 digit imsi\r\n"
              << "\t-p password - mandatory parameter 8 characters\r\n"
              << "\t-s serial - mandatory parameter, path to serial device (e.g. /dev/ttyACM0)"
              << std::endl;
}


int main(int argc, char **argv) {

    char *imsi = nullptr;
    char *pw = nullptr;
    char *ser = nullptr;

    int c = -1;
    while ((c = getopt(argc, argv, "hi:p:s:")) != -1) {
        switch (c) {
            case 'i': {
                imsi = optarg;
                if (15 == strlen(imsi)) {
                    std::cout << std::setw(12)
                              << "\033[0;32m[ DEBUG    ]\033[0m "
                              << std::setw(14)
                              << std::left
                              << "imsi: " << imsi << std::endl;
                } else {

                    std::cerr << std::setw(12)
                              << "\033[0;31m[ DEBUG    ]\033[0m "
                              << std::setw(14)
                              << std::left << "imsi: invalid value (imsi has to be of length 15) - aborting"
                              << std::endl;
                    imsi = nullptr;
                }
                break;
            }
            case 'p': {
                pw = optarg;
                if (8 == strlen(pw)) {
                    std::cout << std::setw(12)
                              << "\033[0;32m[ DEBUG    ]\033[0m "
                              << std::setw(14)
                              << std::left
                              << "pw: " << pw << std::endl;
                } else {
                    std::cerr << std::setw(12)
                              << "\033[0;31m[ DEBUG    ]\033[0m "
                              << std::setw(14)
                              << std::left
                              << "pw: invalid value (pw has to be of length 8) - aborting" << std::endl;
                    pw = nullptr;
                }
                break;
            }
            case 's': {
                ser = optarg;
                if (-1 != access(ser, F_OK)) {
                    std::cout << std::setw(12)
                              << "\033[0;32m[ DEBUG    ]\033[0m "
                              << std::setw(14)
                              << std::left
                              << "serial node: " << ser << std::endl;
                } else {
                    std::cerr << std::setw(12)
                              << "\033[0;31m[ DEBUG    ]\033[0m "
                              << std::setw(14)
                              << std::left << "serial: no device node with path \"" << ser << " exists - aborting"
                              << std::endl;
                    ser = nullptr;
                }
                break;
            }
            case 'h': {
                usage();
                exit(EXIT_SUCCESS);
            }
            default: {
                std::cout << "skipping unsupported option: " << c << std::endl;
            }
                break;
        }
    }

    if (nullptr == imsi || nullptr == pw || nullptr == ser) {

        std::cerr << std::setw(12)
                  << "\033[0;31m[ ERROR    ]\033[0m "
                  << std::setw(14)
                  << std::left
                  << "parameters imsi, pw and serial, have to be set as described in usage: \r\n" << std::endl;

        usage();
        exit(EXIT_FAILURE);
    }

    // Set topics
    char topicCmd[32];
    sprintf(topicCmd, "NBIoT/%s/CMD/MyCmd", imsi);

    char topicInf[32];
    sprintf(topicInf, "NBIoT/%s/INF/MyCmd", imsi);

    char topicTemp[32];
    sprintf(topicTemp, "NBIoT/%s/MES/1", imsi);

    // Start timer thread.
    std::thread *ticker = new std::thread(invokeTick);


    // Initialize library
    unsigned char retInit = init(imsi, pw, ser);

    NbiotResult rc = LC_Pending;

    while (true && retInit) {
        if (isNbiotConnected()) {
            if (LC_Idle == rc) {
                // If not subscribed already, subscribe to command topic.
                if (!isNbiotSubscribedTo(topicCmd)) {
                    nbiotSubscribe(topicCmd, subscriptionHandler);
                }

                if (messageArrived) {
                    nbiotPublish(topicInf, "0", 1, QOS0);
                    messageArrived = false;
                } else {
                    const std::string s = std::to_string(getSensorValue());

                    std::cout << std::setw(9)
                              << "\033[0;32m[ DEBUG    ]\033[0m "
                              << "Publish: "
                              << s
                              << std::endl;

                    nbiotPublish(topicTemp, s.c_str(), s.length(), QOS1);
                }
            }
        } else {
            if (LC_Idle == rc) {
                nbiotConnect(1); // Connect with cleanSession=1
            }
        }

        rc = nbiotEventLoop(EventModeActive);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    ticking = false;
    ticker->join();

    exit(EXIT_SUCCESS);
}
