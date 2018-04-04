/******************************************************************************
 * File: state.cpp
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
 *  added reset of state-timeout to State::enter()
 *****************************************************************************/

#include <itimer.h>
#include <list.h>
#include "abstractstatemachine.h"
#include "transition.h"
#include "state.h"

State::State() :
    stateId(0),
    parent(nullptr),
    transitions(nullptr),
    entryAction()
{

}

State::State(IStateMachineNode* p) :
    stateId(0),
    parent(p),
    transitions(nullptr),
    entryAction()
{

}

void State::enter(StmEvent& e)
{
    IStateMachineNode* rootNode = getRoot();
    if(nullptr != rootNode)
    {
        static_cast<AbstractStateMachine*>(rootNode)->setCurrentState(this);
        if(nullptr != static_cast<AbstractStateMachine*>(rootNode)->getStateTimer())
        {
            if(static_cast<AbstractStateMachine*>(rootNode)->getStateTimer()->valid())
            {
                static_cast<AbstractStateMachine*>(rootNode)->getStateTimer()->clear();
            }
        }
        action(e);
        e.setEventState(StmEvent::Handled);
    }
    else
    {
        e.setEventState(StmEvent::Aborted);
    }
}

IStateMachineNode* State::getRoot() const
{
    IStateMachineNode* resultNode = nullptr;
    if(nullptr != parent)
    {
        if(NodeTypeStateMachine == parent->type())
        {
            resultNode = parent;
        }
        else if(NodeTypeState == parent->type())
        {
            resultNode = static_cast<State*>(parent)->getRoot();
        }
    }
    return resultNode;
}

bool State::handleEvent(StmEvent& ev) const
{
    bool result = false;

    for(util::ListIterator<Transition*> it = transitions.cbegin(); it != transitions.cend(); ++it)
    {
        if(*(*it) == ev.getEventType())
        {
            ev.setEventState(StmEvent::Accepted);
            Transition* tr = *it;
            tr->trigger(ev);
            result = true;
            break;
        }
    }
    if(!result)
    {
        ev.setEventState(StmEvent::Ignored);
    }

    return result;
}
