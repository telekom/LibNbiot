/* ========================================================================
 * LibNbiot: nbiotstm.h
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

/*!
 * \brief The NbiotStm class
 */
class NbiotStm : public AbstractStateMachine
{
public:
    /*!
     * \brief The EventType enum
     */
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
        CheckAttachmentEvent,
        CheckConnectedEvent,
        ConnectedEvent,
        AttachedEvent
    };

    /*!
     * \brief NbiotStm
     * \param dataPool
     */
    explicit NbiotStm(NbiotStmDataPool& dataPool);
    /*!
     * \brief ~NbiotStm
     */
    virtual ~NbiotStm();

    /*!
     * \brief startStateMachine
     */
    virtual void startStateMachine();
    /*!
     * \brief setUpStateMachine
     */
    virtual void setUpStateMachine();

    /*!
     * \brief currentStateToString
     * \return
     */
    const char* currentStateToString() const;

    /*!
     * \brief getCurrentStateId
     * \return
     */
    uint32_t getCurrentStateId() { return getCurrentState()->getId(); }

    /*!
     * \brief mqttDisconnect
     */
    void mqttDisconnect();
    /*!
     * \brief mqttConnect
     * \param cleanSession
     * \return
     */
    bool mqttConnect(unsigned char cleanSession = 0);

    /*!
     * \brief doDisconnect
     */
    void doDisconnect();
    /*!
     * \brief finishDisconnect
     */
    void finishDisconnect();
    /*!
     * \brief doReconnect
     */
    void doReconnect();
    /*!
     * \brief sendEvent
     * \param type
     */
    void sendEvent(int type);
    /*!
     * \brief postEvent
     * \param type
     */
    void postEvent(int type);

    /*!
     * \brief doModemAttach
     */
    void doModemAttach();

    /*!
     * \brief startAttachLoop
     * \return
     */
    bool startAttachLoop(int&);
    /*!
     * \brief doAttachLoop
     * \return
     */
    bool doAttachLoop(int&);
    /*!
     * \brief finishAttachLoop
     * \return
     */
    bool finishAttachLoop(int&);

    /*!
     * \brief setNotifyHandler
     * \param nh
     */
    void setNotifyHandler(notifyHandler nh) { m_notifyHandler = nh; }

    /*!
     * \brief eventDispatcher
     * \return
     */
    EventDispatcher* eventDispatcher() { return m_evDisp; }

protected:
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    /*!
     * \brief actionInitialize
     * \param e
     * \return
     */
    bool actionInitialize(const StmEvent& e);
    /*!
     * \brief actionIsInitializedDisconnected
     * \param e
     * \return
     */
    bool actionIsInitializedDisconnected(const StmEvent& e);
#endif
    /*!
     * \brief actionIsInitializedError
     * \param e
     * \return
     */
    bool actionIsInitializedError(const StmEvent& e);
    /*!
     * \brief actionConnect
     * \param e
     * \return
     */
    bool actionConnect(const StmEvent& e);
    /*!
     * \brief actionIsNotAttached starts the attachment-loop and a one minute timeout-timer before moving to the WaitForAttach state
     * \param e not used
     * \return false
     */
    bool actionIsNotAttached(const StmEvent& e);
    /*!
     * \brief actionIsAttachedIpConnect
     * \param e
     * \return
     */
    bool actionIsAttachedIpConnect(const StmEvent& e);
    /*!
     * \brief actionIsAttachedError
     * \param e
     * \return
     */
    bool actionIsAttachedError(const StmEvent& e);
    /*!
     * \brief actionIsIpConnectedError
     * \param e
     * \return
     */
    bool actionIsIpConnectedError(const StmEvent& e);
    /*!
     * \brief actionIsConnectedConnected
     * \param e
     * \return
     */
    bool actionIsConnectedConnected(const StmEvent& e);
    /*!
     * \brief actionWaitConnectIsConnected
     * \param e
     * \return
     */
    bool actionWaitConnectIsConnected(const StmEvent& e);
    /*!
     * \brief actionIsConnectedError
     * \param e
     * \return
     */
    bool actionIsConnectedError(const StmEvent& e);
    /*!
     * \brief actionSleep
     * \param e
     * \return
     */
    bool actionSleep(const StmEvent& e);
    /*!
     * \brief actionWakeup
     * \param e
     * \return
     */
    bool actionWakeup(const StmEvent& e);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    /*!
     * \brief actionSleepAwake
     * \param e
     * \return
     */
    bool actionSleepAwake(const StmEvent& e);
    /*!
     * \brief actionAwakeSleep
     * \param e
     * \return
     */
    bool actionAwakeSleep(const StmEvent& e);
