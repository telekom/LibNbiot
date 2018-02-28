#ifndef NBIOTSTMDATAPOOL_H
#define NBIOTSTMDATAPOOL_H

#include <nbiotcoreapp.h>
#include <inetwork.h>
#include <nbiottimer.h>
#include "nbiotdef.h"
#include "nbiotstring.h"
#include "nbiotloop.h"
#include "nbiottopicregistry.h"

#include "MQTTSNClient.h"

#define MAX_PACKET_SIZE 200
#define MAX_MSG_HANDLER 0

namespace nbiot
{

class NbiotStmDataPool
{
public:
    explicit NbiotStmDataPool(NbiotLoop& lc) :
        m_errno(NotStartedError),
        m_topicid({MQTTSN_TOPIC_TYPE_NORMAL,invalidTopicId}),
        m_currentTopic(),
        client(NbiotCoreApp::getInstance().getNetworkInstance(), lc, m_topicid, clientCommandTimeout),
        m_topicRegistry(new NbiotTopicRegistry()),
        initialized(false),
        m_hasSubscription(false),
        imsi(),
        imsiAuth(),
        gateway(),
        mqttPort(defaultMqttPort),
        keepAliveInterval(minKeepAlive),
        autoPollInterval(0),
        eventLoopExecInterval(oneSecondMs),
        eventLoopLock(true),
        eventLoopMs(0l),
        lastPollMs(0l),
        m_cleanSession(0),
        m_autoReconnect(false),
        m_duration(0),
        m_modemRestartOnError(0),
        m_maxTopics(defaultMaxTopics),
        m_modemAttached(false),
        m_waitTimer(),
        m_loopCtrl(lc)
    {
        client.setDefaultMessageHandler(m_topicRegistry, &NbiotTopicRegistry::messageDispatcher);
    }

    virtual ~NbiotStmDataPool() { delete m_topicRegistry; }

    unsigned int m_errno;
    MQTTSN_topicid m_topicid;
    NbiotTopic m_currentTopic;
    MQTTSN::Client<INetwork, nbiot::Timer, MAX_PACKET_SIZE, MAX_MSG_HANDLER> client;
    NbiotTopicRegistry* m_topicRegistry;
    bool initialized;
    bool m_hasSubscription;
    nbiot::string imsi;
    nbiot::string imsiAuth;
    nbiot::string gateway;
    int mqttPort;
    unsigned int keepAliveInterval;
    unsigned int autoPollInterval;
    unsigned long eventLoopExecInterval;
    bool eventLoopLock;
    long long eventLoopMs;
    long long lastPollMs;
    unsigned char m_cleanSession;
    bool m_autoReconnect;
    unsigned short m_duration;
    unsigned short m_modemRestartOnError;
    unsigned short m_maxTopics;
    bool m_modemAttached;
    nbiot::Timer m_waitTimer;
    NbiotLoop& m_loopCtrl;

    static const unsigned int oneSecondMs = 1000;
    static const unsigned short invalidTopicId = 0xFFFF;

private:
    NbiotStmDataPool();
    NbiotStmDataPool(NbiotStmDataPool&);
    NbiotStmDataPool& operator = (NbiotStmDataPool&);

    static const unsigned int minKeepAlive = 60;
    static const unsigned int clientCommandTimeout = 15000;
    static const int defaultMqttPort = 1883;
    static const unsigned short defaultMaxTopics = 20;
};

} // namespace nbiot

#endif // NBIOTSTMDATAPOOL_H

