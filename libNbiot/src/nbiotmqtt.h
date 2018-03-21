#ifndef NBIOTMQTT_H
#define NBIOTMQTT_H

#include "nbiotstmdatapool.h"

#include "nbiotdef.h"
#include "nbiotmessage.h"
#include "nbiotnotify.h"
#include "nbiotloop.h"
#include "MQTTSNClient.h"

#include "nbiotstm.h"

class NbiotMQTT
{
public:
    NbiotMQTT();
    virtual ~NbiotMQTT();

    void startStateMachine() { m_stm.startStateMachine(); }
    void setUpStateMachine() { m_stm.setUpStateMachine(); }

    NbiotResult eventLoop(NbiotEventMode mode);

    const char* currentStateToString() const { return m_stm.currentStateToString(); }

    bool validState() { return (nullptr != m_stm.getCurrentState()); }
    uint32_t getCurrentStateId() { return m_stm.getCurrentStateId(); }
    unsigned int getErrorNumber() { return m_dataPool.m_errno; }

    int connect(unsigned char cleanSession);
    int publish(const char* topic, const char* data, size_t len, QoS qos);
    int subscribe(const char* topic, messageHandler mh);
    int unsubscribe(const char* topic);
    void disconnect();
    void sleep(unsigned short duration);
    void hibernate(unsigned short duration);
    void wakeup();

    void poll();

    unsigned long getMessageCount() const;

    bool connected() { return m_dataPool.client.isConnected(); }
    bool hasSubscription() const;
    bool hasSubscription(const char* topic) const;

    void setGateway(const char* hostname) { m_dataPool.gateway = hostname; }
    const char* getGateway() const { return m_dataPool.gateway.c_str(); }
    void setPort(unsigned short port) { m_dataPool.mqttPort = port; }
    void setKeepAlive(unsigned int keepAlive) { m_dataPool.keepAliveInterval = keepAlive; }
    void setAutoPoll(unsigned int autoPoll) { m_dataPool.autoPollInterval = autoPoll; }
    void setTimerCallback(unsigned long tcInterval) { m_dataPool.eventLoopExecInterval = tcInterval; }
    void setMaxTopics(unsigned short maxTopics) { m_dataPool.m_maxTopics = maxTopics; }
    const char* getClientId() const { return m_dataPool.imsi.c_str(); }

    /*!
     * \deprecated only used in deprecated function of public "C" interface
     */
    bool ready() const { return m_dataPool.initialized; }

    bool isConnected() { return (ConnectedState == m_stm.getCurrentStateId()); }
    bool isDisconnected() { return (DisconnectedState == m_stm.getCurrentStateId()); }
    bool isConSleeping() { return (ConnectedSleepState == m_stm.getCurrentStateId()); }
    bool isConAwake() { return (ConnectedAwakeState == m_stm.getCurrentStateId()); }
    bool isDisSleeping() { return (DisconnectedSleepState == m_stm.getCurrentStateId()); }
    bool isSleeping() { return ( isConSleeping() || isConAwake() || isDisSleeping() ); }

    void pubackNotify(nbiot::NbiotTopic& topic);
    int registerNotify(nbiot::NbiotTopic& topic);
    void disconnectNotify(int duration);
    void pingRespNotify(int duration);

    void setNotifyHandler(notifyHandler nh)
    {
        m_notifyHandler = nh;
        m_stm.setNotifyHandler(nh);
    }


private:
    NbiotMQTT(NbiotMQTT&);
    NbiotMQTT& operator = (NbiotMQTT&);

    void sendEvent(int type) { m_stm.sendEvent(type); }
    void postEvent(int type) { m_stm.postEvent(type); }

    nbiot::NbiotLoop m_loopController;
    nbiot::NbiotStmDataPool m_dataPool;
    nbiot::NbiotStm m_stm;
    notifyHandler m_notifyHandler;
    NbiotResult m_evLoopRc;

    static const unsigned int tenSecondsMs = 10000;
    static const unsigned int halfSecondMs = 500;
};

#endif // NBIOTMQTT_H
