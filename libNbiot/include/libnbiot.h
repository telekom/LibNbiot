/* ======================================================================
 * LibNbiot: API definition
 *
 * Copyright (c) 2018, Edgar Hindemith, Yassine Amraue, Thorsten Krautscheid, Kolja Vornholt, T-Systems International GmbH
 * contact: libnbiot@t-systems.com, opensource@telekom.de
 *
 * This file is distributed under the conditions of the Apache2 license.
 * For details see the file LICENSE at the toplevel.
 * ======================================================================
*/

#ifndef LIBNBIOT_H
#define LIBNBIOT_H
#include <stdio.h> // for size_t
#include "nbiotdef.h"
#include "nbiotmessage.h"
#include "nbiotnotify.h"

/*! \mainpage
 *
 * \section intro Introduction
 *
 * The nbIOT library controls the modem connected to the mcu.
 * It provides a high level access to functions for sending and
 * receiving MQTT messages to/from a cloud.
 *
 * \section details Details
 * 
 * \image html stm.png
 *
 * The provided functions are declared in the \link libnbiot.h \endlink header.
 *
 */

#define MIN_KEEPALIVE 60 /*!< MQTT-KEEPALIVE: minimum one minute */

#define MAX_KEEPALIVE 86400 /*!< MQTT-KEEPALIVE: maximum 24 hours */


#define MIN_AUTOPOLL 0 /*!< AUTOPOLL: minimum 0 (default), means switched off */

#define MAX_AUTOPOLL 86399 /*!< AUTOPOLL: maximum 23:59:59 h; if autopoll is used, the maximum of the value should be one second less than keepAliveInterval */


#define MIN_TOPIC_COUNT_LIMIT 1 /*!< MAXTOPICCOUNT: minimum 1 = only one topic allowed */

#define MAX_TOPIC_COUNT_LIMIT 256 /*!< MAXTOPICCOUNT: maximum 256 topics : 256 * (22 Bytes + individual topic length) of RAM usage (max) */


#define MIN_PORT 1024 /*!< PORT: values below 1024 will cause an error */

#define DEFAULT_PORT 1883 /*!< PORT: default MQTT-port */

#define ECHO_PORT 16666 /*!< ECHO_PORT: port of the echo service */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief The NbiotConf struct is used to configure the library with nbiotConfig
 */
struct NbiotConf
{
    void (*notify_fu)(const Notification*);
    unsigned int keepAliveInterval; /*!< The maximum time to send a keepalive-message to the gateway */
    unsigned int autoPollInterval; /*!< The interval to invoke an automatic keepalive-message (must be less than keepAliveInterval) */
    unsigned short maxTopicCount; /*!< The maximum number of topics. Wildcard subscribtions may exceed this limit. Adjust it carfully: costs RAM. */
    const char* gateway; /*!< IP-address of the nbIOT gateway server */
    unsigned short port; /*!< (MQTT-)port of the nbIOT gateway server */
};

typedef struct NbiotConf NbiotConf;

#define NbiotConf_initializer {NULL, NULL, NULL, NULL, NULL, 300, 0, 20, NULL, 1883, NULL, NULL, NULL, NULL}

/*!
 * \brief The NbiotConfigError enum enumerates the possible errors of the configuration ( nbiotConfig returns an OR'ed result )
 */
enum NbiotConfigError
{
    NoError = 0, /*!< No error, configuration successful */
    ErrorKeepAliveInt = 64, /*!< Invalid keepalive-interval */
    ErrorAutoPollInt = 128, /*!< Invalid autopoll-interval */
    ErrorMaxTopicCount = 256, /*!< Invalid topic maximum */
    ErrorGateway = 512, /*!< No gateway */
    ErrorPort = 1024, /*!< Invalid port number (privileged-port-numbers: < 1024) */
    ErrorMultipleConfig = 16384, /*!< Configuration was called more than one time */
    ErrorNoConfig = 32768 /*!< Configuration was never called */
};


/* nbiot-functions ----------------------------------------------------------- */

/*!
 * \brief nbiotConfig configures the library
 * \param conf pointer to NbiotConf struct
 * \return OR'ed combination of NbiotConfigError values or NoError in case of success
 */
unsigned int nbiotConfig(NbiotConf* conf);

/*!
 * \brief nbiotConnect sends a Connect Message to the MQTT-SN Gateway and uses the configuration set by nBiotConfig
 *
 * cleanSession=1 forces to delete the internal topic-registry-list and thus to forget all registered topic-id topic-name
 * \param cleanSession 1 causes a clean session according to the MQTT-SN specs while connecting; 0 does not cause a clean session
 * \return =0 indicates success; >=1 indicates an error according to the defined ErrorNumbers in nbiotdef.h
 */
