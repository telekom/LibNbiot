/*******************************************************************************
 * Copyright (c) 2014, 2015 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

#if !defined(MQTTSNCLIENT_H)
#define MQTTSNCLIENT_H

#include "FP.h"
#include "MQTTSNPacket.h"
#include "stdio.h"

#include "nbiotloopclient.h"
#include "nbiottopic.h"

// Data limits
#if !defined(MAX_REGISTRATIONS)
    #define MAX_REGISTRATIONS 5
#endif
#if !defined(MAX_REGISTRATION_TOPIC_NAME_LENGTH)
    #define MAX_REGISTRATION_TOPIC_NAME_LENGTH 20
#endif
#if !defined(MAX_INCOMING_QOS2_MESSAGES)
    #define MAX_INCOMING_QOS2_MESSAGES 10
#endif

#if !defined(MQTTSNCLIENT_QOS1)
    #define MQTTSNCLIENT_QOS1 1
#endif
#if !defined(MQTTSNCLIENT_QOS2)
    #define MQTTSNCLIENT_QOS2 0
#endif

#define MQTTCLIENT_QOS1 1

namespace MQTTSN
{


enum QoS { QOS0, QOS1, QOS2 };

// all failure return codes must be negative
enum returnCode { MAX_SUBSCRIPTIONS_EXCEEDED = -3, BUFFER_OVERFLOW = -2, FAILURE = -1, SUCCESS = 0 };


struct Message
{
    enum QoS qos;
    bool retained;
    bool dup;
    unsigned short id;
    void *payload;
    size_t payloadlen;
};


struct MessageData
{
    MessageData(MQTTSN_topicid &aTopic, struct Message &aMessage)  : message(aMessage), topic(aTopic)
    { }

    struct Message &message;
    MQTTSN_topicid &topic;
};


class PacketId
{
public:
    PacketId() : next(0)
    {
    }

    int getNext()
    {
        if(next == MAX_PACKET_ID)
            next = 0;
        return /*next = (next == MAX_PACKET_ID) ? 1 :*/ ++next;
    }

private:
    static const int MAX_PACKET_ID = 65535;
    int next;
};


/**
 * @class MQTTSNClient
 * @brief blocking, non-threaded MQTTSN client API
 *
 * This version of the API blocks on all method calls, until they are complete.  This means that only one
 * MQTT request can be in process at any one time.
 * @param Network a network class which supports send, receive
 * @param Timer a timer class with the methods:
 */
template<class Network, class Timer, int MAX_PACKET_SIZE = 100, int MAX_MESSAGE_HANDLERS = 5>
class Client
{

public:

    typedef void (*messageHandler)(MessageData&);

    /** Construct the client
     *  @param network - pointer to an instance of the Network class - must be connected to the endpoint
     *      before calling MQTT connect
     *  @param limits an instance of the Limit class - to alter limits as required
     */
    Client(Network& network, nbiot::NbiotLoop& lc, MQTTSN_topicid& topicName, unsigned int command_timeout_ms = 20000);

    /** Set the default message handling callback - used for any message which does not match a subscription message handler
     *  @param mh - pointer to the callback function
     */
    template<class MHC>
    void setDefaultMessageHandler(MHC* mhc, int (MHC::*messageHandler)(MessageData&))
    {
        defaultMessageHandler.attach(mhc, messageHandler);
    }

    template<class NHC>
    void setPubackNotifyHandler(NHC* nhc, void (NHC::*paNotifyHandler)(nbiot::NbiotTopic&))
    {
        pubackNotifyHandler.attach(nhc, paNotifyHandler);
    }

    template<class NHC>
    void setRegisterNotifyHandler(NHC* nhc, int (NHC::*regNotifyHandler)(nbiot::NbiotTopic&))
    {
        registerNotifyHandler.attach(nhc, regNotifyHandler);
    }

    template<class NHC>
    void setDisconnectNotifyHandler(NHC* nhc, void (NHC::*disNotifyHandler)(int))
    {
        disconnectNotifyHandler.attach(nhc, disNotifyHandler);
    }

    template<class NHC>
    void setPingRespNotifyHandler(NHC* nhc, void (NHC::*pingrespNotifyHandler)(int))
    {
        pingRespNotifyHandler.attach(nhc, pingrespNotifyHandler);
    }

    /** MQTT Connect - send an MQTT connect packet down the network and wait for a Connack
     *  The nework object must be connected to the network endpoint before calling this
     *  Default connect options are used
     *  @return success code -
     */
    int connect();
    
    /** MQTT Connect - send an MQTT connect packet down the network and wait for a Connack
     *  The nework object must be connected to the network endpoint before calling this
     *  @param options - connect options
     *  @return success code -
     */
    int connect(MQTTSNPacket_connectData& options);

