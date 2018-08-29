/* ========================================================================
 * LibNbiot: nbiotstm.cpp
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

#include <choice.h>
#include <state.h>
#include <transition.h>
#include <eventdispatcher.h>
#include <nbiotcoreapp.h>
#include <nbiottimer.h>

#include "nbiotstm.h"

namespace nbiot
{
NbiotStm::NbiotStm(NbiotStmDataPool &dataPool) :
    AbstractStateMachine(new nbiot::Timer()),
    m_initialize(new Transition()),
    m_initialTimeout(new Transition()),
    m_isInitializedDisconnected(new Transition()),
    m_isInitializedError(new Transition()),
    m_connect(new Transition()),
    m_isNotAttached(new Transition()),
    m_isAttachedIpConnect(new Transition()),
    m_isAttachedError(new Transition()),
    m_isIpConnectedError(new Transition()),
    m_isConnectedConnected(new Transition()),
    m_waitConnectIsConnected(new Transition()),
    m_isConnectedError(new Transition()),
    m_sleep(new Transition()),
    m_wakeup(new Transition()),
    m_sleepAwake(new Transition()),
    m_awakeSleep(new Transition()),
    m_awakeActive(new Transition()),
    m_awakeTimeout(new Transition()),
    m_hibernate(new Transition()),
    m_reInitialize(new Transition()),
    m_disconnect(new Transition()),
    m_errorDisconnect(new Transition()),
    m_errorReInitialize(new Transition()),
    m_errorTimeout(new Transition()),
    m_disSleep(new Transition()),
    m_disWakeup(new Transition()),
    m_disHibernate(new Transition()),
    m_isInitialized(new Choice()),
    m_isAttached(new Choice()),
    m_isIpConnected(new Choice()),
    m_Initial(new State(this)),
    m_Disconnected(new State(this)),
    m_Error(new State(this)),
    m_Connected(new State(this)),
    m_ConnectedSleep(new State(this)),
    m_ConnectedAwake(new State(this)),
    m_DeepSleep(new State(this)),
    m_DisconnectedSleep(new State(this)),
    m_WaitForAttach(new State(this)),
    m_WaitForConnect(new State(this)),
    m_dataPool(dataPool),
    m_evDisp(new EventDispatcher(this)),
    m_notifyHandler(nullptr),
    m_attachLoopClient(LI_Cgatt)
{
    m_attachLoopClient.setLoopStartHandler(this, &NbiotStm::startAttachLoop);
    m_attachLoopClient.setLoopStepHandler(this, &NbiotStm::doAttachLoop);
    m_attachLoopClient.setLoopStopHandler(this, &NbiotStm::finishAttachLoop);
}

NbiotStm::~NbiotStm()
{
    delete m_evDisp;

    delete m_initialize;
    delete m_initialTimeout;
    delete m_isInitializedDisconnected;
    delete m_isInitializedError;
    delete m_connect;
    delete m_isNotAttached;
    delete m_isAttachedIpConnect;
    delete m_isAttachedError;
    delete m_isIpConnectedError;
    delete m_isConnectedConnected;
    delete m_waitConnectIsConnected;
    delete m_isConnectedError;
    delete m_sleep;
    delete m_wakeup;
    delete m_sleepAwake;
    delete m_awakeSleep;
    delete m_awakeActive;
    delete m_awakeTimeout;
    delete m_hibernate;
    delete m_reInitialize;
    delete m_disconnect;
    delete m_errorDisconnect;
    delete m_errorReInitialize;
    delete m_errorTimeout;
    delete m_disSleep;
    delete m_disWakeup;
    delete m_disHibernate;

    delete m_isInitialized;
    delete m_isAttached;
    delete m_isIpConnected;

    delete m_Initial;
    delete m_Disconnected;
    delete m_Error;
    delete m_Connected;
    delete m_ConnectedSleep;
    delete m_ConnectedAwake;
    delete m_DeepSleep;
    delete m_DisconnectedSleep;
    delete m_WaitForAttach;
    delete m_WaitForConnect;
}

void NbiotStm::startStateMachine()
{
    State* initial = getInitialState();
    if(nullptr != initial)
    {
        StmEvent event(StmEvent::Invalid);
        initial->enter(event);
        event.setCustomEventType(InitializeEvent);
        m_evDisp->pushEvent(event);
        m_evDisp->dispatch();
        m_dataPool.eventLoopLock = false; // unlock eventloop
    }
}

void NbiotStm::setUpStateMachine()
{
    m_initialize->setTarget(m_isInitialized);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    m_initialize->setAction(this, &NbiotStm::actionInitialize);
#endif
    m_initialize->setEventType(InitializeEvent);

    m_initialTimeout->setTarget(m_isInitialized);
    m_initialTimeout->setEventType(StmEvent::Timer);

    m_isInitializedDisconnected->setTarget(m_Disconnected);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    m_isInitializedDisconnected->setAction(this, &NbiotStm::actionIsInitializedDisconnected);
#endif

    m_isInitializedError->setTarget(m_Error);
    m_isInitializedError->setAction(this, &NbiotStm::actionIsInitializedError);


    m_connect->setTarget(m_isAttached);
    m_connect->setAction(this, &NbiotStm::actionConnect);
    m_connect->setEventType(ConnectEvent);

    m_isNotAttached->setTarget(m_WaitForAttach);
    m_isNotAttached->setAction(this, &NbiotStm::actionIsNotAttached);

    m_isAttachedIpConnect->setTarget(m_isIpConnected);
    m_isAttachedIpConnect->setAction(this, &NbiotStm::actionIsAttachedIpConnect);
    m_isAttachedIpConnect->setEventType(AttachedEvent);

    m_isAttachedError->setTarget(m_Error);
    m_isAttachedError->setAction(this, &NbiotStm::actionIsAttachedError);
    m_isAttachedError->setEventType(StmEvent::Timer);


    m_isIpConnectedError->setTarget(m_Error);
    m_isIpConnectedError->setAction(this, &NbiotStm::actionIsIpConnectedError);

    m_isConnectedConnected->setTarget(m_Connected);
    m_isConnectedConnected->setAction(this, &NbiotStm::actionIsConnectedConnected);
    m_isConnectedConnected->setEventType(ConnectedEvent);

    m_waitConnectIsConnected->setTarget(m_WaitForConnect);
    m_waitConnectIsConnected->setAction(this, &NbiotStm::actionWaitConnectIsConnected);

    m_isConnectedError->setTarget(m_Error);
    m_isConnectedError->setAction(this, &NbiotStm::actionIsConnectedError);
    m_isConnectedError->setEventType(StmEvent::Timer);


    m_sleep->setTarget(m_ConnectedSleep);
    m_sleep->setAction(this, &NbiotStm::actionSleep);
    m_sleep->setEventType(SleepEvent);

    m_wakeup->setTarget(m_Connected);
    m_wakeup->setAction(this, &NbiotStm::actionWakeup);
    m_wakeup->setEventType(WakeupEvent);

    m_sleepAwake->setTarget(m_ConnectedAwake);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    m_sleepAwake->setAction(this, &NbiotStm::actionSleepAwake);
#endif
    m_sleepAwake->setEventType(ConnectedAwakeEvent);

    m_awakeSleep->setTarget(m_ConnectedSleep);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    m_awakeSleep->setAction(this, &NbiotStm::actionAwakeSleep);
#endif
    m_awakeSleep->setEventType(AwakeSleepEvent);

    m_awakeActive->setTarget(m_Connected);
    m_awakeActive->setAction(this, &NbiotStm::actionAwakeActive);
    m_awakeActive->setEventType(AwakeActiveEvent);

    m_awakeTimeout->setTarget(m_ConnectedSleep);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    m_awakeTimeout->setAction(this, &NbiotStm::actionAwakeTimeout);
#endif
    m_awakeTimeout->setEventType(StmEvent::Timer);

    m_hibernate->setTarget(m_DeepSleep);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    m_hibernate->setAction(this, &NbiotStm::actionHibernate);
#endif
    m_hibernate->setEventType(HibernateEvent);

    m_reInitialize->setTarget(m_Initial);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    m_reInitialize->setAction(this, &NbiotStm::actionReInitialize);
#endif
    m_reInitialize->setEventType(ReInitializeEvent);

    m_disconnect->setTarget(m_Disconnected);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    m_disconnect->setAction(this, &NbiotStm::actionDisconnect);
#endif
    m_disconnect->setEventType(DisconnectEvent);

    m_errorDisconnect->setTarget(m_Disconnected);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    m_errorDisconnect->setAction(this, &NbiotStm::actionErrorDisconnect);
#endif
    m_errorDisconnect->setEventType(ErrorDisconnectEvent);

    m_errorReInitialize->setTarget(m_Initial);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    m_errorReInitialize->setAction(this, &NbiotStm::actionErrorReInitialize);
#endif
    m_errorReInitialize->setEventType(ReInitializeEvent);

    m_errorTimeout->setTarget(m_Initial);
    m_errorTimeout->setEventType(StmEvent::Timer);

    m_disSleep->setTarget(m_DisconnectedSleep);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    m_disSleep->setAction(this, &NbiotStm::actionDisSleep);
#endif
    m_disSleep->setEventType(SleepEvent);

    m_disWakeup->setTarget(m_Disconnected);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    m_disWakeup->setAction(this, &NbiotStm::actionDisWakeup);
#endif
    m_disWakeup->setEventType(WakeupEvent);

    m_disHibernate->setTarget(m_DeepSleep);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    m_disHibernate->setAction(this, &NbiotStm::actionDisHibernate);
#endif
    m_disHibernate->setEventType(HibernateEvent);


    m_isInitialized->setCondition(this, &NbiotStm::actionIsInitialized);
    m_isInitialized->setTransitions(m_isInitializedDisconnected, m_isInitializedError);

    m_isAttached->setCondition(this, &NbiotStm::actionIsAttached);
    m_isAttached->setTransitions(m_isAttachedIpConnect, m_isNotAttached);

    m_isIpConnected->setCondition(this, &NbiotStm::actionIsIpConnected);
    m_isIpConnected->setTransitions(m_waitConnectIsConnected, m_isIpConnectedError);

    m_Initial->setAction(this, &NbiotStm::actionInitialEntry);
    m_Initial->addTransition(m_initialize);
    m_Initial->addTransition(m_initialTimeout);
    m_Initial->setId(InitialState);

    m_Disconnected->setAction(this, &NbiotStm::actionDisconnectedEntry);
    m_Disconnected->addTransition(m_connect);
    m_Disconnected->addTransition(m_disSleep);
    m_Disconnected->addTransition(m_disHibernate);
    m_Disconnected->setId(DisconnectedState);

    m_Error->setAction(this, &NbiotStm::actionErrorEntry);
    m_Error->addTransition(m_errorDisconnect);
    m_Error->addTransition(m_errorReInitialize);
    m_Error->addTransition(m_errorTimeout);
    m_Error->setId(ErrorState);

    m_Connected->setAction(this, &NbiotStm::actionConnectedEntry);
    m_Connected->addTransition(m_sleep);
    m_Connected->addTransition(m_hibernate);
    m_Connected->addTransition(m_disconnect);
    m_Connected->setId(ConnectedState);

#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    m_ConnectedSleep->setAction(this, &NbiotStm::actionConnectedSleepEntry);
#endif
    m_ConnectedSleep->addTransition(m_wakeup);
    m_ConnectedSleep->addTransition(m_sleepAwake);
    m_ConnectedSleep->setId(ConnectedSleepState);

    m_ConnectedAwake->setAction(this, &NbiotStm::actionConnectedAwakeEntry);
    m_ConnectedAwake->addTransition(m_awakeSleep);
    m_ConnectedAwake->addTransition(m_awakeActive);
    m_ConnectedAwake->addTransition(m_awakeTimeout);
    m_ConnectedAwake->setId(ConnectedAwakeState);

    m_DeepSleep->setAction(this, &NbiotStm::actionDeepSleepEntry);
    m_DeepSleep->addTransition(m_reInitialize);
    m_DeepSleep->setId(DeepSleepState);

#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    m_DisconnectedSleep->setAction(this, &NbiotStm::actionDisconnectedSleepEntry);
#endif
    m_DisconnectedSleep->addTransition(m_disWakeup);
    m_DisconnectedSleep->setId(DisconnectedSleepState);

    m_WaitForAttach->setAction(this, &NbiotStm::actionWaitForAttachEntry);
    // ---
    //m_WaitForGprs->addTransition(m_checkGprsEnabled);
    // +++
    m_WaitForAttach->addTransition(m_isAttachedIpConnect);
    // +++
    m_WaitForAttach->addTransition(m_isAttachedError);
    m_WaitForAttach->setId(WaitForAttachState);

    m_WaitForConnect->setAction(this, &NbiotStm::actionWaitForConnectEntry);
    // ---
    //m_WaitForConnect->addTransition(m_waitConnectIsConnected);
    // +++
    m_WaitForConnect->addTransition(m_isConnectedConnected);
    // +++
    m_WaitForConnect->addTransition(m_isConnectedError);
    m_WaitForConnect->setId(WaitForConnectState);

    setInitialState(m_Initial);
}


const char* NbiotStm::currentStateToString() const
{
#ifdef DEBUG_MQTT
    uint32_t currentStateId = getCurrentState()->getId();
    switch(currentStateId)
    {
        case InitialState:
            return "Initial State";
        case DisconnectedState:
            return "Disconnected State";
        case ErrorState:
            return "Error State";
        case ConnectedState:
            return "Connected State";
        case ConnectedSleepState:
            return "Sleep State";
        case DeepSleepState:
            return "Hibernate State";
        case InvalidState:
        /* fallthru */
        default:
            return "Invalid State";
    }
