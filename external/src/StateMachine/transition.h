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