    int registerTopic(MQTTSN_topicid& topicName);

    /** MQTT Publish - send an MQTT publish packet and wait for all acks to complete for all QoSs
     *  @param topic - the topic to publish to
     *  @param message - the message to send
     *  @return success code -
     */
    int publish(MQTTSN_topicid& topic, Message& message);
    
    /** MQTT Subscribe - send an MQTT subscribe packet and wait for the suback
     *  @param topicFilter - a topic pattern which can include wildcards
     *  @param qos - the MQTT QoS to subscribe at
     *  @param mh - the callback function to be invoked when a message is received for this subscription
     *  @return success code -
     */
    int subscribe(MQTTSN_topicid& topicFilter, enum QoS qos, messageHandler mh);

    /** MQTT Unsubscribe - send an MQTT unsubscribe packet and wait for the unsuback
     *  @param topicFilter - a topic pattern which can include wildcards
     *  @return success code -
     */
    int unsubscribe(MQTTSN_topicid& topicFilter);

    /** MQTT Disconnect - send an MQTT disconnect packet, and clean up any state
     *  @param duration - used for sleeping clients, 0 means no duration
     *  @return success code -
     */
    int disconnect(unsigned short duration = 0);

    /** A call to this API must be made within the keepAlive interval to keep the MQTT connection alive
     *  yield can be called if no other MQTT operation is needed.  This will also allow messages to be
     *  received.
     *  @param timeout_ms the time to wait, in milliseconds
     *  @return success code - on failure, this means the client has disconnected
     */
    int yield(unsigned long timeout_ms = 15000L);

    void stopYield()
    {
        yieldRunning = false;
    }

    int pmPing(char* cID);

    /** Is the client connected?
     *  @return flag - is the client connected or not?
     */
    bool isConnected()
    {
        return isconnected;
    }

    unsigned long getCommandTimeout() const
    {
        return command_timeout_ms;
    }

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

protected:

    int cycle(Timer& timer);
    bool loopWait(int breakValue, int interval);

private:

    int keepalive();
    int publish(int len, Timer& timer, enum QoS qos);

    int readPacket(Timer& timer);
    int sendPacket(int length, Timer& timer);
    int deliverMessage(MQTTSN_topicid& topic, Message& message);

    Network& ipstack;
    unsigned long command_timeout_ms;

    unsigned char sendbuf[MAX_PACKET_SIZE];
    unsigned char readbuf[MAX_PACKET_SIZE];

    Timer last_sent, last_received;
    unsigned short duration;
    bool ping_outstanding;
    bool pmPingResp_outstanding;
    bool cleansession;
    bool yieldRunning;

    PacketId packetid;

    FP<int, MessageData&> defaultMessageHandler;
    FP<void, nbiot::NbiotTopic&> pubackNotifyHandler;
    FP<int, nbiot::NbiotTopic&> registerNotifyHandler;
    FP<void, int> disconnectNotifyHandler;
    FP<void, int> pingRespNotifyHandler;

    nbiot::NbiotLoop& m_loopController;

    nbiot::LoopClient m_conLoopClient;
    nbiot::LoopClient m_pubLoopClient;
    nbiot::LoopClient m_regLoopClient;
    nbiot::LoopClient m_subLoopClient;
    nbiot::LoopClient m_unsubLoopClient;
    nbiot::LoopClient m_yieldLoopClient;

    MQTTSN_topicid& m_topicName;

    void* m_payload;
    size_t m_payloadlen;
    bool m_retained;
    QoS m_qos;

    bool isconnected;

    static const int loopInterval = 1000;
    static const int minSendTime = 100;
    static const int recomendedSendTime = 1000;
    static const int oneSecond = 1000;
    static const int invalidRC = 255;
    

    unsigned char pubbuf[MAX_PACKET_SIZE];  // store the last publish for sending on reconnect
    int inflightLen;
    unsigned short inflightMsgid;
    enum QoS inflightQoS;

};

}


