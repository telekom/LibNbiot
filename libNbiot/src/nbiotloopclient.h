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

