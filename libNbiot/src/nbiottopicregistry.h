#ifndef NBIOTTOPICREGISTRY_H
#define NBIOTTOPICREGISTRY_H

#include "nbiotmessage.h"
#include "MQTTSNClient.h"

#include "nbiotregistrylist.h"

namespace nbiot {

class NbiotTopicRegistry
{
public:
    NbiotTopicRegistry();
    virtual ~NbiotTopicRegistry();

    int messageDispatcher(MQTTSN::MessageData& md);

    const NbiotTopic& operator [](int index) const
    {
        return m_registry.at(index);
    }

    void insertTopic(const NbiotTopic& topic);

    bool attachWildcardMessageHandler(NbiotTopic& topic) const;
    bool updateWildcardMessageHandler(const NbiotTopic& topic);
    bool attachMessageHandler(int index, messageHandler mh);

    int findTopic(char bytes[2]) const;
    int findTopic(unsigned short topicId) const;
    int findTopic(const char* name) const;

    void setTopicFlag(int index, NbiotTopic::RegistrationFlag topicFlag);
    void unsetTopicFlag(int index, NbiotTopic::RegistrationFlag topicFlag);

    unsigned long arrivedMessages() const { return messageCount; }
    bool hasAnyTopic(NbiotTopic::RegistrationFlag topicFlag = NbiotTopic::REG_BOTH) const;
    bool hasTopic(const char *topicFilter, NbiotTopic::RegistrationFlag topicFlag = NbiotTopic::REG_BOTH) const;
    int topicCount() const { return m_registry.count(); }

    void removeTopic(int index);

    void clearRegistry();

private:
    NbiotTopicRegistry(const NbiotTopicRegistry&);
    NbiotTopicRegistry& operator=(const NbiotTopicRegistry&);

    NbiotRegistryList m_registry;
    unsigned long messageCount;
};

}
#endif // NBIOTTOPICREGISTRY_H