template<class Network, class Timer, int a, int MAX_MESSAGE_HANDLERS>
MQTTSN::Client<Network, Timer, a, MAX_MESSAGE_HANDLERS>::Client(Network& network, nbiot::NbiotLoop &lc, MQTTSN_topicid& topicName, unsigned int command_timeout_ms)  :
    ipstack(network),
    packetid(),
    m_loopController(lc),
    m_conLoopClient(LI_Connect),
    m_pubLoopClient(LI_Publish),
    m_regLoopClient(LI_Register),
    m_subLoopClient(LI_Subscribe),
    m_unsubLoopClient(LI_Unsubscribe),
    m_yieldLoopClient(LI_Yield),
    m_topicName(topicName),
    m_payload(nullptr),
    m_payloadlen(0),
    m_retained(false),
    m_qos(QOS1)
{
    last_sent = Timer();
    last_received = Timer();
    ping_outstanding = false;
    this->command_timeout_ms = command_timeout_ms;
    isconnected = false;

    m_conLoopClient.setLoopStartHandler(this, &Client::startConLoop);
    m_conLoopClient.setLoopStepHandler(this, &Client::doConLoop);
    m_conLoopClient.setLoopStopHandler(this, &Client::finishConLoop);

    m_pubLoopClient.setLoopStartHandler(this, &Client::startPubLoop);
    m_pubLoopClient.setLoopStepHandler(this, &Client::doPubLoop);
    m_pubLoopClient.setLoopStopHandler(this, &Client::finishPubLoop);

    m_regLoopClient.setLoopStartHandler(this, &Client::startRegLoop);
    m_regLoopClient.setLoopStepHandler(this, &Client::doRegLoop);
    m_regLoopClient.setLoopStopHandler(this, &Client::finishRegLoop);

    m_subLoopClient.setLoopStartHandler(this, &Client::startSubLoop);
    m_subLoopClient.setLoopStepHandler(this, &Client::doSubLoop);
    m_subLoopClient.setLoopStopHandler(this, &Client::finishSubLoop);

    m_unsubLoopClient.setLoopStartHandler(this, &Client::startUnsubLoop);
    m_unsubLoopClient.setLoopStepHandler(this, &Client::doUnsubLoop);
    m_unsubLoopClient.setLoopStopHandler(this, &Client::finishUnsubLoop);

    m_yieldLoopClient.setLoopStartHandler(this, &Client::startYieldLoop);
    m_yieldLoopClient.setLoopStepHandler(this, &Client::doYieldLoop);
    m_yieldLoopClient.setLoopStopHandler(this, &Client::finishYieldLoop);

    inflightMsgid = 0;
    inflightQoS = QOS0;

}


template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
int MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::sendPacket(int length, Timer& timer)
{
    int rc = FAILURE,
        sent = 0;
#if defined(DEBUG_MQTT)
    int reason = 0;
#endif

    if(minSendTime > timer.left_ms())
    {
        timer.countdown_ms(recomendedSendTime); // one timeout-second to send the packet
    }

    while (sent < length && !timer._expired())
    {
        if(MAX_PACKET_SIZE < (length - sent))
        {
#if defined(DEBUG_MQTT)
            reason = 2;
#endif
            break;
        }
        if(ipstack.write(&sendbuf[sent], length, timer.left_ms()))
        {
            rc = length;
        }
        if (rc < 0)  // there was an error writing the data
        {
#if defined(DEBUG_MQTT)
            reason = 1;
#endif
            break;
        }
        sent += rc;
    }
    if (sent == length)
    {
        if (this->duration > 0)
            last_sent.countdown(this->duration); // record the fact that we have successfully sent the packet
        rc = SUCCESS;
    }
    else
    {
#if defined(DEBUG_MQTT)
        reason = 2;
#endif
        rc = FAILURE;
    }

#if defined(DEBUG_MQTT)
    if((FAILURE == rc) && (timer._expired()))
    {
        reason = 3;
    }
        
    char printbuf[50];
    debugPrintf("Rc %d from sending packet %s\r\n", rc, MQTTPacket_toString(printbuf, sizeof(printbuf), sendbuf, length));
    if(FAILURE == rc)
    {
        switch(reason)
        {
        case 1:
            debugPrintf("FAILURE: write error\r\n");
            break;
        case 2:
            debugPrintf("FAILURE: sent bytes != packet length\r\n");
            break;
        case 3:
            debugPrintf("FAILURE: timeout\r\n");
            break;
        case 0:
        default:
            debugPrintf("FAILURE: unknown error\r\n");
            break;
        }
    }
#endif
    return rc;
}


/**
 * If any read fails in this method, then we should disconnect from the network, as on reconnect
 * the packets can be retried.
 * @param timeout the max time to wait for the packet read to complete, in milliseconds
 * @return the MQTT packet type, or -1 if none
 */
