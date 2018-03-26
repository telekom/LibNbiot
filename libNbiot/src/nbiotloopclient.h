/* ========================================================================
 * LibNbiot: nbiotloopclient.h
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

#ifndef NBIOTLOOPCLIENT_H
#define NBIOTLOOPCLIENT_H

#include <nbiottimer.h>
#include <FP.h>
#include "nbiotloop.h"

namespace nbiot {

class LoopClient:public iLoopControlled
{
public:
    explicit LoopClient(NbiotLoopId loopId):
        m_result(LC_Fail),
        m_loopCtrl(0),
        id(loopId)
    {}
    virtual ~LoopClient() {}

    virtual NbiotResult preLoopStep()
    {
        if(LC_Pending != m_result)
        {
            if(loopStartHandler.attached())
            {
                if(loopStartHandler(m_loopCtrl))
                {
                    m_result = LC_Pending;
                }
                else
                {
                    m_result = LC_Fail;
                }
            }
            else
            {
                m_result = LC_Pending;
            }
        }
        return m_result;
    }

    virtual NbiotResult loopStep()
    {
        if(LC_Pending == m_result)
        {
            if(loopStepHandler.attached())
            {
                if(loopStepHandler(m_loopCtrl))
                {
                    m_result = LC_Pending;
                }
                else
                {
                    if(LC_Success == postLoopStep())
                    {
                        m_result = LC_Idle;
                    }
                    else
                    {
                        m_result = LC_Fail;
                    }
                }
            }
            else
            {
                m_result = LC_Fail;
            }
        }
        return m_result;
    }

    virtual NbiotResult postLoopStep()
    {
        if(LC_Pending == m_result)
        {
            if(loopStopHandler.attached())
            {
                if(loopStopHandler(m_loopCtrl))
                {
                    m_result = LC_Success;
                }
                else
                {
                    m_result = LC_Fail;
                }
            }
            else
            {
                m_result = LC_Success;
            }
        }
        return m_result;
    }

    virtual NbiotResult getResult() const
    {
        return m_result;
    }

    virtual int getValue() const
    {
        return m_loopCtrl;
    }

    virtual void setValue(int val)
    {
        m_loopCtrl = val;
    }

    template<class LHC>
    void setLoopStartHandler(LHC* lhc, bool (LHC::*loopstartHandler)(int&))
    {
        loopStartHandler.attach(lhc, loopstartHandler);
    }

    template<class LHC>
    void setLoopStepHandler(LHC* lhc, bool (LHC::*loopstepHandler)(int&))
    {
        loopStepHandler.attach(lhc, loopstepHandler);
    }

    template<class LHC>
    void setLoopStopHandler(LHC* lhc, bool (LHC::*loopstopHandler)(int&))
    {
        loopStopHandler.attach(lhc, loopstopHandler);
    }

    Timer& getTimer()
    {
        return m_timer;
    }

    virtual NbiotLoopId getLoopId() const
    {
        return id;
    }

private:
    LoopClient();
    LoopClient(LoopClient&);
    LoopClient& operator = (LoopClient&);

    NbiotResult m_result;
    int m_loopCtrl;
    Timer m_timer;

    FP<bool, int&> loopStartHandler;
    FP<bool, int&> loopStepHandler;
    FP<bool, int&> loopStopHandler;

    const NbiotLoopId id;
};


}
#endif // NBIOTLOOPCLIENT_H

