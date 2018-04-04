/******************************************************************************
 * File: stmevent.cpp
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

#include "stmevent.h"

StmEvent::StmEvent() :
    m_eventType(Invalid),
    m_eventState(Pending),
    m_eventId(-1)
{
}

StmEvent::StmEvent(EventType t) :
    m_eventType(t),
    m_eventState(Pending),
    m_eventId(-1)
{
}

StmEvent::~StmEvent()
{
}

StmEvent::StmEvent(const StmEvent& other) :
    m_eventType(other.m_eventType),
    m_eventState(other.m_eventState),
    m_eventId(other.m_eventId)
{
}

StmEvent& StmEvent::operator = (const StmEvent& other)
{
    m_eventType = other.m_eventType;
    m_eventState = other.m_eventState;
    m_eventId = other.m_eventId;
    return *this;
}

void StmEvent::setCustomEventType(int type)
{
    if((CustomMin <= type) && (CustomMax >= type))
    {
        m_eventType = type;
    }
}

const char* StmEvent::eventStateToString() const
{
    switch (m_eventState)
    {
        case Pending:
            return "pending";
        case Accepted:
            return "accepted";
        case Ignored:
            return "ignored";
        case Aborted:
            return "aborted";
        case Handled:
            return "handled";
        default:
            return "unknown";
    }
}
