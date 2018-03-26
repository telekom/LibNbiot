/* ========================================================================
 * LibNbiot: nbiotmqtt.cpp
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

#include <eventdispatcher.h>

#include <nbiottime.h>
#include <nbiotdebug.h>
#include <atcommands.h>
#include <libnbiotcore.h>

#include "nbiotmqtt.h"

using namespace nbiot;

NbiotMQTT::NbiotMQTT() :
    m_loopController(),
    m_dataPool(m_loopController),
    m_stm(m_dataPool),
    m_notifyHandler(nullptr),
    m_evLoopRc(LC_Idle),
    m_pollInterval(oneSecondMs)
{
    m_dataPool.client.setPubackNotifyHandler(this, &NbiotMQTT::pubackNotify);
    m_dataPool.client.setRegisterNotifyHandler(this, &NbiotMQTT::registerNotify);
    m_dataPool.client.setDisconnectNotifyHandler(this, &NbiotMQTT::disconnectNotify);
    m_dataPool.client.setPingRespNotifyHandler(this, &NbiotMQTT::pingRespNotify);
}

NbiotMQTT::~NbiotMQTT()
{
}



NbiotResult NbiotMQTT::eventLoop(NbiotEventMode mode)
{
    if((0 < m_dataPool.eventLoopExecInterval) && (InvalidState != getCurrentStateId()))
    {
        if(!m_dataPool.eventLoopLock)
        {
            long long millis = getMillis();
            if(millis >= m_dataPool.eventLoopMs)
            {
                m_dataPool.eventLoopMs = millis + m_dataPool.eventLoopExecInterval;

                // lock multiple calls of a time consuming callback (more than eventLoopExecInterval)
                m_dataPool.eventLoopLock = true;

                /* -------------------------------------------------------------------------------------------- */
                // sleepmode handling:
                if(EventModeAsleep == mode)
                {
                    if(ConnectedSleepState == getCurrentStateId())
                    {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
                        debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
                        debugPrintf("eventLoop (sleep): handle wakeup and ping\r\n");
#endif
                        sendEvent(NbiotStm::ConnectedAwakeEvent);
                        m_dataPool.client.pmPing(m_dataPool.imsi.getData());
                        m_dataPool.client.yield(/*tenSecondsMs*/); // since default is 15 sec. use default
                        m_dataPool.lastPollMs = getMillis();
                    }
#ifdef DEBUG_MQTT
                    else
                    {
#ifdef DEBUG_COLOR
                        debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
                        debugPrintf("eventLoop (sleep): not in sleep state\r\n");
                    }
#endif
                }
                /* -------------------------------------------------------------------------------------------- */

                /* -------------------------------------------------------------------------------------------- */
                // hooked loop:
                NbiotResult loopResult = m_loopController.nbiotLoop();
                if(LC_Fail == loopResult)
                {
                    // may have been a timeout while waiting for DISCONNECT from GW
                    // nevertheless continue disconnect process
                    if(LI_Disconnect == m_loopController.getLoopId())
                    {
                        m_stm.finishDisconnect();
                    }
                    else if(LI_Publish == m_loopController.getLoopId())
                    {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
                        debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
                        debugPrintf("error while publishing value\r\n");
#endif
                        // PUBACK notification retcode other then 0 may be in loop-value
                        m_dataPool.m_currentTopic.returnCode = static_cast<enum MQTTSN_returnCodes>(m_loopController.getLoopValue());
                        pubackNotify(m_dataPool.m_currentTopic);

                        if (m_dataPool.m_currentTopic.returnCode!=RC_REJECTED_INVALID_TOPIC_ID) {
                            /// \todo checkout if we really need to disconnect or if we can recover from here
                            m_dataPool.m_errno = ConnectionError;
                            postEvent(NbiotStm::DisconnectEvent); // disconnect: need to be post event!
                            // otherwise the (new) disconnect-loop-client
                            // would be deleted from the client-list in the next step
                        }
                    }

                    // clear loop controller by registering nullptr:
                    // - clear the client-list
                    // - set loopValue = 0
                    // - set loopId = LI_Unknown
                    m_loopController.registerLoopClient(nullptr);
                }
                NbiotLoopId loopId = m_loopController.getLoopId();

                // handling of topic-registry entries after successful register- or subscribe-loop
                if((LC_Idle == loopResult) && (LI_Unknown != loopId))
                {
                    m_dataPool.m_currentTopic.id = m_dataPool.m_topicid.data.id;
                    if(m_dataPool.m_currentTopic.valid())
                    {
                        if(LI_Register == loopId)
                        {
                            m_dataPool.m_topicRegistry->attachWildcardMessageHandler(m_dataPool.m_currentTopic);
                            m_dataPool.m_currentTopic.setReg(NbiotTopic::REG_PUB);
                        }
                        else if(LI_Subscribe == loopId)
                        {
                            m_dataPool.m_currentTopic.setReg(NbiotTopic::REG_SUB);
                        }

                        if((LI_Register == loopId) || (LI_Subscribe == loopId))
                        {
                            m_dataPool.m_topicRegistry->insertTopic(m_dataPool.m_currentTopic);
                        }

                        if(LI_Subscribe == loopId)
                        {
                            m_dataPool.m_topicRegistry->updateWildcardMessageHandler(m_dataPool.m_currentTopic);
                            m_dataPool.m_hasSubscription = true;
                        }

                        if(LI_Publish == loopId)
                        {
                            // PUBACK notification
                            m_dataPool.m_currentTopic.returnCode = RC_ACCEPTED;
                            pubackNotify(m_dataPool.m_currentTopic);
                        }
                    }
                    if((LI_Connect == loopId) && m_dataPool.client.isConnected())
                    {
                        postEvent(NbiotStm::ConnectedEvent);
                    }
                    // continue disconnect process
                    if(LI_Disconnect == loopId)
                    {
                        m_stm.finishDisconnect();
                    }
                    // requires one more call to m_loopController.nbiotLoop() during the next cycle
                    // in the following cycle the next loop client will be loaded
                    // or the client/result will be finaly LI_Unknown and LC_Idle
                    loopResult = LC_Pending;
                }
                /* -------------------------------------------------------------------------------------------- */

                // only if no client loop steps pending:

                /* -------------------------------------------------------------------------------------------- */
                // normal event dispatching:
                if(LC_Pending != loopResult)
                {
                    m_stm.eventDispatcher()->dispatch();
                    if((0 < m_dataPool.autoPollInterval) && (m_dataPool.client.isConnected()))
                    {
                        // some time passed after dispacth()
                        millis = getMillis();
                        // maintain keep alive time
                        if(m_dataPool.autoPollInterval < ((millis - m_dataPool.lastPollMs) / NbiotStmDataPool::oneSecondMs))
                        {
                            m_dataPool.client.yield(halfSecondMs);
                            m_dataPool.lastPollMs = millis;
                        }
                    }
                    m_evLoopRc = LC_Idle;
                }
                else
                {
                    m_evLoopRc = LC_Pending;
                }
                /* -------------------------------------------------------------------------------------------- */

                // unlock event loop
                m_dataPool.eventLoopLock = false;
            }
