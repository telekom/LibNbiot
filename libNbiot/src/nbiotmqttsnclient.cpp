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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ========================================================================
*/

#include "nbiotmqttsnclient.h"

NbiotMqttSnClient::NbiotMqttSnClient(nbiot::NbiotLoop& lc, MQTTSN_topicid& topicName, unsigned int command_timeout_ms) : 
        MQTTSN::Client<NbiotMqttSnClient, INetwork, nbiot::Timer, 200, 5>(NbiotCoreApp::getInstance().getNetworkInstance(), topicName, command_timeout_ms),
        m_loopController(lc),
        m_conLoopClient(LI_Connect),
        m_pubLoopClient(LI_Publish),
        m_regLoopClient(LI_Register),
        m_subLoopClient(LI_Subscribe),
        m_unsubLoopClient(LI_Unsubscribe),
        m_yieldLoopClient(LI_Yield),
        m_disLoopClient(LI_Disconnect),
        m_pubRetryTimeout(0),
        m_pubRetryCount(0),
        m_subRetryTimeout(0),
        m_subRetryCount(0),
        m_maxRetry(0),
        yieldRunning(false)
{
    m_conLoopClient.setLoopStartHandler(this, &NbiotMqttSnClient::startConLoop);
    m_conLoopClient.setLoopStepHandler(this, &NbiotMqttSnClient::doConLoop);
    m_conLoopClient.setLoopStopHandler(this, &NbiotMqttSnClient::finishConLoop);

    m_pubLoopClient.setLoopStartHandler(this, &NbiotMqttSnClient::startPubLoop);
    m_pubLoopClient.setLoopStepHandler(this, &NbiotMqttSnClient::doPubLoop);
    m_pubLoopClient.setLoopStopHandler(this, &NbiotMqttSnClient::finishPubLoop);

    m_regLoopClient.setLoopStartHandler(this, &NbiotMqttSnClient::startRegLoop);
    m_regLoopClient.setLoopStepHandler(this, &NbiotMqttSnClient::doRegLoop);
    m_regLoopClient.setLoopStopHandler(this, &NbiotMqttSnClient::finishRegLoop);

    m_subLoopClient.setLoopStartHandler(this, &NbiotMqttSnClient::startSubLoop);
    m_subLoopClient.setLoopStepHandler(this, &NbiotMqttSnClient::doSubLoop);
    m_subLoopClient.setLoopStopHandler(this, &NbiotMqttSnClient::finishSubLoop);

    m_unsubLoopClient.setLoopStartHandler(this, &NbiotMqttSnClient::startUnsubLoop);
    m_unsubLoopClient.setLoopStepHandler(this, &NbiotMqttSnClient::doUnsubLoop);
    m_unsubLoopClient.setLoopStopHandler(this, &NbiotMqttSnClient::finishUnsubLoop);

    m_yieldLoopClient.setLoopStartHandler(this, &NbiotMqttSnClient::startYieldLoop);
    m_yieldLoopClient.setLoopStepHandler(this, &NbiotMqttSnClient::doYieldLoop);
    m_yieldLoopClient.setLoopStopHandler(this, &NbiotMqttSnClient::finishYieldLoop);

    m_disLoopClient.setLoopStartHandler(this, &NbiotMqttSnClient::startDisLoop);
    m_disLoopClient.setLoopStepHandler(this, &NbiotMqttSnClient::doDisLoop);
    m_disLoopClient.setLoopStopHandler(this, &NbiotMqttSnClient::finishDisLoop);
}


void NbiotMqttSnClient::notifyPingResponse()
{
    if(pingRespNotifyHandler.attached())
    {
        pingRespNotifyHandler(0);
    }
}
 

void NbiotMqttSnClient::notifyDisconnect()
{
    if(disconnectNotifyHandler.attached())
    {
        disconnectNotifyHandler(0);
    }
}

unsigned char NbiotMqttSnClient::notifyRegister(unsigned short topicid, MQTTSNString topicName)
{
    unsigned char rc = 0;
    if(registerNotifyHandler.attached())
      {
	  nbiot::NbiotTopic topic = nbiot::NbiotTopic(topicid);
	  topic.topicName = nbiot::string(topicName.lenstring.data, topicName.lenstring.len);
	  rc = registerNotifyHandler(topic);
      }
    return rc;
}


