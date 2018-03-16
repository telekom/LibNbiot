#ifndef CHOICE_H
#define CHOICE_H

#include "actionptr.h"
#include "istatemachinenode.h"

class StmEvent;
class Transition;
class AbstractStateMachine;

class Choice : public IStateMachineNode
{
public:
    Choice();
    virtual ~Choice() {}

    virtual void execute(StmEvent& e);

    virtual NodeType type() const { return NodeTypeGuard; }

    template <class DerivedStm>
    void setCondition(DerivedStm *stm, bool (DerivedStm::*action)(const StmEvent&)) { condition.setup(stm, action); }
    void setTransitions(Transition* t0, Transition* t1) { target0 = t0; target1 = t1; }

protected:
    virtual bool action(const StmEvent& e) { return (hasAction()) ? condition(e) : false; }
    virtual bool hasAction() const { return condition.valid(); }

private:
    Choice(const Choice&);
    Choice& operator = (const Choice&);

    Transition* target0;
    Transition* target1;
    ActionPtr condition;
};

#endif // CHOICE_H
