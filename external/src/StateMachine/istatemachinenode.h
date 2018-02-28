#ifndef ISTATEMACHINENODE_H
#define ISTATEMACHINENODE_H

class StmEvent;

class IStateMachineNode
{
public:
    IStateMachineNode() {}
    virtual ~IStateMachineNode() {}

    enum NodeType
    {
        NodeTypeState,
        NodeTypeStateMachine,
        NodeTypeTransition,
        NodeTypeGuard
    };

    virtual NodeType type() const = 0;

protected:
    virtual bool action(const StmEvent&) = 0;
    virtual bool hasAction() const = 0;

private:
    IStateMachineNode(const IStateMachineNode&);
    IStateMachineNode& operator = (const IStateMachineNode&);
};

#endif // ISTATEMACHINENODE_H