void NbiotMqttSnClient::notifyPublish(unsigned short topicid, unsigned char returncode)
{
    // send topic-id and returncode
    if(pubackNotifyHandler.attached())
    {
        nbiot::NbiotTopic nbTopic;
	nbTopic.id = topicid;
	nbTopic.returnCode = returncode;
	pubackNotifyHandler(nbTopic);
    }
}


bool NbiotMqttSnClient::loopWait(int breakValue, int interval)
{
    bool ret = true;

    if(0 == interval)
    {
        ret = false;
    }
    else
    {
        nbiot::Timer timer(interval);

        if(breakValue == cycle(timer))
        {
            ret = false;
        }
    }
    return ret;
}

int NbiotMqttSnClient::registerTopic(MQTTSN_topicid& topicName)
{
    int rc = MQTTSN::FAILURE;
    int len = 0;
    MQTTSNString topicstr;

    topicstr.cstring = topicName.data.long_.name;
    topicstr.lenstring.len = topicName.data.long_.len;

    unsigned short msgid = packetid.getNext();

    if(0 < (len = MQTTSNSerialize_register(sendbuf, getMaxPacketSize(), 0, msgid, &topicstr)))
    {
        m_topicName = topicName;
        m_regLoopClient.setValue(len);
        m_regLoopClient.getTimer().setTime(command_timeout_ms);
        m_loopController.registerLoopClient(&m_regLoopClient);
        rc = MQTTSN::SUCCESS;
    }
    return rc;
}


bool NbiotMqttSnClient::startYieldLoop(int& loopTime)
{
#if defined(DEBUG_MQTT)
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("MQTT::yield start %d\r\n", loopTime);
#endif
    m_yieldLoopClient.getTimer().start(loopTime);
    yieldRunning = true;
    return true;
}


bool NbiotMqttSnClient::doYieldLoop(int& loopTime)
{
    bool ret = true;

    loopTime = static_cast<int>(m_yieldLoopClient.getTimer().remaining());

    if(yieldRunning)
    {
        int interval = (loopInterval < loopTime)?loopInterval:loopTime;
        ret = loopWait(MQTTSN::FAILURE, interval);
    }
    else
    {
        ret = false;
    }
#if defined(DEBUG_MQTT)
    if(!yieldRunning)
    {
#ifdef DEBUG_COLOR
        debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
        debugPrintf("MQTT::yield stopped at %d\r\n", loopTime);
    }
#endif
    return ret;
}


bool NbiotMqttSnClient::finishYieldLoop(int& loopTime)
{
    bool ret = true;

    if(0 != loopTime)
    {
        ret = false;
    }

#if defined(DEBUG_MQTT)
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("MQTT::yield end %d\r\n", loopTime);
#endif
    yieldRunning = false;
    return ret;
}


bool NbiotMqttSnClient::startConLoop(int& len)
{
    bool ret = false;

    nbiot::Timer timer = nbiot::Timer(m_conLoopClient.getTimer().getTime());
    if (MQTTSN::SUCCESS == sendPacket(len, timer)) // send the connect packet
    {
        if (this->duration > 0)
            last_received.countdown(this->duration);

        m_conLoopClient.getTimer().start(timer.remaining());
        ret = true;
    }

    return ret;
}


void NbiotMqttSnClient::loopSubscribe(unsigned short id, unsigned long timer_left)
{
    m_subRetryTimeout = timer_left;
    m_subRetryCount = m_maxRetry;
    m_subLoopClient.setValue(id);
    m_subLoopClient.getTimer().setTime(timer_left);
    m_loopController.registerLoopClient(&m_subLoopClient);
}


void NbiotMqttSnClient::stopYield()
{
    yieldRunning = false;
}


void NbiotMqttSnClient::loopYield(unsigned long timeout_ms)
{
    m_yieldLoopClient.setValue(static_cast<int>(timeout_ms));
    m_loopController.registerLoopClient(&m_yieldLoopClient);
}


void NbiotMqttSnClient::loopPublish(unsigned short id, unsigned long timer_left)
{
    m_pubRetryTimeout = timer_left;
    m_pubRetryCount = m_maxRetry;
    m_pubLoopClient.getTimer().setTime(timer_left);
    m_pubLoopClient.setValue(id);
    m_loopController.registerLoopClient(&m_pubLoopClient);
}


