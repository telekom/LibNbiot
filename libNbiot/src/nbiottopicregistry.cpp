/* ========================================================================
 * LibNbiot: nbiottopicregistry.cpp
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

#include <list.h>
#include "nbiotdebug.h"
#include "nbiottopicregistry.h"

namespace nbiot {

NbiotTopicRegistry::NbiotTopicRegistry():
    m_registry(),
    messageCount(0)
{

}

NbiotTopicRegistry::~NbiotTopicRegistry()
{
    m_registry.clear();
}

int NbiotTopicRegistry::messageDispatcher(MQTTSN::MessageData& md)
{
    int rc = RC_REJECTED_INVALID_TOPIC_ID;
    for (util::ListIterator<NbiotTopic> it = m_registry.begin(); it != m_registry.end(); ++it)
    {
        if ((*it).valid() && (*it == md.topic.data.id))
        {
            if ((*it).hasHandler())
            {
                NbiotMessage msg;
                msg.qos = (QoS)md.message.qos;
                msg.retained = md.message.retained;
                msg.dup = md.message.dup;
                msg.id = md.message.id;
                msg.payload = md.message.payload;
                msg.payloadlen = md.message.payloadlen;

                MessageData data;
                data.message = &msg;
                data.topicName = (*it).topicName.c_str();

                (*it).mh(&data);
                messageCount++;
            }
            rc = RC_ACCEPTED;
            break;
        }
    }
    return rc;
}

void NbiotTopicRegistry::insertTopic(const NbiotTopic& topic)
{
    // don't allow empty topic-names
    if(!topic.topicName.empty())
    {
        int index = m_registry.indexOf(topic.topicName.c_str());
        if(-1 == index)
        {
            m_registry.append(topic);
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
            debugPrintf("TopicRegistry: ID %04X %s appended\r\n",topic.id, topic.topicName.c_str());
#endif
        }
        else
        {
#ifdef DEBUG_MQTT
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
            debugPrintf("TopicRegistry: ID %04X updated\r\n",topic.id);
#endif
            m_registry[index] = topic;
        }
    }
}

bool NbiotTopicRegistry::attachWildcardMessageHandler(NbiotTopic& topic) const
{
    bool ret = false;
    for (util::ListIterator<NbiotTopic> it = m_registry.cbegin(); it != m_registry.cend(); ++it)
    {
        if ((*it).isWildcardTopic())
        {
            if ((*it).isTopicMatched(topic))
            {
                topic.mh = (*it).mh;
                ret = true;
                break;
            }
        }
    }
    return ret;
}

bool NbiotTopicRegistry::updateWildcardMessageHandler(const NbiotTopic& topic)
{
    bool ret = false;
    if(topic.isWildcardTopic())
    {
        int index = m_registry.indexOf(topic.topicName.c_str());
        if(0 <= index)
        {
            for (util::ListIterator<NbiotTopic> it = m_registry.begin(); it != m_registry.end(); ++it)
            {
                if (!(*it == topic.topicName.c_str()) && !((*it).isWildcardTopic()))
                {
                    if (m_registry[index].isTopicMatched(*it))
                    {
                        (*it).mh = topic.mh;
                        ret = true;
                    }
                }
            }
        }
    }
    return ret;
}

bool NbiotTopicRegistry::attachMessageHandler(int index, messageHandler mh)
{
    bool ret = false;
    if((0 <= index) &&  (m_registry.count() > index))
    {
        NbiotTopic& topic = m_registry[index];
        topic.mh = mh;
        // update existing topics if this is a wildcard topic
        if (topic.isWildcardTopic())
        {
            for (util::ListIterator<NbiotTopic> it = m_registry.begin(); it != m_registry.end(); ++it)
            {
                if (!(*it == topic.topicName.c_str()) && !((*it).isWildcardTopic()))
                {
                    if (topic.isTopicMatched(*it))
                    {
                        (*it).mh = mh;
                    }
                }
            }
        }
        ret = true;
    }
    return ret;
}

int NbiotTopicRegistry::findTopic(char bytes[2]) const
{
    return m_registry.indexOf(bytes);
}

int NbiotTopicRegistry::findTopic(unsigned short topicId) const
{
    return m_registry.indexOf(topicId);
}

int NbiotTopicRegistry::findTopic(const char* name) const
{
    int ret = -1;
    if('\0' != *name) // skip search for empty string
    {
        ret = m_registry.indexOf(name);
    }
    return ret;
}

void NbiotTopicRegistry::setTopicFlag(int index, NbiotTopic::RegistrationFlag topicFlag)
{
    if((0 <= index) &&  (m_registry.count() > index))
    {
        m_registry[index].setReg(topicFlag);
    }
}

void NbiotTopicRegistry::unsetTopicFlag(int index, NbiotTopic::RegistrationFlag topicFlag)
{
    if((0 <= index) &&  (m_registry.count() > index))
    {
        m_registry[index].unsetReg(topicFlag);
    }
}

bool NbiotTopicRegistry::hasAnyTopic(NbiotTopic::RegistrationFlag topicFlag) const
{
    bool ret = false;
    for (util::ListIterator<NbiotTopic> it = m_registry.cbegin(); it != m_registry.cend(); ++it)
    {
        if ((*it).isReg(topicFlag))
        {
            ret = true;
            break;
        }
    }
    return ret;
}

bool NbiotTopicRegistry::hasTopic(const char *topicFilter, NbiotTopic::RegistrationFlag topicFlag) const
{
    bool ret = false;
    int index = findTopic(topicFilter);
    if(0 <= index)
    {
        ret = (this->operator [](index)).isReg(topicFlag);
    }
    return ret;
}

void NbiotTopicRegistry::removeTopic(int index)
{
    NbiotTopic topic = m_registry[index];
    if (topic.isWildcardTopic())
    {
        for(int i = (m_registry.count() - 1); i >= 0; --i)
        {
            if(index != i)
            {
                if(topic.isTopicMatched(m_registry[i]))
                {
                    m_registry.remove(i);
                }
            }
        }
    }
    m_registry.remove(index);
}

void NbiotTopicRegistry::clearRegistry()
{
   m_registry.clear();
}

}