#ifdef DEBUG_MQTT
            else
            {
                if(EventModeAsleep == mode)
                {
#ifdef DEBUG_COLOR
                    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
                    debugPrintf("eventLoop (sleep): not now\r\n");
                }
            }
#endif
        }
#ifdef DEBUG_MQTT
        else
        {
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
            debugPrintf("eventLoop LOCKED\r\n");
        }
#endif
        // ------------------------------------------------------
        // check if LC is realy released befor sending LC_Idle
        //
        if((LC_Idle == m_evLoopRc) && m_loopController.isBusy())
        {
            m_evLoopRc = LC_Pending;
        }
        //
        // ------------------------------------------------------

    }
    return m_evLoopRc;
}

int NbiotMQTT::connect(unsigned char cleanSession)
{
    int ret = InitializeError;
    if(m_dataPool.initialized)
    {
        if(!isConnected() && !isSleeping())
        {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
            debugPrintf("NbiotMQTT::connect: send ConnectEvent\r\n");
#endif
            m_dataPool.m_cleanSession = cleanSession;
            sendEvent(NbiotStm::ConnectEvent);
        }
        else
        {
            if(isSleeping())
            {
                sendEvent(NbiotStm::WakeupEvent);
            }
        }
        ret = m_dataPool.m_errno;
    }
    return ret;
}