#else
    return "";
#endif
}

void NbiotStm::doReconnect()
{
    unsigned char oldCleanSession = m_dataPool.m_cleanSession; // save cleansession flag
    m_dataPool.m_cleanSession = 0; // no clean session at reconnect after sleep
    sendEvent(ConnectEvent); // try to reconnect
    m_dataPool.m_cleanSession = oldCleanSession; // restore cleansession flag
}

#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
bool NbiotStm::actionInitialize(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionInitialize\r\n");
#endif
    return false;
}

bool NbiotStm::actionIsInitializedDisconnected(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionIsInitializedDisconnected\r\n");
#endif
    return false;
}
#endif

bool NbiotStm::actionIsInitializedError(const StmEvent& e)
{
    (void)e;
    m_dataPool.m_errno = InitializeError;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionIsInitializedError %d\r\n",m_dataPool.m_errno);
#endif
    return false;
}

bool NbiotStm::actionConnect(const StmEvent& e)
{
    (void)e;
    m_dataPool.m_errno = Success;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
        debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
        debugPrintf("STM:actionConnect\r\n");
#endif
    return false;
}

bool NbiotStm::actionIsNotAttached(const StmEvent& e)
{
    (void)e;
    m_dataPool.m_waitTimer.start(timeoutOneMinute);
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
        debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
        debugPrintf("STM:actionIsNotAttached\r\n");
#endif
    doModemAttach();
    return false;
}