void NbiotMqttSnClient::loopRegister(int length)
{
    m_regLoopClient.setValue(length);
    m_regLoopClient.getTimer().setTime(command_timeout_ms);
    m_loopController.registerLoopClient(&m_regLoopClient);
}


void NbiotMqttSnClient::loopDisconnect(int length)
{
    m_disLoopClient.setValue(length);
    m_disLoopClient.getTimer().setTime(command_timeout_ms);
    m_loopController.registerLoopClient(&m_disLoopClient);
}


void NbiotMqttSnClient::loopUnsubscribe(int length)
{
    m_unsubLoopClient.setValue(length);
    m_unsubLoopClient.getTimer().setTime(command_timeout_ms);
    m_loopController.registerLoopClient(&m_unsubLoopClient);
}


void NbiotMqttSnClient::loopConnect(int length)
{
    m_conLoopClient.setValue(length);
    m_conLoopClient.getTimer().setTime(command_timeout_ms);
    m_loopController.registerLoopClient(&m_conLoopClient);
}


bool NbiotMqttSnClient::doConLoop(int& loopTime)
{
    bool ret = true;

    loopTime = static_cast<int>(m_conLoopClient.getTimer().remaining());
    int interval = (loopInterval < loopTime)?loopInterval:loopTime;
    ret = loopWait(MQTTSN_CONNACK, interval);

    return ret;
}


bool NbiotMqttSnClient::finishConLoop(int& loopTime)
{
    bool ret = true;

    if(0 == loopTime)
    {
        ret = false;
    }
    else
    {
        int connack_rc = invalidRC;
        if (MQTTSNDeserialize_connack(&connack_rc, readbuf, getMaxPacketSize()) == 1)
        {
            if((MQTTSN::SUCCESS != connack_rc))
            {
                ret = false;
            }
        }
        else
        {
            ret = false;
        }
    }

    if (cleansession==0 && inflightMsgid>0)
    {
        memcpy(sendbuf, pubbuf, getMaxPacketSize());
        nbiot::Timer connect_timer = nbiot::Timer(loopTime);
        if(MQTTSN::SUCCESS != publish(inflightLen, connect_timer, inflightQoS))
            ret = false;
    }

    if(ret)
    {
        isconnected = true;
        pmPingResp_outstanding = false;
#if defined(DEBUG_MQTT)
#ifdef DEBUG_COLOR
        debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
        debugPrintf("MQTTSN::connected\r\n");
#endif
    }
    return ret;
}


bool NbiotMqttSnClient::startRegLoop(int& len)
{
    bool ret = false;

    nbiot::Timer timer = nbiot::Timer(m_regLoopClient.getTimer().getTime());
    if (MQTTSN::SUCCESS == sendPacket(len, timer)) // send the register packet
    {
        m_regLoopClient.getTimer().start(timer.remaining());
        ret = true;
    }

    return ret;
}


bool NbiotMqttSnClient::doRegLoop(int& loopTime)
{
    bool ret = true;

    loopTime = static_cast<int>(m_regLoopClient.getTimer().remaining());
    int interval = (loopInterval < loopTime)?loopInterval:loopTime;
    ret = loopWait(MQTTSN_REGACK, interval);

    return ret;
}


