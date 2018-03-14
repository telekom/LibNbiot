#ifndef NBIOTTOPIC_H
#define NBIOTTOPIC_H

#include "MQTTSNPacket.h"

#include "nbiotstring.h"
#include "nbiotmessage.h"

namespace nbiot {
class NbiotShortTopic
{
public:
    NbiotShortTopic()
    {
        shortName[0] = '\0';
        shortName[1] = '\0';
    }
    NbiotShortTopic(const NbiotShortTopic& other)
    {
        shortName[0] = other.shortName[0];
        shortName[1] = other.shortName[1];
    }
    NbiotShortTopic(char bytes[2])
    {
        shortName[0] = bytes[0];
        shortName[1] = bytes[1];
    }
    NbiotShortTopic(char b0, char b1)
    {
        shortName[0] = b0;
        shortName[1] = b1;
    }


    NbiotShortTopic& operator = (const NbiotShortTopic& other)
    {
        shortName[0] = other.shortName[0];
        shortName[1] = other.shortName[1];
        return *this;
    }
    NbiotShortTopic& operator = (char bytes[2])
    {
        shortName[0] = bytes[0];
        shortName[1] = bytes[1];
        return *this;
    }
    bool operator == (char bytes[2])
    {
        return ((shortName[0] == bytes[0]) &&
                (shortName[1] == bytes[1]));
    }
    bool operator == (const char bytes[2])
    {
        return ((shortName[0] == bytes[0]) &&
                (shortName[1] == bytes[1]));
    }

    char shortName[2];
};

class NbiotTopic
{
public:
    enum TopicTypeNormal
    {
        TOPIC_NAME,
        TOPIC_ID
    };

    enum RegistrationFlag
    {
        REG_NONE = 0,
        REG_PUB = 1,
        REG_SUB = 2,
        REG_BOTH = 3
    };

    NbiotTopic():type(MQTTSN_TOPIC_TYPE_NORMAL),id(0),shortName(),topicName(), returnCode(MQTTSN_RC_ACCEPTED), mh(nullptr), regFlag(REG_NONE){}
    NbiotTopic(char bytes[2]):type(MQTTSN_TOPIC_TYPE_SHORT),id(0),shortName(bytes),topicName(), returnCode(MQTTSN_RC_ACCEPTED), mh(nullptr), regFlag(REG_NONE){}
    NbiotTopic(unsigned short topicId):type(MQTTSN_TOPIC_TYPE_PREDEFINED),id(topicId),shortName(),topicName(), returnCode(MQTTSN_RC_ACCEPTED), mh(nullptr), regFlag(REG_NONE){}
    NbiotTopic(const char* name):type(MQTTSN_TOPIC_TYPE_NORMAL),id(0),shortName(),topicName(name), returnCode(MQTTSN_RC_ACCEPTED), mh(nullptr), regFlag(REG_NONE){}

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

    bool operator == (unsigned short topicId) { return (id == topicId); }
    bool operator == (char bytes[2]) { return (shortName == bytes); }
    bool operator == (const char* name) { return (topicName == name); }
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

    bool hasHandler() const { return (nullptr != mh); }

    bool isWildcardTopic() const
    {
        return ((!topicName.empty()) && ((string::npos != topicName.find('+')) || (string::npos != topicName.find('#'))));
    }

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

    // slightly modified copy of MQTTClient::isTopicMatched
    //
    // assume topic filter and name is in correct format
    // # can only be at end
    // + and # can only be next to separator
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


    enum MQTTSN_topicTypes type;
    unsigned short id;
    NbiotShortTopic shortName;
    string topicName;
    unsigned char returnCode; // used in notifications about this topic
    messageHandler mh;
    unsigned char regFlag;
};

} // end of ns nbiot

#endif // NBIOTTOPIC_H