bool NbiotStm::actionIsAttachedIpConnect(const StmEvent& e)
{
    (void)e;

    m_dataPool.m_modemRestartOnError = 0;
    m_dataPool.m_waitTimer.clear();
    if(!m_dataPool.gateway.empty())
    {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
        debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
        debugPrintf("Connecting to %s:%d\r\n", m_dataPool.gateway.c_str(), m_dataPool.mqttPort);
#endif
        int rc = NbiotCoreApp::getInstance().getNetworkInstance().connect(m_dataPool.gateway.getData(), m_dataPool.mqttPort);
        if (rc != 1)
        {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
            debugPrintf("rc from TCP connect is %d\r\n", rc);
#endif
        }
    }
    else
    {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
        debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
        debugPrintf("TCP connect is impossible: No host\r\n");
#endif
    }
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:IsGprsEnabledIpConnect %s\r\n", (NbiotCoreApp::getInstance().getNetworkInstance().isConnected()?"true":"false"));
#endif
    return NbiotCoreApp::getInstance().getNetworkInstance().isConnected();
}

bool NbiotStm::actionIsAttachedError(const StmEvent& e)
{
    (void)e;
    m_dataPool.m_errno = GprsNotConnected;
    m_dataPool.m_modemRestartOnError++;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionIsGprsEnabledError %d\r\n",m_dataPool.m_errno);
#endif
    return false;
}

