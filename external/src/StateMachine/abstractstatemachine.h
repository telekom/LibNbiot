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