int NbiotMQTT::publish(const char *topic, const char* data, size_t len, QoS qos)
{
    int ret = InitializeError;
    if(m_dataPool.initialized)
    {
        ret = ConnectionError;
        if(isSleeping())
        {
            sendEvent(NbiotStm::WakeupEvent);
        }
        if(isConnected())
        {
            ret = LoopCtrlBusyError;
            if(!m_loopController.isBusy())
            {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
                debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
                debugPrintf("send value: %s\r\n", data);
#endif
                m_dataPool.m_errno = Success;
                MQTTSN::Message message;
                message.qos = static_cast<MQTTSN::QoS>(qos);
                message.retained = false;
                message.dup = false;
                message.payload = (void*)data;
                message.payloadlen = len;
                int rc = MQTTSN::SUCCESS;
                m_dataPool.m_topicid = {MQTTSN_TOPIC_TYPE_NORMAL,NbiotStmDataPool::invalidTopicId};

                /// @TODO handle short names and predefined topic IDs

                int index = m_dataPool.m_topicRegistry->findTopic(topic);
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
                debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
                debugPrintf("topic index: %d\r\n", index);
#endif
                if(0 > index)
                {
                    if(m_dataPool.m_topicRegistry->topicCount() < m_dataPool.m_maxTopics)
                    {
                        m_dataPool.m_currentTopic = NbiotTopic(topic);
                        m_dataPool.m_currentTopic.toSNTopic(m_dataPool.m_topicid);
                        rc = m_dataPool.client.registerTopic(m_dataPool.m_topicid);
                    }
                    else
                    {
                        rc = MQTTSN::FAILURE;
                        m_dataPool.m_errno = MaxTopicsExceeded;
                    }
                }
                else
                {
                    if(!(m_dataPool.m_topicRegistry->operator [](index)).isReg(NbiotTopic::REG_PUB))
                    {
                        m_dataPool.m_topicRegistry->setTopicFlag(index, NbiotTopic::REG_PUB);
                    }
                    m_dataPool.m_currentTopic = m_dataPool.m_topicRegistry->operator [](index);
                    m_dataPool.m_currentTopic.toSNTopic(m_dataPool.m_topicid, NbiotTopic::TOPIC_ID);
                }
                if(MQTTSN::SUCCESS == rc)
                {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
                    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
                    debugPrintf("publish: TID=%04X -> %s\r\n", m_dataPool.m_topicid.data.id, data);
#endif
                    rc = m_dataPool.client.publish(m_dataPool.m_topicid, message);
                    if(0 > rc)
                    {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
                        debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
                        debugPrintf("error while sending value\r\n");
#endif
                        m_dataPool.m_errno = ConnectionError;
                        sendEvent(NbiotStm::DisconnectEvent); // disconnect
                    }
                    else
                    {
                        poll();
                    }
                }
                else
                {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
                    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
                    debugPrintf("error while sending value\r\n");
#endif
                    if(Success == m_dataPool.m_errno)
                    {
                        m_dataPool.m_errno = ConnectionError;
                    }
                    sendEvent(NbiotStm::DisconnectEvent); // disconnect
                }
                ret = m_dataPool.m_errno;
            }
        }
    }
    return ret;
}

int NbiotMQTT::subscribe(const char *topic, messageHandler mh)
{
    int rc = ConnectionError;
    if(isConnected())
    {
        rc = LoopCtrlBusyError;
        if(!m_loopController.isBusy())
        {
            rc = MaxTopicsExceeded;
            int index = m_dataPool.m_topicRegistry->findTopic(topic);
            if(-1 == index)
            {
                if(m_dataPool.m_topicRegistry->topicCount() < m_dataPool.m_maxTopics)
                {
                    m_dataPool.m_currentTopic = NbiotTopic(topic);
                    m_dataPool.m_currentTopic.toSNTopic(m_dataPool.m_topicid);
                    /// @TODO handle short names and predifined topic IDs
                    rc = m_dataPool.client.subscribe(m_dataPool.m_topicid, MQTTSN::QOS1, nullptr);
                    if (Success != rc)
                    {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
                        debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
                        debugPrintf("rc from MQTT subscribe is %d\r\n", rc);
#endif
                        rc = ConnectionError;
                    }
                    else
                    {
                        m_dataPool.m_currentTopic.mh = mh;
                    }
                }
            }
            else
            {
                NbiotTopic nbTopic = m_dataPool.m_topicRegistry->operator [](index);
                if(!nbTopic.isReg(NbiotTopic::REG_SUB))
                {
                    m_dataPool.m_currentTopic = nbTopic;
                    m_dataPool.m_currentTopic.toSNTopic(m_dataPool.m_topicid);
                    /// @TODO handle short names and predifined topic IDs
                    rc = m_dataPool.client.subscribe(m_dataPool.m_topicid, MQTTSN::QOS1, nullptr);
                    if (Success != rc)
                    {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
                        debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
                        debugPrintf("rc from MQTT subscribe is %d\r\n", rc);
#endif
                        rc = ConnectionError;
                    }
                    else
                    {
                        m_dataPool.m_currentTopic.mh = mh;
                    }
                }
                else
                {
                    m_dataPool.m_topicRegistry->attachMessageHandler(index, mh);
                    rc = Success;
                }
            }
        }
    }
    return rc;
}