bool NbiotStm::actionIsIpConnectedError(const StmEvent& e)
{
    (void)e;
    m_dataPool.m_errno = IpNotConnected;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionIsIpConnectedError %d\r\n",m_dataPool.m_errno);
#endif
    return false;
}

bool NbiotStm::actionIsConnectedConnected(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionIsConnectedConnected %d\r\n",m_dataPool.client.isConnected());
#endif
    return true;
}

bool NbiotStm::actionWaitConnectIsConnected(const StmEvent& e)
{
    (void)e;
    bool ret = mqttConnect(m_dataPool.m_cleanSession);
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionWaitConnectIsConnected %d\r\n",ret);
#endif
    return ret;
}

bool NbiotStm::actionIsConnectedError(const StmEvent& e)
{
    (void)e;
    m_dataPool.m_errno = ConnectionError;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionIsConnectedError %d\r\n",m_dataPool.m_errno);
#endif
    return false;
}

bool NbiotStm::actionSleep(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionSleep\r\n");
#endif
    if(0 < m_dataPool.m_duration)
    {
        mqttDisconnect();
    }
    return false;
}

bool NbiotStm::actionWakeup(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionWakeup\r\n");
#endif
    if(0 < m_dataPool.m_duration)
    {
        mqttConnect();
    }
    return false;
}

