#include "stmevent.h"
#include "state.h"
#include "choice.h"
#include "transition.h"

Transition::Transition() :
    target(nullptr),
    trAction(),
    eventType(StmEvent::Invalid)
{

}

void Transition::trigger(StmEvent& e)
{
    action(e);
    if(nullptr != target)
    {
        if(NodeTypeTransition == target->type())
        {
            static_cast<Transition*>(target)->trigger(e);
        }
        else if (NodeTypeGuard == target->type())
        {
            static_cast<Choice*>(target)->execute(e);
        }
        else if (NodeTypeState == target->type())
        {
            static_cast<State*>(target)->enter(e);
        }
        else
        {
            e.setEventState(StmEvent::Aborted);
        }
    }
    else
    {
        e.setEventState(StmEvent::Aborted);
    }
}

