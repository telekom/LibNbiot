#ifndef NBIOTLOOP_H
#define NBIOTLOOP_H

#include <nbiotdebug.h>
#include <list.h>
#include "nbiotdef.h"


namespace nbiot {

class iLoopControlled
{
public:
    virtual ~iLoopControlled() {}

    virtual NbiotResult preLoopStep() = 0;
    virtual NbiotResult loopStep() = 0;
    virtual NbiotResult postLoopStep() = 0;

    virtual NbiotResult getResult() const = 0;
    virtual int getValue() const = 0;
    virtual void setValue(int) = 0;
    virtual NbiotLoopId getLoopId() const = 0;
};

class NbiotLoop
{
public:
    NbiotLoop():
        m_clientList(nullptr),
        m_loopClient(nullptr),
        m_loopValue(0),
        m_loopId(LI_Unknown)
    {}
    virtual ~NbiotLoop() {}

    NbiotResult nbiotLoop()
    {
        NbiotResult ret = LC_Idle;
        // is there a client hooked?
        if(nullptr != m_loopClient)
        {
            // loop steps pending?
            ret = m_loopClient->loopStep();
            m_loopValue = m_loopClient->getValue();
            if(LC_Pending != ret)
            {
                ret = m_loopClient->getResult();

                m_loopClient = nullptr;
            }
#ifdef DEBUG_MQTT
            debugPrintf("loop controller: step %s %s\r\n", (LI_Unknown == m_loopId)?"LI_Unknown":loop_id[m_loopId], (LC_Pending != ret)?"finished":"ok");
#endif
        }
        else
        {
            if(m_clientList.count())
            {
                m_loopClient = m_clientList.take_front();
                m_loopId = m_loopClient->getLoopId();
                ret = m_loopClient->preLoopStep();
                if(LC_Pending != ret)
                {
                    m_loopClient = nullptr;
                    m_loopId = LI_Unknown;
                }
#ifdef DEBUG_MQTT
                debugPrintf("loop controller: start %s %s\r\n", (LI_Unknown == m_loopId)?"LI_Unknown":loop_id[m_loopId], (LC_Pending != ret)?"fail":"ok");
#endif
            }
            else
            {
                m_loopValue = 0;
                m_loopId = LI_Unknown;
            }
        }
        return ret;
    }

    void registerLoopClient(iLoopControlled* client)
    {
        if(nullptr != client)
        {
#ifdef DEBUG_MQTT
            NbiotLoopId loopId = client->getLoopId();
            debugPrintf("loop controller: register client %s\r\n", (LI_Unknown == loopId)?"LI_Unknown":loop_id[loopId]);
#endif
            m_clientList.append(client);
        }
        else
        {
            m_clientList.clear();
            m_loopClient = nullptr;
            m_loopValue = 0;
            m_loopId = LI_Unknown;
        }
    }

    int getLoopValue() const { return m_loopValue; }
    NbiotLoopId getLoopId() const { return m_loopId; }

    bool isBusy() const { return ((0 < m_clientList.count()) || (LI_Unknown != m_loopId)); }

    const char* loop_id[7] = {
                        "LI_Cgatt",
                        "LI_Yield",
                        "LI_Connect",
                        "LI_Publish",
                        "LI_Register",
                        "LI_Subscribe",
                        "LI_Unsubscribe"
                      };

private:
    NbiotLoop(NbiotLoop&);
    NbiotLoop& operator = (NbiotLoop&);

    util::list<iLoopControlled*> m_clientList;
    iLoopControlled* m_loopClient;
    int m_loopValue;
    NbiotLoopId m_loopId;
};

}
#endif // NBIOTLOOP_H