#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
bool NbiotStm::actionSleepAwake(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionSleepAwake %d\r\n",m_dataPool.m_errno);
#endif
    return false;
}

bool NbiotStm::actionAwakeSleep(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionAwakeSleep %d\r\n",m_dataPool.m_errno);
#endif
    return false;
}
#endif

bool NbiotStm::actionAwakeActive(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionAwakeActive %d\r\n",m_dataPool.m_errno);
#endif
    mqttConnect();
    return false;
}

#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
bool NbiotStm::actionAwakeTimeout(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionAwakeTimeout %d\r\n",m_dataPool.m_errno);
#endif
    return false;
}

bool NbiotStm::actionHibernate(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionHibernate\r\n");
#endif
    return false;
}

bool NbiotStm::actionReInitialize(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionReInitialize\r\n");
#endif
    return false;
}

bool NbiotStm::actionDisconnect(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionDisconnect %d\r\n",m_dataPool.m_errno);
#endif
    return false;
}

bool NbiotStm::actionErrorDisconnect(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionErrorDisconnect %d\r\n",m_dataPool.m_errno);
#endif
    return false;
}

bool NbiotStm::actionErrorReInitialize(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionErrorReInitialize %d\r\n",m_dataPool.m_errno);
#endif
    return false;
}

bool NbiotStm::actionDisSleep(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionDisSleep %d\r\n",m_dataPool.m_errno);
#endif
    return false;
}

bool NbiotStm::actionDisWakeup(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionDisWakeup %d\r\n",m_dataPool.m_errno);
#endif
    return false;
}

