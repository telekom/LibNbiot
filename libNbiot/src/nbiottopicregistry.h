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

/*!
 * \brief The NbiotTopicRegistry class
 */
class NbiotTopicRegistry
{
public:
    /*!
     * \brief NbiotTopicRegistry
     */
    NbiotTopicRegistry();
    /*!
     * \brief ~NbiotTopicRegistry
     */
    virtual ~NbiotTopicRegistry();

    /*!
     * \brief messageDispatcher
     * \param md
     * \return
     */
    int messageDispatcher(MQTTSN::MessageData& md);

    /*!
     * \brief operator []
     * \param index
     * \return
     */
    const NbiotTopic& operator [](int index) const
    {
        return m_registry.at(index);
    }

    /*!
     * \brief insertTopic
     * \param topic
     */
    void insertTopic(const NbiotTopic& topic);

    /*!
     * \brief attachWildcardMessageHandler
     * \param topic
     * \return
     */
    bool attachWildcardMessageHandler(NbiotTopic& topic) const;
    /*!
     * \brief updateWildcardMessageHandler
     * \param topic
     * \return
     */
    bool updateWildcardMessageHandler(const NbiotTopic& topic);
    /*!
     * \brief attachMessageHandler
     * \param index
     * \param mh
     * \return
     */
    bool attachMessageHandler(int index, messageHandler mh);

    /*!
     * \brief findTopic
     * \param bytes
     * \return
     */
    int findTopic(char bytes[2]) const;
    /*!
     * \brief findTopic
     * \param topicId
     * \return
     */
    int findTopic(unsigned short topicId) const;
    /*!
     * \brief findTopic
     * \param name
     * \return
     */
    int findTopic(const char* name) const;

    /*!
     * \brief setTopicFlag
     * \param index
     * \param topicFlag
     */
    void setTopicFlag(int index, NbiotTopic::RegistrationFlag topicFlag);
    /*!
     * \brief unsetTopicFlag
     * \param index
     * \param topicFlag
     */
    void unsetTopicFlag(int index, NbiotTopic::RegistrationFlag topicFlag);

    /*!
     * \brief arrivedMessages
     * \return
     */
    unsigned long arrivedMessages() const { return messageCount; }
    /*!
     * \brief hasAnyTopic
     * \param topicFlag
     * \return
     */
    bool hasAnyTopic(NbiotTopic::RegistrationFlag topicFlag = NbiotTopic::REG_BOTH) const;
    /*!
     * \brief hasTopic
     * \param topicFilter
     * \param topicFlag
     * \return
     */
    bool hasTopic(const char *topicFilter, NbiotTopic::RegistrationFlag topicFlag = NbiotTopic::REG_BOTH) const;
    /*!
     * \brief topicCount
     * \return
     */
    int topicCount() const { return m_registry.count(); }

    /*!
     * \brief removeTopic
     * \param index
     */
    void removeTopic(int index);

    /*!
     * \brief clearRegistry
     */
    void clearRegistry();

private:
    NbiotTopicRegistry(const NbiotTopicRegistry&);
    NbiotTopicRegistry& operator=(const NbiotTopicRegistry&);

    NbiotRegistryList m_registry;
    unsigned long messageCount;
};

}
#endif // NBIOTTOPICREGISTRY_H