bool NbiotMqttSnClient::finishRegLoop(int& loopTime)
{
    bool ret = true;

    if(0 == loopTime)
    {
        ret = false;
    }
    else
    {
        unsigned short topicid;
        unsigned short submsgid;
        unsigned char returncode;
        if (0 < MQTTSNDeserialize_regack(&topicid, &submsgid, &returncode, readbuf, getMaxPacketSize()))
        {
            if((MQTTSN::SUCCESS == returncode))
            {
#if defined(DEBUG_MQTT)
#ifdef DEBUG_COLOR
                debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
                debugPrintf("MQTTSN::reg(%s)=%d\r\n", m_topicName.data.long_.name, topicid);
#endif
                m_topicName.data.id = topicid;
            }
            else
            {
                ret = false;
            }
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}

bool NbiotMqttSnClient::sendSubPacket(int packetId, unsigned char dup, unsigned long timeout)
{
    bool ret = false;
    int len = 0;
    unsigned short id = static_cast<unsigned short>(packetId);

    len = MQTTSNSerialize_subscribe(sendbuf, getMaxPacketSize(), dup, m_qos, id, &m_topicName);
    if (0 < len)
    {
        nbiot::Timer timer = nbiot::Timer(timeout);
        if (MQTTSN::SUCCESS == sendPacket(len, timer)) // send the subscribe packet
        {
            ret = true;
        }
    }
    return ret;
}

bool NbiotMqttSnClient::startSubLoop(int& packetId)
{
    nbiot::Timer timer = nbiot::Timer(m_subLoopClient.getTimer().getTime());
    bool ret = sendSubPacket(packetId, 0, timer.remaining());
    if(ret)
    {
        m_subLoopClient.getTimer().start(timer.remaining());
    }
    return ret;
}


bool NbiotMqttSnClient::doSubLoop(int& loopTime)
{
    bool ret = true;

    if((0 == m_subLoopClient.getTimer().remaining()) && (0 < m_subRetryCount))
    {
        m_subRetryCount--;
        int packetId = m_subLoopClient.getValue();
        nbiot::Timer timer = nbiot::Timer(m_subRetryTimeout);
        if(sendSubPacket(packetId, 1, timer.remaining())) // re-transmit the packet and reset the timer until the counter is zero
        {
            m_subLoopClient.getTimer().start(timer.remaining());
        }
    }
    loopTime = static_cast<int>(m_subLoopClient.getTimer().remaining());
    int interval = (loopInterval < loopTime)?loopInterval:loopTime;
    ret = loopWait(MQTTSN_SUBACK, interval);

    return ret;
}


bool NbiotMqttSnClient::finishSubLoop(int& loopTime)
{
    bool ret = true;

    if(0 == loopTime)
    {
        ret = false;
    }
    else
    {
        int grantedQoS = -1;
        unsigned short mypacketid;
        unsigned char retcode;
        if (MQTTSNDeserialize_suback(&grantedQoS, &m_topicName.data.id, &mypacketid, &retcode, readbuf, getMaxPacketSize()) == 1)
        {
            if(MQTTSN_RC_ACCEPTED == retcode)
            {
                if(!defaultMessageHandler.attached())
                {
                    ret = false;
                }
            }
            else
            {
                ret = false;
            }
        }
        else
        {
            ret = false;
        }
    }
#if defined(DEBUG_MQTT)
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("MQTTSN::subscribe(%d)=%s\r\n", m_topicName.data.id, ret?"ok":"fail");
#endif
    return ret;
}

bool NbiotMqttSnClient::sendPubPacket(int packetId, unsigned char dup, unsigned long timeout)
{
    bool ret = false;
    int len = 0;
    unsigned short id = static_cast<unsigned short>(packetId);

    len = MQTTSNSerialize_publish(sendbuf, getMaxPacketSize(), dup, m_qos, m_retained, id,
              m_topicName, (unsigned char*)m_payload, m_payloadlen);
    if (0 < len)
    {

        memcpy(pubbuf, sendbuf, len);
        inflightLen = len;
        inflightMsgid = id;
        inflightQoS = m_qos;


        nbiot::Timer timer = nbiot::Timer(timeout);
        if (MQTTSN::SUCCESS == sendPacket(len, timer)) // send the publish packet
        {
            ret = true;
        }
    }
    return ret;
}

bool NbiotMqttSnClient::startPubLoop(int& packetId)
{
    nbiot::Timer timer = nbiot::Timer(m_pubLoopClient.getTimer().getTime());
    bool ret = sendPubPacket(packetId, 0, timer.remaining());
    if(ret)
    {
        m_pubLoopClient.getTimer().start(timer.remaining());
    }
    return ret;
}


bool NbiotMqttSnClient::doPubLoop(int& loopTime)
{
    bool ret = true;

    if(QOS0 == static_cast<QoS>(m_qos))
    {
        ret = false;
    }
    else
    {
        if((0 == m_pubLoopClient.getTimer().remaining()) && (0 < m_pubRetryCount))
        {
            m_pubRetryCount--;
            int packetId = m_pubLoopClient.getValue();
            nbiot::Timer timer = nbiot::Timer(m_pubRetryTimeout);
            if(sendPubPacket(packetId, 1, timer.remaining())) // re-transmit the packet and reset the timer until the counter is zero
            {
                m_pubLoopClient.getTimer().start(timer.remaining());
            }
        }
        loopTime = static_cast<int>(m_pubLoopClient.getTimer().remaining());
        int interval = (loopInterval < loopTime)?loopInterval:loopTime;
        ret = loopWait(MQTTSN_PUBACK, interval);
    }

    return ret;
}


bool NbiotMqttSnClient::finishPubLoop(int& loopTime)
{
    bool ret = true;

    if((QOS0 != static_cast<QoS>(m_qos)))
    {
        if(0 == loopTime)
        {
            ret = false;
        }
        else
        {
            unsigned short mypacketid;
            unsigned short topicid;
            unsigned char returncode;
            loopTime = MQTTSN::SUCCESS;

            if (1 != MQTTSNDeserialize_puback(&topicid, &mypacketid, &returncode, readbuf, getMaxPacketSize()))
            {
                ret = false;
            }
            else if (inflightMsgid == mypacketid)
            {
                inflightMsgid = 0;
                if(MQTTSN::SUCCESS != static_cast<int>(returncode))
                {
                    loopTime = static_cast<int>(returncode);
                    ret = false;
                }
            }
        }
    }
    return ret;
}



bool NbiotMqttSnClient::startUnsubLoop(int& len)
{
    bool ret = false;

    nbiot::Timer timer = nbiot::Timer(m_unsubLoopClient.getTimer().getTime());
    if (MQTTSN::SUCCESS == sendPacket(len, timer)) // send the register packet
    {
        m_unsubLoopClient.getTimer().start(timer.remaining());
        ret = true;
    }

    return ret;
}


bool NbiotMqttSnClient::doUnsubLoop(int& loopTime)
{
    bool ret = true;

    loopTime = static_cast<int>(m_unsubLoopClient.getTimer().remaining());
    int interval = (loopInterval < loopTime)?loopInterval:loopTime;
    ret = loopWait(MQTTSN_UNSUBACK, interval);

    return ret;
}


bool NbiotMqttSnClient::finishUnsubLoop(int& loopTime)
{
    bool ret = true;

    if(0 == loopTime)
    {
        ret = false;
    }
    else
    {
        unsigned short mypacketid;  // should be the same as the packetid above
        if (MQTTSNDeserialize_unsuback(&mypacketid, readbuf, getMaxPacketSize()) != 1)
        {
            ret = false;
        }
    }
#if defined(DEBUG_MQTT)
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("MQTTSN::unsubscribe(%d)=%s\r\n", m_topicName.data.id, ret?"ok":"fail");
#endif
    return ret;
}


int NbiotMqttSnClient::pmPing(char *cID)
{
    int rc = MQTTSN::FAILURE;
    if (!pmPingResp_outstanding)
    {
        MQTTSNString clientid = MQTTSNString_initializer;
        clientid.cstring = cID;
        nbiot::Timer timer = nbiot::Timer(1000);
        int len = MQTTSNSerialize_pingreq(sendbuf, getMaxPacketSize(), clientid);
        if (len > 0 && (rc = sendPacket(len, timer)) == MQTTSN::SUCCESS) // send the ping packet
            pmPingResp_outstanding = true;
    }
    return rc;
}


bool NbiotMqttSnClient::startDisLoop(int& len)
{
    bool ret = false;

    nbiot::Timer timer = nbiot::Timer(m_disLoopClient.getTimer().getTime());
    if (MQTTSN::SUCCESS == sendPacket(len, timer)) // send the disconnect packet
    {
        m_disLoopClient.getTimer().start(timer.remaining());
        ret = true;
    }

    return ret;
}


bool NbiotMqttSnClient::doDisLoop(int& loopTime)
{
    bool ret = true;

    loopTime = static_cast<int>(m_disLoopClient.getTimer().remaining());
    int interval = (loopInterval < loopTime)?loopInterval:loopTime;
    ret = loopWait(MQTTSN_DISCONNECT, interval);

    return ret;
}


bool NbiotMqttSnClient::finishDisLoop(int& loopTime)
{
    bool ret = true;

    isconnected = false;
    pmPingResp_outstanding = false;

#if defined(DEBUG_MQTT)
#ifdef DEBUG_COLOR
    debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
    debugPrintf("MQTTSN::disconnect %s\r\n", (0 == loopTime)?"timeout":"ok");
#endif
    return ret;
}