int NbiotMQTT::unsubscribe(const char* topic)
{
    int rc = ConnectionError;
    if(isConnected())
    {
        if(hasSubscription(topic))
        {
            int index = m_dataPool.m_topicRegistry->findTopic(topic);
            // index does not need to be checked because hasSubscription(topic) is true
            NbiotTopic nbTopic = m_dataPool.m_topicRegistry->operator [](index);
            MQTTSN_topicid topicid;
            nbTopic.toSNTopic(topicid, NbiotTopic::TOPIC_NAME);
            rc = m_dataPool.client.unsubscribe(topicid);
            if (Success != rc)
            {
    #ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
                debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
                debugPrintf("rc from MQTT sunubscribe is %d\r\n", rc);
    #endif
                rc = ConnectionError;
            }
            else
            {
                m_dataPool.m_topicRegistry->removeTopic(index);
                if(!hasSubscription())
                {
                    m_dataPool.m_hasSubscription = false;
                }

            }
        }
    }
    return rc;
}

void NbiotMQTT::disconnect()
{
    if(isConnected() || isSleeping())
    {
        if(isSleeping())
        {
            sendEvent(NbiotStm::WakeupEvent);
        }
        sendEvent(NbiotStm::DisconnectEvent);
    }
}

void NbiotMQTT::sleep(unsigned short duration)
{
    if(isConnected() || isDisconnected())
    {
        m_dataPool.m_duration = duration;
        m_dataPool.m_autoReconnect = false;
        sendEvent(NbiotStm::SleepEvent);
    }
}

void NbiotMQTT::hibernate(unsigned short duration)
{
    if(isConnected() || isDisconnected() || isSleeping())
    {
        /// workflow: first sleep then hibernate with different durations
        /// +----------+-------+-----------+------------+-----------------+
        /// |          | sleep | hibernate | m_duration | m_autoReconnect |
        /// +----------+-------+-----------+------------+-----------------+
        /// | duration |   0   |     0     |     0      |     false       | wakeup to connected, disconnect, hibernate and no re-connect
        /// | duration |   X   |     0     |     0      |     false       | wakeup to disconnected, reconnect and cancelation of auto-reconnect by hibernate(0)
        /// | duration |   0   |     Y     |     Y      |     true        | wakeup to connected, disconnect with duration Y, hibernate and re-connect after
        /// | duration |   X   |     Y     |     Y      |     true        | wakeup to disconnected, reconnect, disconnect with duration Y, hibernate and re-connect after
        /// +----------+-------+-----------+------------+-----------------+
        if(isSleeping())
        {
            sendEvent(NbiotStm::WakeupEvent);
        }
        m_dataPool.m_duration = duration;
        if(isConnected() && (0 < duration))
        {
            m_dataPool.m_autoReconnect = true;
        }
        else
        {
            m_dataPool.m_autoReconnect = false;
        }
        sendEvent(NbiotStm::HibernateEvent);
    }
}

void NbiotMQTT::wakeup()
{
    if(isSleeping())
    {
        if(isConAwake())
        {
            sendEvent(NbiotStm::AwakeSleepEvent);
        }
        sendEvent(NbiotStm::WakeupEvent);
    }
    else if(DeepSleepState == getCurrentStateId())
    {
        sendEvent(NbiotStm::ReInitializeEvent);
    }
    if(m_dataPool.m_autoReconnect)
    {
        m_stm.doReconnect();
        m_dataPool.m_autoReconnect = false;
    }
    m_dataPool.m_duration = 0;
}

