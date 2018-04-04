/******************************************************************************
 * File: choice.cpp
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
#include "transition.h"
#include "choice.h"

Choice::Choice() :
    target0(nullptr),
    target1(nullptr),
    condition()
{

}

void Choice::execute(StmEvent& e)
{
    if(hasAction())
    {
        if(action(e))
        {
            if(nullptr != target0)
            {
                target0->trigger(e);
            }
            else
            {
                e.setEventState(StmEvent::Aborted);
            }
        }
        else
        {
            if(nullptr != target1)
            {
                target1->trigger(e);
            }
            else
            {
                e.setEventState(StmEvent::Aborted);
            }
        }
    }
    else
    {
        e.setEventState(StmEvent::Aborted);
    }
}
