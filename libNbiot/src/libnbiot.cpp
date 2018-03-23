#include "nbiotmqtt.h"
#include "libnbiot.h"

NbiotMQTT nbiotMqtt;

unsigned int configError = ErrorNoConfig;


unsigned int nbiotConfig(NbiotConf* conf)
{
    unsigned int result = NoError;

    if(ErrorNoConfig == configError)
    {
        if(NULL != conf->notify_fu)
        {
            nbiotMqtt.setNotifyHandler(conf->notify_fu);
        }
        else
        {
            // App does not want to be notified = not an error
        }

        if((MIN_KEEPALIVE <= conf->keepAliveInterval) && (MAX_KEEPALIVE >= conf->keepAliveInterval))
        {
            nbiotMqtt.setKeepAlive(conf->keepAliveInterval);
        }
        else
        {
            result |= ErrorKeepAliveInt;
        }

        if((MAX_AUTOPOLL >= conf->autoPollInterval) && (conf->autoPollInterval < conf->keepAliveInterval))
        {
            nbiotMqtt.setAutoPoll(conf->autoPollInterval);
        }
        else
        {
            result |= ErrorAutoPollInt;
        }

        if((MIN_TOPIC_COUNT_LIMIT <= conf->maxTopicCount) && (MAX_TOPIC_COUNT_LIMIT >= conf->maxTopicCount))
        {
            nbiotMqtt.setMaxTopics(conf->maxTopicCount);
        }
        else
        {
            result |= ErrorMaxTopicCount;
        }

        if(MIN_POLL_INTERVAL <= conf->pollInterval)
        {
            nbiotMqtt.setPollInterval(conf->pollInterval);
        }
        else
        {
            result |= ErrorPollInterval;
        }

        if((NULL != conf->gateway) && (0 < strlen(conf->gateway)))
        {
            nbiotMqtt.setGateway(conf->gateway);
        }
        else
        {
            result |= ErrorGateway;
        }

        if(MIN_PORT <= conf->port)
        {
            nbiotMqtt.setPort(conf->port);
        }
        else
        {
            result |= ErrorPort;
        }
        
        configError = result;
    }
    else
    {
        result = ErrorMultipleConfig;
    }

    return result;
}


int nbiotConnect(unsigned char cleanSession)
{
    return nbiotMqtt.connect(cleanSession);
}

int nbiotPublish(const char *topic, const char* data, size_t len, enum QoS qos)
{
    return nbiotMqtt.publish(topic, data, len, qos);
}

int nbiotSubscribe(const char *topic, messageHandler mh)
{
    return nbiotMqtt.subscribe(topic, mh);
}

int nbiotUnsubscribe(const char* topic)
{
    return nbiotMqtt.unsubscribe(topic);
}

void nbiotDisconnect()
{
    nbiotMqtt.disconnect();
}

void nbiotSleep(unsigned short duration)
{
    nbiotMqtt.sleep(duration);
}

void nbiotHibernate(unsigned short duration)
{
    nbiotMqtt.hibernate(duration);
}

void nbiotWakeup()
{
    nbiotMqtt.wakeup();
}

void nbiotPoll(unsigned short pollInterval)
{
    nbiotMqtt.poll(pollInterval);
}

unsigned char isNbiotConnected()
{
    unsigned char ret = 0;
    if(nbiotMqtt.isConnected() || nbiotMqtt.isConSleeping() || nbiotMqtt.isConAwake())
    {
        ret = 1;
    }
    return ret;
}

unsigned char isNbiotReady()
{
    unsigned char ret = 0;
    if(nbiotMqtt.ready())
    {
        ret = 1;
    }
    return ret;
}

unsigned char hasNbiotSubscription()
{
    unsigned char ret = 0;
    if(nbiotMqtt.hasSubscription())
    {
        ret = 1;
    }
    return ret;
}

unsigned char isNbiotSubscribedTo(const char* topic)
{
    unsigned char ret = 0;
    if(nbiotMqtt.hasSubscription(topic))
    {
        ret = 1;
    }
    return ret;
}

unsigned char nbiotStart()
{
    unsigned char ret = 0;
    if(NoError == configError)
    {
        if(!nbiotMqtt.validState()) // STM: not setup and not started yet
        {
            nbiotMqtt.setUpStateMachine();
            nbiotMqtt.startStateMachine();
            ret = 1;
        }
    }
    return ret;
}

unsigned int getNbiotState()
{
    return nbiotMqtt.getCurrentStateId();
}


unsigned int getNbiotError()
{
    return nbiotMqtt.getErrorNumber();
}

NbiotResult nbiotEventLoop(NbiotEventMode mode)
{
    return nbiotMqtt.eventLoop(mode);
}

NbiotResult getNbiotEventLoopStatus()
{
    return nbiotMqtt.eventLoopStatus();
}

const char* currentNbiotState()
{
    return nbiotMqtt.currentStateToString();
}
