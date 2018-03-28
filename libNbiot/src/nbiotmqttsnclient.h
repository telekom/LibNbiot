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



