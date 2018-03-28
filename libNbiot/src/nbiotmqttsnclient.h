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


    void loopYield(unsigned long timeout_ms)
    {
        m_yieldLoopClient.setValue(static_cast<int>(timeout_ms));
        m_loopController.registerLoopClient(&m_yieldLoopClient);
    };


    void loopDisconnect(int length)
    {
        m_disLoopClient.setValue(length);
        m_disLoopClient.getTimer().setTime(command_timeout_ms);
        m_loopController.registerLoopClient(&m_disLoopClient);
    };


    void loopUnsubscribe(int length)
    {
        m_unsubLoopClient.setValue(length);
        m_unsubLoopClient.getTimer().setTime(command_timeout_ms);
        m_loopController.registerLoopClient(&m_unsubLoopClient);
    };


    void loopConnect(int length)
    {
        m_conLoopClient.setValue(length);
        m_conLoopClient.getTimer().setTime(command_timeout_ms);
        m_loopController.registerLoopClient(&m_conLoopClient);
    };

    void loopRegister(int length)
    {
        m_regLoopClient.setValue(length);
        m_regLoopClient.getTimer().setTime(command_timeout_ms);
        m_loopController.registerLoopClient(&m_regLoopClient);
    }; 

    void loopPublish(unsigned short id, unsigned long timer_left)
    {
        m_pubLoopClient.getTimer().setTime(timer_left);
        m_pubLoopClient.setValue(id);
        m_loopController.registerLoopClient(&m_pubLoopClient);
    };      


    void loopSubscribe(int length)
    {
        m_subLoopClient.setValue(length);
        m_subLoopClient.getTimer().setTime(command_timeout_ms);
        m_loopController.registerLoopClient(&m_subLoopClient);
    };

    void stopYield()
    {
        yieldRunning = false;
    };

    int registerTopic(MQTTSN_topicid& topicName);
    int pmPing(char* cID);    

protected:

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

    static const int loopInterval = 1000;    
    static const int invalidRC = 255;

    bool yieldRunning;
};

#endif // NBIOTMQTTSNCLIENT_H



