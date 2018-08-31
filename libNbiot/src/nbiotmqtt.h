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

#ifndef NBIOTMQTT_H
#define NBIOTMQTT_H

#include "nbiotstmdatapool.h"

#include "nbiotdef.h"
#include "nbiotmessage.h"
#include "nbiotnotify.h"
#include "nbiotloop.h"
#include "nbiotmqttsnclient.h"

#include "nbiotstm.h"

/*!
 * \brief The NbiotMQTT class
 */
class NbiotMQTT
{
public:
    /*!
     * \brief NbiotMQTT
     */
    NbiotMQTT();
    /*!
     * \brief ~NbiotMQTT
     */
    virtual ~NbiotMQTT();

    /*!
     * \brief startStateMachine
     */
    void startStateMachine() { m_stm.startStateMachine(); }
    /*!
     * \brief setUpStateMachine
     */
    void setUpStateMachine() { m_stm.setUpStateMachine(); }

    /*!
     * \brief eventLoop
     * \param mode
     * \return
     */
    NbiotResult eventLoop(NbiotEventMode mode);

    /*!
     * \brief currentStateToString
     * \return
     */
    const char* currentStateToString() const { return m_stm.currentStateToString(); }

    /*!
     * \brief validState
     * \return
     */
    bool validState() { return (nullptr != m_stm.getCurrentState()); }
    /*!
     * \brief getCurrentStateId
     * \return
     */
    uint32_t getCurrentStateId() { return m_stm.getCurrentStateId(); }
    /*!
     * \brief getErrorNumber
     * \return
     */
    unsigned int getErrorNumber() { return m_dataPool.m_errno; }

    /*!
     * \brief connect
     * \param cleanSession
     * \return
     */
    int connect(unsigned char cleanSession);
    /*!
     * \brief publish
     * \param topic
     * \param data
     * \param len
     * \param qos
     * \return
     */
    int publish(const char* topic, const char* data, size_t len, QoS qos);
    /*!
     * \brief subscribe
     * \param topic
     * \param mh
     * \return
     */
    int subscribe(const char* topic, messageHandler mh);
    /*!
     * \brief unsubscribe
     * \param topic
     * \return
     */
    int unsubscribe(const char* topic);
    /*!
     * \brief disconnect
     */
    void disconnect();
    /*!
     * \brief sleep
     * \param duration
     */
    void sleep(unsigned short duration);
    /*!
     * \brief hibernate
     * \param duration
     */
    void hibernate(unsigned short duration);
    /*!
     * \brief wakeup
     */
    void wakeup();

    /*!
     * \brief poll
     * \param pollInterval
     */
    void poll(unsigned short pollInterval = 0);

    /*!
     * \brief getMessageCount
     * \return
     */
    unsigned long getMessageCount() const;

    /*!
     * \brief connected
     * \return
     */
    bool connected() { return m_dataPool.client.isConnected(); }
    /*!
     * \brief hasSubscription
     * \return
     */
    bool hasSubscription() const;
    /*!
     * \brief hasSubscription
     * \param topic
     * \return
     */
    bool hasSubscription(const char* topic) const;

    /*!
     * \brief setGateway
     * \param hostname
     */
    void setGateway(const char* hostname) { m_dataPool.gateway = hostname; }
    /*!
     * \brief getGateway
     * \return
     */
    const char* getGateway() const { return m_dataPool.gateway.c_str(); }
    /*!
     * \brief setPort
     * \param port
     */
    void setPort(unsigned short port) { m_dataPool.mqttPort = port; }
    /*!
     * \brief setKeepAlive
     * \param keepAlive
     */
    void setKeepAlive(unsigned int keepAlive) { m_dataPool.keepAliveInterval = keepAlive; }
    /*!
     * \brief setAutoPoll
     * \param autoPoll
     */
    void setAutoPoll(unsigned int autoPoll);
    /*!
     * \brief setTimerCallback
     * \param tcInterval
     */
    void setTimerCallback(unsigned long tcInterval) { m_dataPool.eventLoopExecInterval = tcInterval; }
    /*!
     * \brief setMaxTopics
     * \param maxTopics
     */
    void setMaxTopics(unsigned short maxTopics) { m_dataPool.m_maxTopics = maxTopics; }
    /*!
     * \brief setMaxResend
     * \param maxResend
     */
    void setMaxResend(unsigned short maxResend) { m_dataPool.client.setMaxRetry(maxResend); }
    /*!
     * \brief getClientId
     * \return
     */
    const char* getClientId() const { return m_dataPool.authLogin.c_str(); }
    /*!
     * \brief setPollInterval
     * \param pollInterval
     */
    void setPollInterval(unsigned short pollInterval) { m_pollInterval = pollInterval; }
    /*!
     * \brief setLogin
     * \param login
     */
    void setLogin(const char* login) { m_dataPool.authLogin = login; }
    /*!
     * \brief setPassword
     * \param password
     */
    void setPassword(const char* password) { m_dataPool.authPasswd = password; }

    /*!
     * \deprecated only used in deprecated function of public "C" interface
     */
    bool ready() const { return m_dataPool.initialized; }

    /*!
     * \brief isConnected
     * \return
     */
    bool isConnected() { return (ConnectedState == m_stm.getCurrentStateId()); }
    /*!
     * \brief isDisconnected
     * \return
     */
    bool isDisconnected() { return (DisconnectedState == m_stm.getCurrentStateId()); }
    /*!
     * \brief isConSleeping
     * \return
     */
    bool isConSleeping() { return (ConnectedSleepState == m_stm.getCurrentStateId()); }
    /*!
     * \brief isConAwake
     * \return
     */
    bool isConAwake() { return (ConnectedAwakeState == m_stm.getCurrentStateId()); }
    /*!
     * \brief isDisSleeping
     * \return
     */
    bool isDisSleeping() { return (DisconnectedSleepState == m_stm.getCurrentStateId()); }
    /*!
     * \brief isSleeping
     * \return
     */
    bool isSleeping() { return ( isConSleeping() || isConAwake() || isDisSleeping() ); }

    /*!
     * \brief pubackNotify
     * \param topic
     */
    void pubackNotify(nbiot::NbiotTopic& topic);
    /*!
     * \brief registerNotify
     * \param topic
     * \return
     */
    int registerNotify(nbiot::NbiotTopic& topic);
    /*!
     * \brief disconnectNotify
     * \param duration
     */
    void disconnectNotify(int duration);
    /*!
     * \brief pingRespNotify
     * \param duration
     */
    void pingRespNotify(int duration);

    /*!
     * \brief setNotifyHandler
     * \param nh
     */
    void setNotifyHandler(notifyHandler nh)
    {
        m_notifyHandler = nh;
        m_stm.setNotifyHandler(nh);
    }

    /*!
     * \brief eventLoopStatus
     * \return
     */
    NbiotResult eventLoopStatus() { return ((LC_Pending == m_evLoopRc) || m_loopController.isBusy())?LC_Pending:LC_Idle; }


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
    unsigned short m_pollInterval;

    static const unsigned int tenSecondsMs = 10000;
    static const unsigned int halfSecondMs = 500;
    static const unsigned short oneSecondMs = 1000;
};

#endif // NBIOTMQTT_H
