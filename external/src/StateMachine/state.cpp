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