#endif
    /*!
     * \brief actionAwakeActive
     * \param e
     * \return
     */
    bool actionAwakeActive(const StmEvent& e);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    /*!
     * \brief actionAwakeTimeout
     * \param e
     * \return
     */
    bool actionAwakeTimeout(const StmEvent& e);
    /*!
     * \brief actionHibernate
     * \param e
     * \return
     */
    bool actionHibernate(const StmEvent& e);
    /*!
     * \brief actionReInitialize
     * \param e
     * \return
     */
    bool actionReInitialize(const StmEvent& e);
    /*!
     * \brief actionDisconnect
     * \param e
     * \return
     */
    bool actionDisconnect(const StmEvent& e);
    /*!
     * \brief actionErrorDisconnect
     * \param e
     * \return
     */
    bool actionErrorDisconnect(const StmEvent& e);
    /*!
     * \brief actionErrorReInitialize
     * \param e
     * \return
     */
    bool actionErrorReInitialize(const StmEvent& e);
    /*!
     * \brief actionDisSleep
     * \param e
     * \return
     */
    bool actionDisSleep(const StmEvent& e);
    /*!
     * \brief actionDisWakeup
     * \param e
     * \return
     */
    bool actionDisWakeup(const StmEvent& e);
    /*!
     * \brief actionDisHibernate
     * \param e
     * \return
     */
    bool actionDisHibernate(const StmEvent& e);
#endif
    /*!
     * \brief actionIsInitialized
     * \param e
     * \return
     */
    bool actionIsInitialized(const StmEvent& e);
    /*!
     * \brief actionIsAttached
     * \param e
     * \return
     */
    bool actionIsAttached(const StmEvent& e);
    /*!
     * \brief actionIsIpConnected
     * \param e
     * \return
     */
    bool actionIsIpConnected(const StmEvent& e);

    /*!
     * \brief actionInitialEntry
     * \param e
     * \return
     */
    bool actionInitialEntry(const StmEvent& e);
    /*!
     * \brief actionDisconnectedEntry
     * \param e
     * \return
     */
    bool actionDisconnectedEntry(const StmEvent& e);
    /*!
     * \brief actionErrorEntry
     * \param e
     * \return
     */
    bool actionErrorEntry(const StmEvent& e);
    /*!
     * \brief actionConnectedEntry
     * \param e
     * \return
     */
    bool actionConnectedEntry(const StmEvent& e);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    /*!
     * \brief actionConnectedSleepEntry
     * \param e
     * \return
     */
    bool actionConnectedSleepEntry(const StmEvent& e);
#endif
    /*!
     * \brief actionConnectedAwakeEntry
     * \param e
     * \return
     */
    bool actionConnectedAwakeEntry(const StmEvent& e);
    /*!
     * \brief actionDeepSleepEntry
     * \param e
     * \return
     */
    bool actionDeepSleepEntry(const StmEvent& e);
#if defined(UNITTEST_MQTT) || defined (DEBUG_MQTT)
    /*!
     * \brief actionDisconnectedSleepEntry
     * \param e
     * \return
     */
    bool actionDisconnectedSleepEntry(const StmEvent& e);
#endif
    /*!
     * \brief actionWaitForAttachEntry
     * \param e
     * \return
     */
    bool actionWaitForAttachEntry(const StmEvent& e);
    /*!
     * \brief actionWaitForConnectEntry
     * \param e
     * \return
     */
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
    Transition* m_isNotAttached;
    Transition* m_isAttachedIpConnect;
    Transition* m_isAttachedError;
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
    Choice* m_isAttached;
    Choice* m_isIpConnected;

    State* m_Initial;
    State* m_Disconnected;
    State* m_Error;
    State* m_Connected;
    State* m_ConnectedSleep;
    State* m_ConnectedAwake;
    State* m_DeepSleep;
    State* m_DisconnectedSleep;
    State* m_WaitForAttach;
    State* m_WaitForConnect;

    NbiotStmDataPool& m_dataPool;
    EventDispatcher* m_evDisp;
    notifyHandler m_notifyHandler;
    nbiot::LoopClient m_attachLoopClient;

    static const unsigned long fiftyFiveSeconds = 55000;
    static const unsigned long timeoutOneMinute = 60000;

    static const unsigned short restartModemThreshold = 5;
    static const int loopInterval = 1000;
};

} // namespace nbiot

#endif // NBIOTSTM_H
