/******************************************************************************
 * File: transition.h
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
#ifndef TRANSITION_H
#define TRANSITION_H

#include "actionptr.h"
#include "stmevent.h"
#include "istatemachinenode.h"

class AbstractStateMachine;

class Transition : public IStateMachineNode
{
public:
    Transition();
    virtual ~Transition() {}

    virtual void trigger(StmEvent& e);

    virtual NodeType type() const { return NodeTypeTransition; }

    template <class DerivedStm>
    void setAction(DerivedStm *stm, bool (DerivedStm::*action)(const StmEvent&)) { trAction.setup(stm, action); }
    void setTarget(IStateMachineNode* t) { target = t; }
    void setEventType(int type) { eventType = type; }

    bool operator == (const StmEvent::EventType type) const { return (type == eventType); }

protected:
    virtual bool action(const StmEvent& e) { return (hasAction()) ? trAction(e) : false; }
    virtual bool hasAction() const { return trAction.valid(); }

private:
    Transition(const Transition&);
    Transition& operator = (const Transition&);

    IStateMachineNode* target;
    ActionPtr trAction;
    int eventType;
};

#endif // TRANSITION_H
