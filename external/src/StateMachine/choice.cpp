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