bool NbiotStm::actionDisHibernate(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionDisHibernate %d\r\n",m_dataPool.m_errno);
#endif
    return false;
}
#endif


bool NbiotStm::actionIsInitialized(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionIsInitialized %d\r\n",m_dataPool.initialized);
#endif
    return m_dataPool.initialized;
}

bool NbiotStm::actionIsAttached(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionIsAttached %s\r\n", (NbiotCoreApp::getInstance().getModemInstance().isAttached()?"true":"false"));
#endif
    return NbiotCoreApp::getInstance().getModemInstance().isAttached();
}

bool NbiotStm::actionIsIpConnected(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:actionIsIpConnected %s\r\n", (NbiotCoreApp::getInstance().getNetworkInstance().isConnected()?"true":"false"));
#endif
    return NbiotCoreApp::getInstance().getNetworkInstance().isConnected();
}


bool NbiotStm::actionInitialEntry(const StmEvent& e)
{
    bool ok = true;
    bool reInitialize = false;
    if((ReInitializeEvent == static_cast<EventType>(e.getEventType())) ||
       (StmEvent::Timer == static_cast<StmEvent::EventType>(e.getEventType())))
    {
        reInitialize = true;
    }

    if(!reInitialize)
    {
        // reboot modem (switching off and on again)
        ok = NbiotCoreApp::getInstance().getModemInstance().reboot();
    }
    // init modem

    if(ok && !NbiotCoreApp::getInstance().getModemInstance().initialize())
    {
        ok = false;
    }
    if(ok && !reInitialize)
    {
        if(m_dataPool.authLogin.empty())
        {
            NbiotCoreApp::getInstance().getModemInstance().getImsi(m_dataPool.authLogin);
        }
        if(m_dataPool.authLogin.empty())
        {
            ok = false;
        }
        else
        {
            m_dataPool.authentication = m_dataPool.authLogin;
            m_dataPool.authentication += m_dataPool.authPasswd;
        }
    }
    // if it's ReInitialze we need to go out of here in any case
    if(reInitialize)
    {
        postEvent(InitializeEvent);
    }
    m_dataPool.initialized = ok;
    if(m_notifyHandler)
    {
        Notification n;
        n.state = InitialState;
        if(reInitialize)
        {
            n.action = ReInitialize;
        }
        else
        {
            n.action = Initialize;
        }
        n.returnCode = RC_ACCEPTED;
        if(ok)
        {
            n.errorNumber = Success;
        }
        else
        {
            n.errorNumber = InitializeError;
        }
        m_notifyHandler(&n);
    }
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:initialized %d\r\n",m_dataPool.initialized);
#endif
    // set a timeout for this state, to go to error-state or disconnected-state if we're stuck here
    m_evDisp->setTimeout(timeoutOneMinute);
    return m_dataPool.initialized;
}

bool NbiotStm::actionDisconnectedEntry(const StmEvent& e)
{
    (void)e;
    doDisconnect();
    if(m_notifyHandler)
    {
        Notification n;
        n.state = DisconnectedState;
        n.action = Disconnect;
        n.returnCode = RC_ACCEPTED;
        n.errorNumber = static_cast<enum ErrorNumber>(m_dataPool.m_errno); // inform user about previous error
        m_notifyHandler(&n);
    }
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:disconnected %d\r\n",m_dataPool.m_errno);
#endif
    // reset error number
    m_dataPool.m_errno = Success;
    return true;
}

