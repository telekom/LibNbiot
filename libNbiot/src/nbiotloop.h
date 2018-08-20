/* ========================================================================
 * LibNbiot: nbiotloop.h
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

#ifndef NBIOTLOOP_H
#define NBIOTLOOP_H

#include <nbiotdebug.h>
#include <list.h>
#include "nbiotdef.h"


namespace nbiot {

/*!
 * \brief The iLoopControlled class
 */
class iLoopControlled
{
public:
    virtual ~iLoopControlled() {}

    /*!
     * \brief preLoopStep
     * \return
     */
    virtual NbiotResult preLoopStep() = 0;
    /*!
     * \brief loopStep
     * \return
     */
    virtual NbiotResult loopStep() = 0;
    /*!
     * \brief postLoopStep
     * \return
     */
    virtual NbiotResult postLoopStep() = 0;

    /*!
     * \brief getResult
     * \return
     */
    virtual NbiotResult getResult() const = 0;
    /*!
     * \brief getValue
     * \return
     */
    virtual int getValue() const = 0;
    /*!
     * \brief setValue
     */
    virtual void setValue(int) = 0;
    /*!
     * \brief getLoopId
     * \return
     */
    virtual NbiotLoopId getLoopId() const = 0;
};

/*!
 * \brief The NbiotLoop class
 */
class NbiotLoop
{
public:
    /*!
     * \brief NbiotLoop
     */
    NbiotLoop():
        m_clientList(nullptr),
        m_loopClient(nullptr),
        m_loopValue(0),
        m_loopId(LI_Unknown),
        m_breakFlag(false)
    {}
    virtual ~NbiotLoop() {}

    /*!
     * \brief nbiotLoop
     * \return
     */
    NbiotResult nbiotLoop()
    {
        NbiotResult ret = LC_Idle;

        if(m_breakFlag)
        {
            clear();
            m_breakFlag = false;
        }
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
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
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
#ifdef DEBUG_COLOR
                debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
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

    /*!
     * \brief registerLoopClient
     * \param client
     */
    void registerLoopClient(iLoopControlled* client)
    {
        if(nullptr != client)
        {
#ifdef DEBUG_MQTT
            NbiotLoopId loopId = client->getLoopId();
#ifdef DEBUG_COLOR
            debugPrintf("\033[0;32m[ MQTT     ]\033[0m ");
#endif
            debugPrintf("loop controller: register client %s\r\n", (LI_Unknown == loopId)?"LI_Unknown":loop_id[loopId]);
#endif
            m_clientList.append(client);
        }
        else
        {
            clear();
        }
    }

    /*!
     * \brief loopBreak
     */
    void loopBreak()
    {
        if(0 < m_clientList.count()) // don't set break-flag in advance
        {
            m_breakFlag = true;
        }
    }

    /*!
     * \brief getLoopValue
     * \return
     */
    int getLoopValue() const { return m_loopValue; }
    /*!
     * \brief getLoopId
     * \return
     */
    NbiotLoopId getLoopId() const { return m_loopId; }

    /*!
     * \brief isBusy
     * \return
     */
    bool isBusy() const { return ((0 < m_clientList.count()) || (LI_Unknown != m_loopId)); }

    /*!
     * \brief loop_id
     */
    const char* loop_id[8] = {
                        "LI_Cgatt",
                        "LI_Yield",
                        "LI_Connect",
                        "LI_Publish",
                        "LI_Register",
                        "LI_Subscribe",
                        "LI_Unsubscribe",
                        "LI_Disconnect"
                      };

private:
    NbiotLoop(NbiotLoop&);
    NbiotLoop& operator = (NbiotLoop&);

    void clear()
    {
        m_clientList.clear();
        m_loopClient = nullptr;
        m_loopValue = 0;
        m_loopId = LI_Unknown;
    }

    util::list<iLoopControlled*> m_clientList;
    iLoopControlled* m_loopClient;
    int m_loopValue;
    NbiotLoopId m_loopId;
    bool m_breakFlag;
};

}
#endif // NBIOTLOOP_H