template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
int MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::readPacket(Timer& timer)
{
    int rc = FAILURE;
    int len = 0;  // the length of the whole packet including length field 
    int lenlen = 0;
    int datalen = 0;
#if defined(DEBUG_MQTT)
    int reason = 0;
#endif


#if defined(DEBUG_MQTT)
    debugPrintf("MQTT::readPacket(%d)\r\n", timer.left_ms());
#endif
    #define MAX_NO_OF_PACKET_LENGTH_BYTES 3
    #define MIN_NO_OF_PACKET_LENGTH_BYTES 1
    #define MQTTSN_MIN_PACKET_LENGTH 2

    // 1. read the first byte
    while(0xffff < timer.left_ms())
    {
        len = ipstack.read(readbuf, MIN_NO_OF_PACKET_LENGTH_BYTES, 0xffff);
        if(MIN_NO_OF_PACKET_LENGTH_BYTES == len)
        {
            break;
        }
    }
    if(MIN_NO_OF_PACKET_LENGTH_BYTES > len)
    {
        len = ipstack.read(readbuf, MIN_NO_OF_PACKET_LENGTH_BYTES, timer.left_ms());
    }
    if (len < MIN_NO_OF_PACKET_LENGTH_BYTES)
    {
#if defined(DEBUG_MQTT)
        reason = 1;
#endif
        goto exit;
    }

    // 2. check if it's 1-byte or 3-byte packetlength
    if(1 == readbuf[0])
    {
        // read the next two bytes
        while(0xffff < timer.left_ms())
        {
            int rb = ipstack.read(readbuf + MIN_NO_OF_PACKET_LENGTH_BYTES, (MAX_NO_OF_PACKET_LENGTH_BYTES-MIN_NO_OF_PACKET_LENGTH_BYTES), 0xffff);
            if(-1 < rb)
            {
                len += rb;
            }
            if(MAX_NO_OF_PACKET_LENGTH_BYTES == len)
            {
                break;
            }
        }
        if(MAX_NO_OF_PACKET_LENGTH_BYTES > len)
        {
            len += ipstack.read(readbuf + MIN_NO_OF_PACKET_LENGTH_BYTES, (MAX_NO_OF_PACKET_LENGTH_BYTES-MIN_NO_OF_PACKET_LENGTH_BYTES), timer.left_ms());
        }
        if (len < MAX_NO_OF_PACKET_LENGTH_BYTES)
        {
#if defined(DEBUG_MQTT)
            reason = 2;
#endif
            goto exit;
        }
    }

    // 3. read the length.  This is variable in itself
    lenlen = MQTTSNPacket_decode(readbuf, len, &datalen);

    // 4. read the remaining bytes
    while(0xffff < timer.left_ms())
    {
        int rb = ipstack.read(&readbuf[len], (datalen-len), 0xffff);
        if(-1 < rb)
        {
            len += rb;
        }
        if(datalen <= len)
        {
            break;
        }
    }
    if (datalen > len)
    {
        len += ipstack.read(&readbuf[len], (datalen-len), timer.left_ms());
    }
    if (datalen != len)
    {
#if defined(DEBUG_MQTT)
        reason = 4;
#endif
        goto exit; // there was an error
    }
        
    rc = readbuf[lenlen];
    if (this->duration > 0)
        last_received.countdown(this->duration); // record the fact that we have successfully received a packet
exit:
        
#if defined(DEBUG_MQTT)
    char printbuf[50];
    debugPrintf("Rc %d:%d from receiving packet %s\r\n", rc, reason, (MQTTSN_MIN_PACKET_LENGTH <= len)?MQTTPacket_toString(printbuf, sizeof(printbuf), readbuf, len):"");
#endif
    return rc;
}

template<class Network, class Timer, int a, int MAX_MESSAGE_HANDLERS>
int MQTTSN::Client<Network, Timer, a, MAX_MESSAGE_HANDLERS>::deliverMessage(MQTTSN_topicid& topic, Message& message)
{
    int rc = FAILURE;

    if (defaultMessageHandler.attached())
    {
        MessageData md(topic, message);
        rc = defaultMessageHandler(md);
    }

    return rc;
}

template<class Network, class Timer, int a, int b>
bool MQTTSN::Client<Network, Timer, a, b>::loopWait(int breakValue, int interval)
{
    bool ret = true;

    if(0 == interval)
    {
        ret = false;
    }
    else
    {
        Timer timer(interval);

        if(breakValue == cycle(timer))
        {
            ret = false;
        }
    }
    return ret;
}

template<class Network, class Timer, int a, int b>
bool MQTTSN::Client<Network, Timer, a, b>::startYieldLoop(int& loopTime)
{
#if defined(DEBUG_MQTT)
    debugPrintf("MQTT::yield start %d\r\n", loopTime);
#endif
    m_yieldLoopClient.getTimer().start(static_cast<unsigned long>(loopTime));
    yieldRunning = true;
    return true;
}

template<class Network, class Timer, int a, int b>
bool MQTTSN::Client<Network, Timer, a, b>::doYieldLoop(int& loopTime)
{
    bool ret = true;

    loopTime = static_cast<int>(m_yieldLoopClient.getTimer().remaining());

    if(yieldRunning)
    {
        int interval = (loopInterval < loopTime)?loopInterval:loopTime;
        ret = loopWait(FAILURE, interval);
    }
    else
    {
        ret = false;
    }
#if defined(DEBUG_MQTT)
    if(!yieldRunning)
    {
        debugPrintf("MQTT::yield stopped at %d\r\n", loopTime);
    }
#endif
    return ret;
}