bool NbiotStm::actionErrorEntry(const StmEvent& e)
{
    (void)e;
    if(m_dataPool.initialized)
    {
        /// @TODO modem reboot may not be useful with new NB-IoT modem
        // reboot modem after 'restartModemThreshold' GPRS-errors in series
        if(restartModemThreshold <= m_dataPool.m_modemRestartOnError)
        {
            m_dataPool.m_modemRestartOnError = 0;
            // reboot modem (switching off and on again)
            NbiotCoreApp::getInstance().getModemInstance().reboot();
            // ReInitialize will not reboot modem
            postEvent(ReInitializeEvent);
        }
        else
        {
            postEvent(ErrorDisconnectEvent);
        }
    }
    else
    {
        // set a timeout for this state, to try re-initializing
        m_evDisp->setTimeout(timeoutOneMinute);
    }
    if(m_notifyHandler)
    {
        Notification n;
        n.state = ErrorState;
        n.returnCode = RC_ACCEPTED;
        switch(m_dataPool.m_errno)
        {
        case GprsNotConnected:
        case IpNotConnected:
        case ConnectionError:
            n.action = Connect;
            break;
        case InitializeError:
        case NotStartedError:
        default:
            n.action = Initialize;
            break;
        }
        n.errorNumber = static_cast<enum ErrorNumber>(m_dataPool.m_errno);
        m_notifyHandler(&n);
    }
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:error %d\r\n",m_dataPool.m_errno);
#endif
    return false;
}

bool NbiotStm::actionConnectedEntry(const StmEvent& e)
{
    (void)e;
    m_dataPool.m_errno = Success;
    m_dataPool.m_waitTimer.clear();
    if(!m_dataPool.m_hasSubscription && m_dataPool.m_topicRegistry->hasAnyTopic())
    {
        // handled at GW; depends on cleansession-flag
        if(0 != m_dataPool.m_cleanSession)
            m_dataPool.m_topicRegistry->clearRegistry();
        else
            m_dataPool.m_hasSubscription = true;
    }
    if(m_notifyHandler)
    {
        Notification n;
        n.state = ConnectedState;
        n.action = Connect;
        n.returnCode = RC_ACCEPTED;
        n.errorNumber = Success;
        m_notifyHandler(&n);
    }
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:connected %d\r\n",m_dataPool.m_errno);
#endif
    return true;
}

#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
bool NbiotStm::actionConnectedSleepEntry(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:sleep %d\r\n",m_dataPool.m_errno);
#endif
    return true;
}
#endif

bool NbiotStm::actionConnectedAwakeEntry(const StmEvent& e)
{
    (void)e;
    m_evDisp->setTimeout(NbiotStmDataPool::oneSecondMs * m_dataPool.m_duration);
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:connected awake %d\r\n",m_dataPool.m_errno);
#endif
    return true;
}

bool NbiotStm::actionDeepSleepEntry(const StmEvent& e)
{
    (void)e;
    doDisconnect();
    NbiotCoreApp::getInstance().getModemInstance().switchModem(IModem::MODEM_OFF);
    m_dataPool.initialized = false;
    m_dataPool.m_hasSubscription = false;
    if(m_notifyHandler)
    {
        Notification n;
        n.state = DeepSleepState;
        n.action = Hibernate;
        n.returnCode = RC_ACCEPTED;
        n.errorNumber = Success;
        m_notifyHandler(&n);
    }
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:hibernate %d\r\n",m_dataPool.m_errno);
#endif
    return true;
}

#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
bool NbiotStm::actionDisconnectedSleepEntry(const StmEvent& e)
{
    (void)e;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:sleep (disconnected) %d\r\n",m_dataPool.m_errno);
#endif
    return true;
}
#endif

bool NbiotStm::actionWaitForAttachEntry(const StmEvent& e)
{
    (void)e;
    // ---
    //postEvent(CheckGprsEvent);
    // <+++
    // set a timeout for this state, to escape to error state
    m_evDisp->setTimeout(timeoutOneMinute);
    // +++>
    return true;
}

bool NbiotStm::actionWaitForConnectEntry(const StmEvent& e)
{
    (void)e;
    // ---
    //postEvent(CheckConnectedEvent);
    // <+++
    // set a timeout for this state, to escape to error state
    m_evDisp->setTimeout(timeoutOneMinute);
    // +++>
    return true;
}