int nbiotConnect(unsigned char cleanSession);

/*!
 * \brief nbiotPublish publishes arbitrary data to the specified topic
 *
 * nbiotPublish also manages an internal topicregistry-list and registers a topic with the MQTT-SN-Gateway according to the MQTT-SN spec if not already registered. The topicregistry-list contains registered topics and therefore saves a topic-id - topic-name mapping. Currently, the only way to clear this topic-registry list is to connect with cleanSession=1.
 * In case of QOS0 a publish message is directly send to the Gateway/Server (Fire and Forget Principle). QoS1 expects a Puback-Answer from the Gateway. In order to preserve non-blocking routines in case of QoS1, nbiotPublish appends a publish routine to an internal event queue which is processed if the user calls NbiotLoop. This is why a return code of 0 in case of QoS1 does not mean that the publish was successfull (indicating Puback received) but rather only indicates whether the publish task could be appended to the internal event queue.
 * \param topic defines the topic the data is puplished to
 * \param data contains the message that is published to the topic
 * \param len describes the length of the message
 * \param qos defines the quality of service level according the MQTT-Spec
 * \return =0 indicates Success (see in the description for detailed explanation), >=1 indicates an error according to the defined ErrorNumbers in nbiotdef.h
 */
int nbiotPublish(const char* topic, const char* data, size_t len, enum QoS qos);

/*!
 * \brief nbiotSubscribe subscribe to the given topic and install a handler function to receive the incoming messages
 *
 * nbiotSubscribe also manages an internal topicregistry-list and adds
 * \param topic defines the topic that the client wants to subscribe to
 * \param mh defines the handler function
 * \return =0 indicates Success, >=1 indicates an error according to the defined ErrorNumbers in nbiotdef.h
 */
int nbiotSubscribe(const char* topic, messageHandler mh);

/*!
 * \brief nbiotUnsubscribe unsubscribe from the given topic
 * \param topic
 */
int nbiotUnsubscribe(const char* topic);

/*!
 * \brief nbiotDisconnect MQTT disconnect
 */
void nbiotDisconnect();

/*!
 * \brief nbiotSleep puts internal statemachine to 'connected sleep' state
 */
void nbiotSleep(unsigned short duration);

/*!
 * \brief nbiotHibernate disconnects and switch off modem
 */
void nbiotHibernate(unsigned short duration);

/*  */
/*!
 * \brief nbiotWakeup returns from sleep or hibernate. the lib will be in disconnected state after hibernate
 */
void nbiotWakeup();

/*  */
/*!
 * \brief nbiotPoll polls the gateway/broker to maintain the keep-alive and to look for new messages
 */
void nbiotPoll();

/*!
 * \brief isNbiotConnected checks whether the internal state machine state is connected
 * \returns 1 if the client is connected or 0 otherwise
 */
unsigned char isNbiotConnected();

/*!
 * \deprecated
 * \brief isNbiotReady
 * \returns 1 if the modem is initialized or 0 otherwise
 */
unsigned char isNbiotReady();

/*!
 * \brief hasNbiotSubscription checks whether the client has any subscription
 * \returns 1 if there is any subscription or 0 otherwise
 */
unsigned char hasNbiotSubscription();

/*!
 * \brief isNbiotSubscribedTo checks whether the client is subscribed to the given topic
 * \param topic
 * \returns 1 if the client subscribed the given topic or 0 otherwise
 */
unsigned char isNbiotSubscribedTo(const char* topic);

/*!
 * \brief nbiotStart starts the engine
 * \return
 */
unsigned char nbiotStart();

/*!
 * \brief getNbiotState returns current state of the internal state machine
 * \returns the id of the current state (internal statemachine) according to the enum StateIds in nbiotdef.h
 */
unsigned int getNbiotState();

/*!
 * \brief getNbiotError
 * \returns the error-number of the internal statemachine
 */
unsigned int getNbiotError();

/*!
 * \brief nbiotEventLoop executes pending actions at the event queue. If called once nbiotEventLoop executes only one pending action. Need to be called periodically (interval <= 1s) while there's nothing else to do.
 *
 * \returns indicates the status of the internal event queue according the enum NbiotResult in nbiotdef.h. LC_Idle(0) means that there is no action currently executing, because e.g. there are no pending actions in the queue or one was successfully executed. LC_Pending(1) means that there is an action currently executing.
 */
enum NbiotResult nbiotEventLoop(NbiotEventMode mode);

/* /nbiot-functions ---------------------------------------------------------- */

/*!
 * \deprecated
 * \brief currentNbiotState
 * \returns the name of the current state
 */
const char* currentNbiotState();

#ifdef __cplusplus
}
#endif

#endif // LIBNBIOT_H