template<class Network, class Timer, int a, int b>
bool MQTTSN::Client<Network, Timer, a, b>::finishYieldLoop(int& loopTime)
{
    bool ret = true;

    if(0 != loopTime)
    {
        ret = false;
    }

#if defined(DEBUG_MQTT)
    debugPrintf("MQTT::yield end %d\r\n", loopTime);
#endif
    yieldRunning = false;
    return ret;
}

template<class Network, class Timer, int a, int b>
int MQTTSN::Client<Network, Timer, a, b>::yield(unsigned long timeout_ms)
{
    int rc = SUCCESS;
#if defined(DEBUG_MQTT)
    debugPrintf("MQTT::yield(%d)\r\n", timeout_ms);
#endif

    m_yieldLoopClient.setValue(static_cast<int>(timeout_ms));
    m_loopController.registerLoopClient(&m_yieldLoopClient);
    return rc;
}


template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
int MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::cycle(Timer& timer)
{
    /* get one piece of work off the wire and one pass through */

#if defined(DEBUG_MQTT)
    debugPrintf("MQTT::cycle(%d)\r\n", timer.left_ms());
#endif
    // read the socket, see what work is due
    unsigned short packet_type = readPacket(timer);

#if defined(DEBUG_MQTT)
    debugPrintf("MQTT::cycle: readPacket done @ %d\r\n", timer.left_ms());
#endif

    int len = 0;
    unsigned char rc = SUCCESS;

    switch (packet_type)
    {
        case MQTTSN_CONNACK:
        case MQTTSN_SUBACK:
        case MQTTSN_REGACK:
            break;
        case MQTTSN_PUBACK:
        {
            if(0 == inflightMsgid) // PUBACK after PUBLISH with QoS0: something went wrong
            {
                unsigned short mypacketid;
                unsigned short topicid;
                unsigned char returncode;
                if (1 != MQTTSNDeserialize_puback(&topicid, &mypacketid, &returncode, readbuf, MAX_PACKET_SIZE))
                    rc = FAILURE;
                else
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
            }
            break;
        }
        case MQTTSN_REGISTER:
        {
            unsigned short topicid, packetid;
            MQTTSNString topicName;
            rc = MQTTSN_RC_ACCEPTED;
            if (MQTTSNDeserialize_register(&topicid, &packetid, &topicName, readbuf, MAX_PACKET_SIZE) != 1)
                goto exit;
            if(registerNotifyHandler.attached())
            {
                nbiot::NbiotTopic topic = nbiot::NbiotTopic(topicid);
                topic.topicName = nbiot::string(topicName.lenstring.data, topicName.lenstring.len);
                rc = registerNotifyHandler(topic);
            }
            len = MQTTSNSerialize_regack(sendbuf, MAX_PACKET_SIZE, topicid, packetid, rc);
            if (len <= 0)
                rc = FAILURE;
            else
                rc = sendPacket(len, timer);
            break;
        }
        case MQTTSN_DISCONNECT:
        {
            int duration = 0;
            if (MQTTSNDeserialize_disconnect(&duration, readbuf, MAX_PACKET_SIZE) != 1)
            {
                rc = FAILURE;
            }
            else
            {
                isconnected = false;
                if(disconnectNotifyHandler.attached())
                {
                    disconnectNotifyHandler(0);
                }
            }
            break;
        }
        case MQTTSN_PUBLISH:
        {
            MQTTSN_topicid topicid;
            Message msg;
            if (MQTTSNDeserialize_publish((unsigned char*)&msg.dup, (int*)&msg.qos, (unsigned char*)&msg.retained, (unsigned short*)&msg.id, &topicid,
                                 (unsigned char**)&msg.payload, (int*)&msg.payloadlen, readbuf, MAX_PACKET_SIZE) != 1)
            {
                goto exit;
            }
            int retCode = deliverMessage(topicid, msg);
            if (msg.qos != QOS0)
            {
                if (msg.qos == QOS1)
                {
                    len = MQTTSNSerialize_puback(sendbuf, MAX_PACKET_SIZE, topicid.data.id, msg.id, (FAILURE == retCode)?0:retCode);
#if defined(DEBUG_MQTT)
    debugPrintf("MQTT::cycle send PUBACK\r\n");
#endif
                }
                else if (msg.qos == QOS2)
                {
                    len = MQTTSNSerialize_pubrec(sendbuf, MAX_PACKET_SIZE, msg.id);
                }
                if (len <= 0)
                {
                    rc = FAILURE;
                }
                else
                {
                    rc = sendPacket(len, timer);
                }
                if (rc == FAILURE)
                    goto exit; // there was a problem
            }
            break;
        }
        case MQTTSN_PINGRESP:
        {
            if(pmPingResp_outstanding)
            {
                if(pingRespNotifyHandler.attached())
                {
                    pingRespNotifyHandler(0);
                }
                pmPingResp_outstanding = false;
            }
            else
            {
                if(ping_outstanding)
                {
                    ping_outstanding = false;
                }
            }
            break;
        }
    }
    keepalive();
exit:
    if (rc == SUCCESS)
        rc = packet_type;
    return rc;
}


