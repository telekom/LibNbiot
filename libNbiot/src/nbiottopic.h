/* ========================================================================
 * LibNbiot: nbiottopic.h
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

#ifndef NBIOTTOPIC_H
#define NBIOTTOPIC_H

#include "MQTTSNPacket.h"

#include "nbiotstring.h"
#include "nbiotmessage.h"

namespace nbiot {
/*!
 * \brief The NbiotShortTopic class
 */
class NbiotShortTopic
{
public:
    /*!
     * \brief NbiotShortTopic
     */
    NbiotShortTopic()
    {
        shortName[0] = '\0';
        shortName[1] = '\0';
    }
    /*!
     * \brief NbiotShortTopic
     * \param other
     */
    NbiotShortTopic(const NbiotShortTopic& other)
    {
        shortName[0] = other.shortName[0];
        shortName[1] = other.shortName[1];
    }
    /*!
     * \brief NbiotShortTopic
     * \param bytes
     */
    NbiotShortTopic(char bytes[2])
    {
        shortName[0] = bytes[0];
        shortName[1] = bytes[1];
    }
    /*!
     * \brief NbiotShortTopic
     * \param b0
     * \param b1
     */
    NbiotShortTopic(char b0, char b1)
    {
        shortName[0] = b0;
        shortName[1] = b1;
    }


    /*!
     * \brief operator =
     * \param other
     * \return
     */
    NbiotShortTopic& operator = (const NbiotShortTopic& other)
    {
        shortName[0] = other.shortName[0];
        shortName[1] = other.shortName[1];
        return *this;
    }
    /*!
     * \brief operator =
     * \param bytes
     * \return
     */
    NbiotShortTopic& operator = (char bytes[2])
    {
        shortName[0] = bytes[0];
        shortName[1] = bytes[1];
        return *this;
    }
    /*!
     * \brief operator ==
     * \param bytes
     * \return
     */
    bool operator == (char bytes[2])
    {
        return ((shortName[0] == bytes[0]) &&
                (shortName[1] == bytes[1]));
    }
    /*!
     * \brief operator ==
     * \param bytes
     * \return
     */
    bool operator == (const char bytes[2])
    {
        return ((shortName[0] == bytes[0]) &&
                (shortName[1] == bytes[1]));
    }

    /*!
     * \brief shortName
     */
    char shortName[2];
};

/*!
 * \brief The NbiotTopic class
 */
class NbiotTopic
{
public:
    /*!
     * \brief The TopicTypeNormal enum
     */
    enum TopicTypeNormal
    {
        TOPIC_NAME,
        TOPIC_ID
    };

    /*!
     * \brief The RegistrationFlag enum
     */
    enum RegistrationFlag
    {
        REG_NONE = 0,
        REG_PUB = 1,
        REG_SUB = 2,
        REG_BOTH = 3
    };

    /*!
     * \brief NbiotTopic
     */
    NbiotTopic():type(MQTTSN_TOPIC_TYPE_NORMAL),id(0),shortName(),topicName(), returnCode(MQTTSN_RC_ACCEPTED), mh(nullptr), regFlag(REG_NONE){}
    /*!
     * \brief NbiotTopic
     * \param bytes
     */
    NbiotTopic(char bytes[2]):type(MQTTSN_TOPIC_TYPE_SHORT),id(0),shortName(bytes),topicName(), returnCode(MQTTSN_RC_ACCEPTED), mh(nullptr), regFlag(REG_NONE){}
    /*!
     * \brief NbiotTopic
     * \param topicId
     */
    NbiotTopic(unsigned short topicId):type(MQTTSN_TOPIC_TYPE_PREDEFINED),id(topicId),shortName(),topicName(), returnCode(MQTTSN_RC_ACCEPTED), mh(nullptr), regFlag(REG_NONE){}
    /*!
     * \brief NbiotTopic
     * \param name
     */
    NbiotTopic(const char* name):type(MQTTSN_TOPIC_TYPE_NORMAL),id(0),shortName(),topicName(name), returnCode(MQTTSN_RC_ACCEPTED), mh(nullptr), regFlag(REG_NONE){}

    /*!
     * \brief operator =
     * \param other
     * \return
     */
    NbiotTopic& operator = (const NbiotTopic& other)
    {
        type = other.type;
        id = other.id;
        shortName = other.shortName;
        topicName = other.topicName;
        returnCode = other.returnCode;
        if(nullptr == mh)   // don't update messageHandler in case of id update
        {                   // because other.mh will be a nullptr
            mh = other.mh;  // to change or remove messageHandler set it manually
        }
        regFlag = other.regFlag;
        return *this;
    }

