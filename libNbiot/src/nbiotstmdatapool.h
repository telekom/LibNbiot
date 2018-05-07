/* ========================================================================
 * LibNbiot: nbiotstmdatapool.h
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

#ifndef NBIOTSTMDATAPOOL_H
#define NBIOTSTMDATAPOOL_H

#include <nbiotcoreapp.h>
#include <inetwork.h>
#include <nbiottimer.h>
#include "nbiotdef.h"
#include "nbiotstring.h"
#include "nbiotloop.h"
#include "nbiottopicregistry.h"
#include "nbiotmqttsnclient.h"

#define MAX_PACKET_SIZE 200
#define MAX_MSG_HANDLER 0

namespace nbiot
{

/*!
 * \brief The NbiotStmDataPool class
 */
class NbiotStmDataPool
{
public:
    /*!
     * \brief NbiotStmDataPool
     * \param lc
     */
    explicit NbiotStmDataPool(NbiotLoop& lc) :
        m_errno(NotStartedError),
        m_topicid({MQTTSN_TOPIC_TYPE_NORMAL,invalidTopicId}),
        m_currentTopic(),
        //client(NbiotCoreApp::getInstance().getNetworkInstance(), lc, m_topicid, clientCommandTimeout),
        client(lc, m_topicid, clientCommandTimeout),
        m_topicRegistry(new NbiotTopicRegistry()),
        initialized(false),
        m_hasSubscription(false),
        authLogin(),
        authPasswd(),
        authentication(),
        gateway(),
        mqttPort(defaultMqttPort),
        keepAliveInterval(minKeepAlive),
        autoPollInterval(0),
        eventLoopExecInterval(oneHundredMs),
        eventLoopLock(true),
        eventLoopMs(0l),
        lastPollMs(0l),
        m_cleanSession(0),
        m_autoReconnect(false),
        m_duration(0),
        m_modemRestartOnError(0),
        m_maxTopics(defaultMaxTopics),
        m_maxResend(0),
        m_modemAttached(false),
        m_waitTimer(),
        m_loopCtrl(lc)
    {
        client.setDefaultMessageHandler(m_topicRegistry, &NbiotTopicRegistry::messageDispatcher);
    }

    /*!
     * \brief ~NbiotStmDataPool
     */
    virtual ~NbiotStmDataPool() { delete m_topicRegistry; }

    /*!
     * \brief m_errno
     */
    unsigned int m_errno;
    /*!
     * \brief m_topicid
     */
    MQTTSN_topicid m_topicid;
    /*!
     * \brief m_currentTopic
     */
    NbiotTopic m_currentTopic;
    /*!
     * \brief client
     */
    NbiotMqttSnClient client;
    /*!
     * \brief m_topicRegistry
     */
    NbiotTopicRegistry* m_topicRegistry;
    /*!
     * \brief initialized
     */
    bool initialized;
    /*!
     * \brief m_hasSubscription
     */
    bool m_hasSubscription;
    /*!
     * \brief authLogin
     */
    nbiot::string authLogin;
    /*!
     * \brief authPasswd
     */
    nbiot::string authPasswd;
    /*!
     * \brief authentication
     */
    nbiot::string authentication;
    /*!
     * \brief gateway
     */
    nbiot::string gateway;
    /*!
     * \brief mqttPort
     */
    unsigned short mqttPort;
    /*!
     * \brief keepAliveInterval
     */
    unsigned int keepAliveInterval;
    /*!
     * \brief autoPollInterval
     */
    unsigned int autoPollInterval;
    /*!
     * \brief eventLoopExecInterval
     */
    unsigned long eventLoopExecInterval;
    /*!
     * \brief eventLoopLock
     */
    bool eventLoopLock;
    /*!
     * \brief eventLoopMs
     */
    long long eventLoopMs;
    /*!
     * \brief lastPollMs
     */
    long long lastPollMs;
    /*!
     * \brief m_cleanSession
     */
    unsigned char m_cleanSession;
    /*!
     * \brief m_autoReconnect
     */
    bool m_autoReconnect;
    /*!
     * \brief m_duration
     */
    unsigned short m_duration;
    /*!
     * \brief m_modemRestartOnError
     */
    unsigned short m_modemRestartOnError;
    /*!
     * \brief m_maxTopics
     */
    unsigned short m_maxTopics;
    /*!
     * \brief m_maxResend
     */
    unsigned short m_maxResend;
    /*!
     * \brief m_modemAttached
     */
    bool m_modemAttached;
    /*!
     * \brief m_waitTimer
     */
    nbiot::Timer m_waitTimer;
    /*!
     * \brief m_loopCtrl
     */
    NbiotLoop& m_loopCtrl;

    /*!
     * \brief oneSecondMs
     */
    static const unsigned int oneSecondMs = 1000;
    /*!
     * \brief oneHundredMs
     */
    static const unsigned int oneHundredMs = 100;
    /*!
     * \brief invalidTopicId
     */
    static const unsigned short invalidTopicId = 0xFFFF;

private:
    NbiotStmDataPool();
    NbiotStmDataPool(NbiotStmDataPool&);
    NbiotStmDataPool& operator = (NbiotStmDataPool&);

    static const unsigned int minKeepAlive = 60;
    static const unsigned int clientCommandTimeout = 15000;
    static const unsigned short defaultMqttPort = 1883;
    static const unsigned short defaultMaxTopics = 20;
};

} // namespace nbiot

#endif // NBIOTSTMDATAPOOL_H