template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
int MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::keepalive()
{
    int rc = FAILURE;

    if (duration == 0)
    {
        rc = SUCCESS;
    }
    else
    {
        if (last_sent._expired() || last_received._expired())
        {
            if (!ping_outstanding)
            {
                MQTTSNString clientid = MQTTSNString_initializer;
                Timer timer = Timer(oneSecond);
                int len = MQTTSNSerialize_pingreq(sendbuf, MAX_PACKET_SIZE, clientid);
                if (len > 0 && (rc = sendPacket(len, timer)) == SUCCESS) // send the ping packet
                    ping_outstanding = true;
            }
        }
    }
    return rc;
}

template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
int MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::pmPing(char *cID)
{
    int rc = FAILURE;
    if (!pmPingResp_outstanding)
    {
        MQTTSNString clientid = MQTTSNString_initializer;
        clientid.cstring = cID;
        Timer timer = Timer(1000);
        int len = MQTTSNSerialize_pingreq(sendbuf, MAX_PACKET_SIZE, clientid);
        if (len > 0 && (rc = sendPacket(len, timer)) == SUCCESS) // send the ping packet
            pmPingResp_outstanding = true;
    }
    return rc;
}

template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
bool MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::startConLoop(int& len)
{
    bool ret = false;

    Timer timer = Timer(m_conLoopClient.getTimer().getTime());
    if (SUCCESS == sendPacket(len, timer)) // send the connect packet
    {
        if (this->duration > 0)
            last_received.countdown(this->duration);

        m_conLoopClient.getTimer().start(timer.left_ms());
        ret = true;
    }

    return ret;
}

template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
bool MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::doConLoop(int& loopTime)
{
    bool ret = true;

    loopTime = static_cast<int>(m_conLoopClient.getTimer().remaining());
    int interval = (loopInterval < loopTime)?loopInterval:loopTime;
    ret = loopWait(MQTTSN_CONNACK, interval);

    return ret;
}

template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
bool MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::finishConLoop(int& loopTime)
{
    bool ret = true;

    if(0 == loopTime)
    {
        ret = false;
    }
    else
    {
        int connack_rc = invalidRC;
        if (MQTTSNDeserialize_connack(&connack_rc, readbuf, MAX_PACKET_SIZE) == 1)
        {
            if((SUCCESS != connack_rc))
            {
                ret = false;
            }
        }
        else
            ret = false;
    }

    if (inflightMsgid > 0)
    {
        memcpy(sendbuf, pubbuf, MAX_PACKET_SIZE);
        Timer connect_timer = Timer(loopTime);
        if(SUCCESS != publish(inflightLen, connect_timer, inflightQoS))
            ret = false;
    }

    if(ret)
    {
        isconnected = true;
        pmPingResp_outstanding = false;
#if defined(DEBUG_MQTT)
debugPrintf("MQTTSN::connected\r\n");
#endif
    }
    return ret;
}

template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
int MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::connect(MQTTSNPacket_connectData& options)
{
    int rc = FAILURE;
    int len = 0;

    if (!isconnected) // don't send connect packet again if we are already connected
    {
        this->duration = options.duration;
        this->cleansession = options.cleansession;
        if (0 < (len = MQTTSNSerialize_connect(sendbuf, MAX_PACKET_SIZE, &options)))
        {
            m_conLoopClient.setValue(len);
            m_conLoopClient.getTimer().setTime(command_timeout_ms);
            m_loopController.registerLoopClient(&m_conLoopClient);
            rc = SUCCESS;
        }
    }

    return rc;
}


template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
int MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::connect()
{
    MQTTSNPacket_connectData default_options = MQTTSNPacket_connectData_initializer;
    return connect(default_options);
}

template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
bool MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::startRegLoop(int& len)
{
    bool ret = false;

    Timer timer = Timer(m_regLoopClient.getTimer().getTime());
    if (SUCCESS == sendPacket(len, timer)) // send the register packet
    {
        m_regLoopClient.getTimer().start(timer.left_ms());
        ret = true;
    }

    return ret;
}

template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
bool MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::doRegLoop(int& loopTime)
{
    bool ret = true;

    loopTime = static_cast<int>(m_regLoopClient.getTimer().remaining());
    int interval = (loopInterval < loopTime)?loopInterval:loopTime;
    ret = loopWait(MQTTSN_REGACK, interval);

    return ret;
}

