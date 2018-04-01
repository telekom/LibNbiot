/* ========================================================================
 * LibNbiot: nbiotmqtt.h
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

#ifndef NBIOTMQTTSNCLIENT_H
#define NBIOTMQTTSNCLIENT_H


#include <nbiottimer.h>
#include <itimer.h>
#include <nbiotloopclient.h>
#include <inetwork.h>
#include <nbiotcoreapp.h>

#define MQTTCLIENT_QOS1 1
#include "MQTTSNClient.h"


class NbiotMqttSnClient : public MQTTSN::Client <NbiotMqttSnClient, INetwork, nbiot::Timer, 200, 5> 
{

public:

    NbiotMqttSnClient(nbiot::NbiotLoop& lc, MQTTSN_topicid& topicName, unsigned int command_timeout_ms = 20000);

    void loopConnect(int length);
    void loopDisconnect(int length);

    void loopSubscribe(int length);
    void loopUnsubscribe(int length);

    int registerTopic(MQTTSN_topicid& topicName);
    void loopRegister(int length); 
    void loopPublish(unsigned short id, unsigned long timer_left);

    void loopYield(unsigned long timeout_ms);
    void stopYield();

    unsigned char notifyRegister(unsigned short topicid, MQTTSNString topicName );
    void notifyPublish(unsigned short topicid, unsigned char returncode);

    void notifyPingResponse();
    void notifyDisconnect();
    

    int pmPing(char* cID);

    template<class NHC>
    void setPubackNotifyHandler(NHC* nhc, void (NHC::*paNotifyHandler)(nbiot::NbiotTopic&))
    {
        pubackNotifyHandler.attach(nhc, paNotifyHandler);
    }

    template<class NHC>
    void setRegisterNotifyHandler(NHC* nhc, int (NHC::*regNotifyHandler)(nbiot::NbiotTopic&))
    {
        registerNotifyHandler.attach(nhc, regNotifyHandler);
    }

    template<class NHC>
    void setDisconnectNotifyHandler(NHC* nhc, void (NHC::*disNotifyHandler)(int))
    {
        disconnectNotifyHandler.attach(nhc, disNotifyHandler);
    }

    template<class NHC>
    void setPingRespNotifyHandler(NHC* nhc, void (NHC::*pingrespNotifyHandler)(int))
    {
        pingRespNotifyHandler.attach(nhc, pingrespNotifyHandler);
    }

private:

    bool startConLoop(int&);
    bool doConLoop(int&);
    bool finishConLoop(int&);

    bool startPubLoop(int&);
    bool doPubLoop(int&);
    bool finishPubLoop(int&);

    bool startRegLoop(int&);
    bool doRegLoop(int&);
    bool finishRegLoop(int&);

    bool startSubLoop(int&);
    bool doSubLoop(int&);
    bool finishSubLoop(int&);

    bool startUnsubLoop(int&);
    bool doUnsubLoop(int&);
    bool finishUnsubLoop(int&);

    bool startYieldLoop(int&);
    bool doYieldLoop(int&);
    bool finishYieldLoop(int&);    

    bool startDisLoop(int&);
    bool doDisLoop(int&);
    bool finishDisLoop(int&);    

    bool loopWait(int breakValue, int interval);

    nbiot::NbiotLoop& m_loopController;

    nbiot::LoopClient m_conLoopClient;
    nbiot::LoopClient m_pubLoopClient;
    nbiot::LoopClient m_regLoopClient;
    nbiot::LoopClient m_subLoopClient;
    nbiot::LoopClient m_unsubLoopClient;
    nbiot::LoopClient m_yieldLoopClient;
    nbiot::LoopClient m_disLoopClient;

    FP<void, nbiot::NbiotTopic&> pubackNotifyHandler;
    FP<int, nbiot::NbiotTopic&> registerNotifyHandler;
    FP<void, int> disconnectNotifyHandler;
    FP<void, int> pingRespNotifyHandler;    

    static const int loopInterval = 1000;    
    static const int invalidRC = 255;

    bool yieldRunning;
};

#endif // NBIOTMQTTSNCLIENT_H



