/******************************************************************************
 * File: eventdispatcher.cpp
 * Author: Edgar Hindemith
 * This file is part of the 'Simple Statemachine for Embedded Systems'
 * see https://github.com/edgar4k/StateMachine
 *
 * Copyright (C) 2016,2018 Edgar Hindemith
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
/******************************************************************************
 * Changelog
 * 2017 Edgar Hindemith
 *  added setTimeout() implementation and check of state-timeout in dispatch()
 *****************************************************************************/

#include <itimer.h>
#include "abstractstatemachine.h"
#include "state.h"
#include "eventdispatcher.h"


EventDispatcher::EventDispatcher(AbstractStateMachine* stm):
    m_stm(stm),
    m_events(),
    m_pendingEvents(),
    m_eventId(0)
{
}

EventDispatcher::~EventDispatcher()
{
}

void EventDispatcher::dispatch()
{
    // check for a timeout first
    if((nullptr != m_stm) && (nullptr != m_stm->getStateTimer()))
    {
        if((m_stm->getStateTimer()->valid()) && (0 == m_stm->getStateTimer()->remaining()))
        {
            StmEvent timeout = StmEvent(StmEvent::Timer);
            m_events.append(timeout);
            // reset the timer: timer will be cleared at the next state-change, otherwise we will come back here
            m_stm->getStateTimer()->start( m_stm->getStateTimer()->expired() );
        }
    }

    //while(!m_events.isEmpty())
    while(0 < m_events.count())
    {
        if(nullptr != m_stm)
        {
            StmEvent ev = m_events.take_front();
            ev.setEventState(StmEvent::Pending);
            if(-1 == ev.getEventId())
            {
                m_eventId++;
                if(0 >= m_eventId)
                {
                    m_eventId = 1;
                }
                ev.setEventId(m_eventId);
            }
            m_stm->handleEvent(ev);
        }
        else
        {
            m_events.pop();
        }
    }

    while(!m_pendingEvents.isEmpty())
    {
        if(nullptr != m_stm)
        {
            StmEvent ev = m_pendingEvents.take_front();
            m_events.append(ev);
        }
        else
        {
            m_pendingEvents.pop();
        }
    }
}

void EventDispatcher::setTimeout(unsigned long ms)
{
    if((nullptr != m_stm) && (nullptr != m_stm->getStateTimer()))
    {
        m_stm->getStateTimer()->start(ms);
    }
}