template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
bool MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::finishRegLoop(int& loopTime)
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
        if (0 < MQTTSNDeserialize_regack(&topicid, &submsgid, &returncode, readbuf, MAX_PACKET_SIZE))
        {
            if((SUCCESS == returncode))
            {
#if defined(DEBUG_MQTT)
debugPrintf("MQTTSN::reg(%s)=%d\r\n", m_topicName.data.long_.name, topicid);
#endif
                m_topicName.data.id = topicid;
            }
            else
                ret = false;
        }
        else
            ret = false;
    }
    return ret;
}


template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
int MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::registerTopic(MQTTSN_topicid& topicName)
{
    int rc = FAILURE;
    int len = 0;
    MQTTSNString topicstr;

    topicstr.cstring = topicName.data.long_.name;
    topicstr.lenstring.len = topicName.data.long_.len;

    unsigned short msgid = packetid.getNext();

    if(0 < (len = MQTTSNSerialize_register(sendbuf, MAX_PACKET_SIZE, 0, msgid, &topicstr)))
    {
        m_topicName = topicName;
        m_regLoopClient.setValue(len);
        m_regLoopClient.getTimer().setTime(command_timeout_ms);
        m_loopController.registerLoopClient(&m_regLoopClient);
        rc = SUCCESS;
    }
    return rc;
}


template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
bool MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::startSubLoop(int& len)
{
    bool ret = false;

    Timer timer = Timer(m_subLoopClient.getTimer().getTime());
    if (SUCCESS == sendPacket(len, timer)) // send the register packet
    {
        m_subLoopClient.getTimer().start(timer.left_ms());
        ret = true;
    }

    return ret;
}

template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
bool MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::doSubLoop(int& loopTime)
{
    bool ret = true;

    loopTime = static_cast<int>(m_subLoopClient.getTimer().remaining());
    int interval = (loopInterval < loopTime)?loopInterval:loopTime;
    ret = loopWait(MQTTSN_SUBACK, interval);

    return ret;
}


template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
bool MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::finishSubLoop(int& loopTime)
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
        if (MQTTSNDeserialize_suback(&grantedQoS, &m_topicName.data.id, &mypacketid, &retcode, readbuf, MAX_PACKET_SIZE) == 1)
        {
            if(MQTTSN_RC_ACCEPTED == retcode)
            {
                if(!defaultMessageHandler.attached())
                {
                    ret = false;
                }
            }
            else
                ret = false;
        }
        else
            ret = false;
    }
#if defined(DEBUG_MQTT)
debugPrintf("MQTTSN::subscribe(%d)=%s\r\n", m_topicName.data.id, ret?"ok":"fail");
#endif
    return ret;
}


template<class Network, class Timer, int MAX_PACKET_SIZE, int MAX_MESSAGE_HANDLERS>
int MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, MAX_MESSAGE_HANDLERS>::subscribe(MQTTSN_topicid& topicFilter, enum QoS qos, messageHandler messageHandler)
{
    int rc = FAILURE;
    int len = 0;

    if (isconnected)
    {
        if(0 < (len = MQTTSNSerialize_subscribe(sendbuf, MAX_PACKET_SIZE, 0, qos, packetid.getNext(), &topicFilter)))
        {
            (void)messageHandler;
            m_topicName = topicFilter;
            m_subLoopClient.setValue(len);
            m_subLoopClient.getTimer().setTime(command_timeout_ms);
            m_loopController.registerLoopClient(&m_subLoopClient);
            rc = SUCCESS;
        }
    }
    return rc;
}

template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
bool MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::startUnsubLoop(int& len)
{
    bool ret = false;

    Timer timer = Timer(m_unsubLoopClient.getTimer().getTime());
    if (SUCCESS == sendPacket(len, timer)) // send the register packet
    {
        m_unsubLoopClient.getTimer().start(timer.left_ms());
        ret = true;
    }

    return ret;
}

template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
bool MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::doUnsubLoop(int& loopTime)
{
    bool ret = true;

    loopTime = static_cast<int>(m_unsubLoopClient.getTimer().remaining());
    int interval = (loopInterval < loopTime)?loopInterval:loopTime;
    ret = loopWait(MQTTSN_UNSUBACK, interval);

    return ret;
}

template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
bool MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::finishUnsubLoop(int& loopTime)
{
    bool ret = true;

    if(0 == loopTime)
    {
        ret = false;
    }
    else
    {
        unsigned short mypacketid;  // should be the same as the packetid above
        if (MQTTSNDeserialize_unsuback(&mypacketid, readbuf, MAX_PACKET_SIZE) != 1)
        {
            ret = false;
        }
    }
#if defined(DEBUG_MQTT)
debugPrintf("MQTTSN::unsubscribe(%d)=%s\r\n", m_topicName.data.id, ret?"ok":"fail");
#endif
    return ret;
}

