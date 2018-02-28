#include <itimer.h>
#include "stmevent.h"
#include "state.h"
#include "abstractstatemachine.h"

AbstractStateMachine::AbstractStateMachine(ITimer *t) :
    currentState(nullptr),
    initialState(nullptr),
    stateTimeout(t)
{
}

AbstractStateMachine::~AbstractStateMachine()
{
    if(nullptr != stateTimeout)
    {
        delete stateTimeout;
    }
}

bool AbstractStateMachine::handleEvent(StmEvent& ev) const
{
    bool ret = false;
    if(nullptr != currentState)
    {
        ret = currentState->handleEvent(ev);
    }
    else
    {
        ev.setEventState(StmEvent::Ignored);
    }
    return ret;
}
