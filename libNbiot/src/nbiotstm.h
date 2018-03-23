#ifndef NBIOTSTM_H
#define NBIOTSTM_H

#include <abstractstatemachine.h>
#include <stmevent.h>
#include <state.h>

#include "nbiotdef.h"
#include "nbiotstmdatapool.h"
#include "nbiotnotify.h"
class Transition;
class Choice;
class EventDispatcher;
namespace nbiot
{

class NbiotStm : public AbstractStateMachine
{
public:

    enum EventType
    {
        InitializeEvent = StmEvent::CustomMin,
        ConnectEvent,
        SleepEvent,
        WakeupEvent,
        ConnectedAwakeEvent,
        AwakeSleepEvent,
        AwakeActiveEvent,
        HibernateEvent,
        ReInitializeEvent,
        DisconnectEvent,
        ErrorDisconnectEvent,
        CheckGprsEvent,
        CheckConnectedEvent,
        ConnectedEvent,
        AttachedEvent
    };

    explicit NbiotStm(NbiotStmDataPool& dataPool);
    virtual ~NbiotStm();

    virtual void startStateMachine();
    virtual void setUpStateMachine();

    const char* currentStateToString() const;

    uint32_t getCurrentStateId() { return getCurrentState()->getId(); }

    void mqttDisconnect();
    bool mqttConnect(unsigned char cleanSession = 0);

    void doDisconnect();
    void finishDisconnect();
    void doReconnect();
    void sendEvent(int type);
    void postEvent(int type);

    void doModemAttach();

    bool startCgattLoop(int&);
    bool doCgattLoop(int&);
    bool finishCgattLoop(int&);

    void setNotifyHandler(notifyHandler nh) { m_notifyHandler = nh; }

    EventDispatcher* eventDispatcher() { return m_evDisp; }

protected:
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    bool actionInitialize(const StmEvent& e);
    bool actionIsInitializedDisconnected(const StmEvent& e);
#endif
    bool actionIsInitializedError(const StmEvent& e);
    bool actionConnect(const StmEvent& e);
    bool actionIsGprsEnabledIpConnect(const StmEvent& e);
    bool actionIsGprsEnabledError(const StmEvent& e);
    bool actionIsIpConnectedError(const StmEvent& e);
    bool actionIsConnectedConnected(const StmEvent& e);
    bool actionWaitConnectIsConnected(const StmEvent& e);
    bool actionIsConnectedError(const StmEvent& e);
    bool actionSleep(const StmEvent& e);
    bool actionWakeup(const StmEvent& e);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    bool actionSleepAwake(const StmEvent& e);
    bool actionAwakeSleep(const StmEvent& e);
#endif
    bool actionAwakeActive(const StmEvent& e);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    bool actionAwakeTimeout(const StmEvent& e);
    bool actionHibernate(const StmEvent& e);
    bool actionReInitialize(const StmEvent& e);
    bool actionDisconnect(const StmEvent& e);
    bool actionErrorDisconnect(const StmEvent& e);
    bool actionErrorReInitialize(const StmEvent& e);
    bool actionDisSleep(const StmEvent& e);
    bool actionDisWakeup(const StmEvent& e);
    bool actionDisHibernate(const StmEvent& e);
#endif
    bool actionIsInitialized(const StmEvent& e);
    bool actionIsIpConnected(const StmEvent& e);

    bool actionInitialEntry(const StmEvent& e);
    bool actionDisconnectedEntry(const StmEvent& e);
    bool actionErrorEntry(const StmEvent& e);
    bool actionConnectedEntry(const StmEvent& e);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    bool actionConnectedSleepEntry(const StmEvent& e);
#endif
    bool actionConnectedAwakeEntry(const StmEvent& e);
    bool actionDeepSleepEntry(const StmEvent& e);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    bool actionDisconnectedSleepEntry(const StmEvent& e);
#endif
    bool actionWaitForGprsEntry(const StmEvent& e);
    bool actionWaitForConnectEntry(const StmEvent& e);

private:
    NbiotStm();
    NbiotStm(NbiotStm&);
    NbiotStm& operator = (NbiotStm&);


    Transition* m_initialize;
    Transition* m_initialTimeout;
    Transition* m_isInitializedDisconnected;
    Transition* m_isInitializedError;
    Transition* m_connect;
    Transition* m_isGprsEnabledIpConnect;
    Transition* m_isGprsEnabledError;
    Transition* m_isIpConnectedError;
    Transition* m_isConnectedConnected;
    Transition* m_waitConnectIsConnected;
    Transition* m_isConnectedError;
    Transition* m_sleep;
    Transition* m_wakeup;
    Transition* m_sleepAwake;
    Transition* m_awakeSleep;
    Transition* m_awakeActive;
    Transition* m_awakeTimeout;
    Transition* m_hibernate;
    Transition* m_reInitialize;
    Transition* m_disconnect;
    Transition* m_errorDisconnect;
    Transition* m_errorReInitialize;
    Transition* m_errorTimeout;
    Transition* m_disSleep;
    Transition* m_disWakeup;
    Transition* m_disHibernate;

    Choice* m_isInitialized;
    Choice* m_isIpConnected;

    State* m_Initial;
    State* m_Disconnected;
    State* m_Error;
    State* m_Connected;
    State* m_ConnectedSleep;
    State* m_ConnectedAwake;
    State* m_DeepSleep;
    State* m_DisconnectedSleep;
    State* m_WaitForGprs;
    State* m_WaitForConnect;

    NbiotStmDataPool& m_dataPool;
    EventDispatcher* m_evDisp;
    notifyHandler m_notifyHandler;
    nbiot::LoopClient m_cgattLoopClient;

    static const unsigned int fiftyFiveSeconds = 55000;
    static const unsigned long timeoutOneMinute = 60000;

    static const unsigned short restartModemThreshold = 5;
    static const int loopInterval = 1000;
};

} // namespace nbiot

#endif // NBIOTSTM_H