template<class Network, class Timer, int MAX_PACKET_SIZE, int MAX_MESSAGE_HANDLERS>
int MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, MAX_MESSAGE_HANDLERS>::unsubscribe(MQTTSN_topicid& topicFilter)
{
    int rc = FAILURE;
    int len = 0;

    if (isconnected)
    {
        if ( 0 < (len = MQTTSNSerialize_unsubscribe(sendbuf, MAX_PACKET_SIZE, packetid.getNext(), &topicFilter)))
        {
            m_topicName = topicFilter;
            m_unsubLoopClient.setValue(len);
            m_unsubLoopClient.getTimer().setTime(command_timeout_ms);
            m_loopController.registerLoopClient(&m_unsubLoopClient);
            rc = SUCCESS;
        }
    }
    return rc;
}


template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
bool MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::startPubLoop(int& packetId)
{
    bool ret = false;
    int len = 0;
    unsigned short id = static_cast<unsigned short>(packetId);

    len = MQTTSNSerialize_publish(sendbuf, MAX_PACKET_SIZE, 0, m_qos, m_retained, id,
              m_topicName, (unsigned char*)m_payload, m_payloadlen);
    if (0 < len)
    {
        if (!cleansession)
        {
            memcpy(pubbuf, sendbuf, len);
            inflightLen = len;
            inflightMsgid = id;
            inflightQoS = m_qos;
        }

        Timer timer = Timer(m_pubLoopClient.getTimer().getTime());
        if (SUCCESS == sendPacket(len, timer)) // send the publish packet
        {
            m_pubLoopClient.getTimer().start(timer.left_ms());
            ret = true;
        }
    }
    return ret;
}

template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
bool MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::doPubLoop(int& loopTime)
{
    bool ret = true;

    if(QOS0 == m_qos)
    {
        ret = false;
    }
    else
    {
        loopTime = static_cast<int>(m_pubLoopClient.getTimer().remaining());
        int interval = (loopInterval < loopTime)?loopInterval:loopTime;
        ret = loopWait(MQTTSN_PUBACK, interval);
    }

    return ret;
}

template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
bool MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::finishPubLoop(int& loopTime)
{
    bool ret = true;

    if((QOS0 != m_qos))
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
            loopTime = SUCCESS;

            if (1 != MQTTSNDeserialize_puback(&topicid, &mypacketid, &returncode, readbuf, MAX_PACKET_SIZE))
            {
                ret = false;
            }
            else if (inflightMsgid == mypacketid)
            {
                inflightMsgid = 0;
                if(SUCCESS != static_cast<int>(returncode))
                {
                    loopTime = static_cast<int>(returncode);
                    ret = false;
                }
            }
        }
    }
    return ret;
}


template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
int MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::publish(int len, Timer& timer, enum QoS qos)
{
    int rc = SUCCESS;
    
    if(QOS0 == qos)
    {
        rc = sendPacket(len, timer); // send the publish packet
        // if (rc != SUCCESS) there was a problem
    }
    else if (qos == QOS1)
    {
        // not handled here
        // see
        // int publish(MQTTSN_topicid&, Message&)
    }

    return rc;
}


template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
int MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::publish(MQTTSN_topicid& topicName, Message& message)
{
    int rc = FAILURE;
    Timer timer = Timer(command_timeout_ms);
    unsigned short id = 0;  // dummy - not used for anything @QOS0

    if (isconnected)
    {
        if (QOS1 == message.qos)
        {
            id = packetid.getNext();
        }
        m_topicName = topicName;
        m_payload = message.payload;
        m_payloadlen = message.payloadlen;
        m_retained = message.retained;
        m_qos = message.qos;

        m_pubLoopClient.getTimer().setTime(static_cast<unsigned long>(timer.left_ms()));
        m_pubLoopClient.setValue(id);
        m_loopController.registerLoopClient(&m_pubLoopClient);
        rc = SUCCESS;
    }

    return rc;
}


template<class Network, class Timer, int MAX_PACKET_SIZE, int b>
int MQTTSN::Client<Network, Timer, MAX_PACKET_SIZE, b>::disconnect(unsigned short duration)
{
    int rc = FAILURE;
    Timer timer = Timer(command_timeout_ms);     // we might wait for incomplete incoming publishes to complete
    int int_duration = (duration == 0) ? -1 : (int)duration;
    int len = MQTTSNSerialize_disconnect(sendbuf, MAX_PACKET_SIZE, int_duration);
    if (len > 0)
        rc = sendPacket(len, timer);            // send the disconnect packet

    isconnected = false;
    pmPingResp_outstanding = false;
    return rc;
}


#endif

