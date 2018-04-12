/* ========================================================================
 * LibNbiot: nbiottopicregistry.h
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

#ifndef NBIOTTOPICREGISTRY_H
#define NBIOTTOPICREGISTRY_H

#include "nbiotmessage.h"
#include "nbiotmqttsnclient.h"

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
