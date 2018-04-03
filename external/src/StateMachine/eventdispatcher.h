/******************************************************************************
 * File: eventdispatcher.h
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
 *  added setTimeout() method
 *****************************************************************************/
#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <list.h>
#include "stmevent.h"

class AbstractStateMachine;

class EventDispatcher
{
public:
    explicit EventDispatcher(AbstractStateMachine* stm);
    virtual ~EventDispatcher();

    void pushEvent(StmEvent& e) { m_events.append(e); }
    void postEvent(StmEvent& e) { m_pendingEvents.append(e); }
    void dispatch();
    void setTimeout(unsigned long ms);

private:
    EventDispatcher();
    EventDispatcher(EventDispatcher&);
    EventDispatcher& operator = (EventDispatcher&);

    AbstractStateMachine* m_stm;

    util::list<StmEvent> m_events;
    util::list<StmEvent> m_pendingEvents;
    int m_eventId;
};

#endif // EVENTDISPATCHER_H