void NbiotMQTT::poll(unsigned short pollInterval)
{
    if(isConnected() || isSleeping())
    {
        if(isSleeping())
        {
            sendEvent(NbiotStm::WakeupEvent);
        }
        if(!hasSubscription())
        {
            m_dataPool.client.yield(halfSecondMs);
        }
        else
        {
            m_dataPool.client.yield((0 == pollInterval)?m_pollInterval:pollInterval);
        }
        m_dataPool.lastPollMs = getMillis();
    }
}

unsigned long NbiotMQTT::getMessageCount() const
{
    return m_dataPool.m_topicRegistry->arrivedMessages();
}

bool NbiotMQTT::hasSubscription() const
{
    return m_dataPool.m_topicRegistry->hasAnyTopic(NbiotTopic::REG_SUB);
}

bool NbiotMQTT::hasSubscription(const char* topic) const
{
    return m_dataPool.m_topicRegistry->hasTopic(topic, NbiotTopic::REG_SUB);
}

void NbiotMQTT::pubackNotify(nbiot::NbiotTopic& topic)
{
    if(m_notifyHandler)
    {
        Notification n;
        n.state = static_cast<enum StateIds>(getCurrentStateId());
        n.action = Publish;
        n.returnCode = static_cast<enum MQTTSN_returnCodes>(topic.returnCode);
        if(RC_ACCEPTED != n.returnCode)
        {
            n.errorNumber = (n.returnCode == RC_REJECTED_INVALID_TOPIC_ID) ? InvalidTopicID : ConnectionError;
        }
        else
        {
            n.errorNumber = Success;
        }
        m_notifyHandler(&n);
    }
    if(RC_REJECTED_INVALID_TOPIC_ID == topic.returnCode)
    {
        int index = m_dataPool.m_topicRegistry->findTopic(topic.id);
        if(0 <= index)
        {
            m_dataPool.m_topicRegistry->removeTopic(index);
        }
    }
}

int NbiotMQTT::registerNotify(nbiot::NbiotTopic& topic)
{
    int rc = RC_ACCEPTED;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:registerNotify [%04X]%s\r\n", topic.id, topic.topicName.c_str());
#endif
    //! @TODO apply topic flags
    int index = m_dataPool.m_topicRegistry->findTopic(topic.topicName.c_str());
    if(-1 != index) // just an update, will not exceed the topic limit
    {
        NbiotTopic nbTopic = m_dataPool.m_topicRegistry->operator [](index);
        if(nullptr == nbTopic.mh) // until now topic is only used by publish or it has no message handler attached
        {
            // try if it's a wildcard matching topic
            m_dataPool.m_topicRegistry->attachWildcardMessageHandler(topic);
        }
        // apply the update
        m_dataPool.m_topicRegistry->insertTopic(topic);
    }
    else
    {
        if(m_dataPool.m_topicRegistry->topicCount() < m_dataPool.m_maxTopics)
        {
            // must be a wildcard matching topic
            m_dataPool.m_topicRegistry->attachWildcardMessageHandler(topic);
            m_dataPool.m_topicRegistry->insertTopic(topic);
        }
        else
        {
            rc = RC_REJECTED_NOT_SUPPORTED;
            if(m_notifyHandler)
            {
                Notification n;
                n.state = static_cast<enum StateIds>(getCurrentStateId());
                n.action = RegisterTopic;
                n.returnCode = RC_REJECTED_NOT_SUPPORTED;
                n.errorNumber = MaxTopicsExceeded;
                m_notifyHandler(&n);
            }
        }
    }
    return rc;
}

void NbiotMQTT::disconnectNotify(int duration)
{
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:disconnectNotify d=%d\r\n", duration);
#endif
    uint32_t stateId = getCurrentStateId();
    bool expectedDisconnect = ((ConnectedSleepState == stateId) || (ConnectedAwakeState == stateId));
    if(m_notifyHandler)
    {
        Notification n;
        n.state = static_cast<enum StateIds>(stateId);
        n.action = Disconnect;
        n.returnCode = RC_ACCEPTED;
        n.errorNumber = (expectedDisconnect)? Success : ConnectionError;
        m_notifyHandler(&n);
    }
    if((0 >= duration) && !expectedDisconnect)
    {
        disconnect();
    }
}

void NbiotMQTT::pingRespNotify(int duration)
{
    (void)duration;
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("STM:pingResponseNotify\r\n");
#endif
    if(ConnectedAwakeState == getCurrentStateId())
    {
        sendEvent(NbiotStm::AwakeSleepEvent);
        m_dataPool.client.stopYield();
    }
}

