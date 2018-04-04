/******************************************************************************
 * File: stmevent.h
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
#ifndef STMEVENT_H
#define STMEVENT_H


class StmEvent
{
public:
    enum EventType
    {
        Invalid = 0,                            // invalid event
        Timer = 1,                              // timer event
        KeyPress = 2,                           // key pressed
        KeyRelease = 3,                         // key released
        CustomMin = 256,
        CustomMax = 0x7fffffff
    };
    enum EventState
    {
        Pending,
        Accepted,
        Ignored,
        Aborted,
        Handled
    };

    StmEvent();
    explicit StmEvent(EventType t);
    virtual ~StmEvent();
    StmEvent(const StmEvent& other);
    StmEvent& operator = (const StmEvent& other);

    void setCustomEventType(int type);
    EventType getEventType() const { return static_cast<EventType>(m_eventType); }

    void setEventState(EventState s) { m_eventState = s; }
    EventState getEventState() const { return m_eventState; }

    void setEventId(int id) { m_eventId = id; }
    int getEventId() const { return m_eventId; }

    const char* eventStateToString() const;

    bool operator == (const StmEvent& other) const { return other.m_eventType == m_eventType; }

private:
    int m_eventType;
    EventState m_eventState;
    int m_eventId;
};

#endif // STMEVENT_H
