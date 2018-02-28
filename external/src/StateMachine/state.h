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