void NbiotStm::mqttDisconnect()
{
    if(m_dataPool.client.isConnected())
    {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
        debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
        debugPrintf("MQTT disconnecting\r\n");
#endif
        m_dataPool.client.disconnect(m_dataPool.m_duration);
    }
}

bool NbiotStm::mqttConnect(unsigned char cleanSession)
{
    bool ret = true;
    if(!m_dataPool.client.isConnected())
    {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
        debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
        debugPrintf("MQTT connecting\r\n");
#endif
        MQTTSNPacket_connectData data = MQTTSNPacket_connectData_initializer;
        data.duration = m_dataPool.keepAliveInterval;
        data.cleansession = cleanSession;
        data.clientID.cstring = m_dataPool.authentication.getData();
        int rc = m_dataPool.client.connect(data);
        if (rc != 0)
        {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
            debugPrintf("rc from MQTT connect is %d\r\n", rc);
#endif
            ret = false;
        }
    }
    return ret;
}

void NbiotStm::doDisconnect()
{
    if(m_dataPool.client.isConnected())
    {
        m_dataPool.client.disconnect(m_dataPool.m_duration);
    }
    else
    {
        finishDisconnect();
    }
}

void NbiotStm::finishDisconnect()
{
    if(NbiotCoreApp::getInstance().getNetworkInstance().isConnected())
    {
        NbiotCoreApp::getInstance().getNetworkInstance().disconnect();
    }
    if(m_dataPool.m_loopCtrl.isBusy())
    {
        // clear the loop-controller
        m_dataPool.m_loopCtrl.loopBreak();
    }
}

void NbiotStm::sendEvent(int type)
{
    StmEvent event;
    event.setCustomEventType(type);
    m_evDisp->pushEvent(event);
    m_evDisp->dispatch();
}

void NbiotStm::postEvent(int type)
{
    StmEvent event;
    event.setCustomEventType(type);
    m_evDisp->postEvent(event);
}

void NbiotStm::doModemAttach()
{
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("register CGATT loop client\r\n");
#endif
    m_dataPool.m_modemAttached = false;
    m_attachLoopClient.getTimer().setTime(fiftyFiveSeconds);
    m_dataPool.m_loopCtrl.registerLoopClient(&m_attachLoopClient);
}


bool NbiotStm::startAttachLoop(int& i)
{
    (void) i;
    bool ret = false;

#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("start CGATT loop\r\n");
#endif
    Timer timer = Timer(m_attachLoopClient.getTimer().getTime());
    if (NbiotCoreApp::getInstance().getModemInstance().attach()) // start attachement
    {
        m_attachLoopClient.getTimer().start(timer.remaining());
        m_dataPool.m_modemAttached = false;
        ret = true;
    }

    return ret;
}

bool NbiotStm::doAttachLoop(int& loopTime)
{
    bool ret = true;

#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("CGATT loop: step T = %d\r\n", m_attachLoopClient.getTimer().remaining());
#endif
    loopTime = static_cast<int>(m_attachLoopClient.getTimer().remaining());
    int interval = (loopInterval < loopTime)?loopInterval:loopTime;
    if(0 == interval)
    {
        ret = false;
    }
    else
    {
        Timer timer(interval);

        while(0 < timer.remaining())
        {
            if(NbiotCoreApp::getInstance().getModemInstance().isAttached())
            {
                m_dataPool.m_modemAttached = true;
                ret = false;
                break;
            }
        }
    }

    return ret;
}

bool NbiotStm::finishAttachLoop(int& loopTime)
{
    bool ret = true;

#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("finish CGATT loop\r\n");
#endif
    if(0 == loopTime)
    {
        ret = false;
    }
    else
    {
        if(!NbiotCoreApp::getInstance().getModemInstance().isAttached())
        {
            m_dataPool.m_modemAttached = false;
            ret = false;
        }
        // <+++
        else
        {
            postEvent(AttachedEvent);
        }
        // +++>
    }

    return ret;
}


} // namespace nbiot
