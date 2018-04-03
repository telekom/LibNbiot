/******************************************************************************
 * File: state.h
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
#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include <list.h>
#include "actionptr.h"
#include "stmevent.h"
#include "istatemachinenode.h"

class Transition;
class AbstractStateMachine;

class State : public IStateMachineNode
{
public:
    State();
    State(IStateMachineNode* p);
    virtual ~State() {}

    virtual void enter(StmEvent& e);

    virtual NodeType type() const { return NodeTypeState; }

    template <class DerivedStm>
    void setAction(DerivedStm *stm, bool (DerivedStm::*action)(const StmEvent&)) { entryAction.setup(stm, action); }

    void setParent(IStateMachineNode* p) { parent = p; }
    IStateMachineNode* getParent() const { return parent; }

    IStateMachineNode* getRoot() const;

    void setId(uint32_t id) { stateId = id; }
    uint32_t getId() const { return stateId; }

    bool handleEvent(StmEvent& ev) const;

    void addTransition(Transition* tr) { transitions.append(tr); }

    bool operator == (const State& other) { return ( (other.stateId == stateId) && (other.parent == parent) ); }

protected:
    virtual bool action(const StmEvent& e) { return (hasAction()) ? entryAction(e) : false; }
    virtual bool hasAction() const { return entryAction.valid(); }

private:
    State(const State&);
    State& operator = (const State&);

    uint32_t stateId;
    IStateMachineNode* parent;

    util::list<Transition*> transitions;

    ActionPtr entryAction;
};

#endif // STATE_H