    /*!
     * \brief operator ==
     * \param topicId
     * \return
     */
    bool operator == (unsigned short topicId) { return (id == topicId); }
    /*!
     * \brief operator ==
     * \param bytes
     * \return
     */
    bool operator == (char bytes[2]) { return (shortName == bytes); }
    /*!
     * \brief operator ==
     * \param name
     * \return
     */
    bool operator == (const char* name) { return (topicName == name); }
    /*!
     * \brief operator ==
     * \param topicid
     * \return
     */
    bool operator == (const MQTTSN_topicid& topicid)
    {
        if(type == topicid.type)
        {
            switch(topicid.type)
            {
                case MQTTSN_TOPIC_TYPE_SHORT:
                    return (shortName == topicid.data.short_name);
                case MQTTSN_TOPIC_TYPE_PREDEFINED:
                    return (id == topicid.data.id);
                case MQTTSN_TOPIC_TYPE_NORMAL:
                // fallthru
                default:
                    return (
                            ((0 != topicid.data.id) && (id == topicid.data.id)) ||
                            (topicName == string(topicid.data.long_.name, topicid.data.long_.len))
                           );
            }
        }
        return false;
    }

    /*!
     * \brief valid
     * \return
     */
    bool valid()
    {
        switch(type)
        {
            case MQTTSN_TOPIC_TYPE_SHORT:
                return (!(shortName == NbiotShortTopic().shortName));
            case MQTTSN_TOPIC_TYPE_PREDEFINED:
                return (0 != id);
            case MQTTSN_TOPIC_TYPE_NORMAL:
            // fallthru
            default:
                return ((0 != id) || (!topicName.empty()));
        }
    }

    /*!
     * \brief setReg
     * \param flag
     */
    void setReg(RegistrationFlag flag)
    {
        if(REG_NONE != flag)
        {
            regFlag |= (static_cast<unsigned char>(flag));
        }
        else
        {
            regFlag = REG_NONE;
        }
    }
    /*!
     * \brief unsetReg
     * \param flag
     */
    void unsetReg(RegistrationFlag flag)
    {
        if(REG_NONE != flag)
        {
            regFlag &= ~(static_cast<unsigned char>(flag));
        }
        else
        {
            regFlag = REG_BOTH;
        }
    }
    /*!
     * \brief isReg
     * \param flag
     * \return
     */
    bool isReg(RegistrationFlag flag) const
    {
        if(REG_NONE != flag)
        {
            return (static_cast<bool>(regFlag & flag));
        }
        else
        {
            return (REG_NONE == regFlag);
        }
    }

    /*!
     * \brief hasHandler
     * \return
     */
    bool hasHandler() const { return (nullptr != mh); }

    /*!
     * \brief isWildcardTopic
     * \return
     */
    bool isWildcardTopic() const
    {
        return ((!topicName.empty()) && ((string::npos != topicName.find('+')) || (string::npos != topicName.find('#'))));
    }

    /*!
     * \brief toSNTopic
     * \param topicid
     * \param ttn
     */
    void toSNTopic(MQTTSN_topicid& topicid, TopicTypeNormal ttn = TOPIC_NAME)
    {
        topicid.type = type;
        switch(type)
        {
            case MQTTSN_TOPIC_TYPE_SHORT:
                topicid.data.short_name[0] = shortName.shortName[0];
                topicid.data.short_name[1] = shortName.shortName[1];
                break;
            case MQTTSN_TOPIC_TYPE_PREDEFINED:
                topicid.data.id = id;
                break;
            case MQTTSN_TOPIC_TYPE_NORMAL:
            // fallthru
            default:
            {
                if(TOPIC_NAME == ttn)
                {
                    topicid.data.long_.name = topicName.getData();
                    topicid.data.long_.len = topicName.size();
                }
                else
                {
                    topicid.data.id = id;
                }
            }
            break;
        }
    }

    /*!
     * \brief isTopicMatched
     * slightly modified copy of MQTTClient::isTopicMatched
     *
     * assume topic filter and name is in correct format
     * # can only be at end
     * + and # can only be next to separator
     *
     * \param topic
     * \return
     */
    bool isTopicMatched(const NbiotTopic& topic) const
    {
        unsigned int i = 0;
        const char* curn = topic.topicName.c_str();
        const char* curn_end = curn + topic.topicName.size();

        while (topicName[i] && curn < curn_end)
        {
            if (*curn == '/' && topicName[i] != '/')
                break;
            if (topicName[i] != '+' && topicName[i] != '#' && topicName[i] != *curn)
                break;
            if (topicName[i] == '+')
            {   // skip until we meet the next separator, or end of string
                const char* nextpos = curn + 1;
                while (nextpos < curn_end && *nextpos != '/')
                    nextpos = ++curn + 1;
            }
            else if (topicName[i] == '#')
                curn = curn_end - 1;    // skip until end of string
            i++;
            curn++;
        };

        return (curn == curn_end) && (topicName[i] == '\0');
    }


    /*!
     * \brief type
     */
    enum MQTTSN_topicTypes type;
    /*!
     * \brief id
     */
    unsigned short id;
    /*!
     * \brief shortName
     */
    NbiotShortTopic shortName;
    /*!
     * \brief topicName
     */
    string topicName;
    /*!
     * \brief returnCode
     */
    unsigned char returnCode; // used in notifications about this topic
    /*!
     * \brief mh
     */
    messageHandler mh;
    /*!
     * \brief regFlag
     */
    unsigned char regFlag;
};

} // end of ns nbiot

#endif // NBIOTTOPIC_H

