/******************************************************************************
 * File: abstractstatemachine.h
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
 *  added ITimer-pointer to the state machine
 *****************************************************************************/
#ifndef ABSTRACTSTATEMACHINE_H
#define ABSTRACTSTATEMACHINE_H

#include "istatemachinenode.h"

class ITimer;
class State;
class StmEvent;

class AbstractStateMachine : public IStateMachineNode
{
public:
    explicit AbstractStateMachine(ITimer* t);
    virtual ~AbstractStateMachine();

    virtual NodeType type() const { return NodeTypeStateMachine; }

    void setCurrentState(State* s) { currentState = s; }
    State* getCurrentState() const { return currentState; }

    ITimer* getStateTimer() const { return stateTimeout; }

    virtual void startStateMachine() = 0;
    virtual void setUpStateMachine() = 0;

    bool handleEvent(StmEvent& ev) const;

protected:
    virtual bool action(const StmEvent&) { return false; }
    virtual bool hasAction() const { return false; }

    void setInitialState(State* s) { initialState = s; }
    State* getInitialState() { return initialState; }

private:
    AbstractStateMachine(const AbstractStateMachine&);
    AbstractStateMachine& operator =(const AbstractStateMachine&);

    State* currentState;
    State* initialState;

    ITimer* stateTimeout;
};

#endif // ABSTRACTSTATEMACHINE_H
