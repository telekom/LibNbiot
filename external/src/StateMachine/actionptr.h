#ifndef ACTIONPTR_H
#define ACTIONPTR_H

class StmEvent;
class AbstractStateMachine;

class ActionPtr
{
public:
    ActionPtr() :
        stateMachine(nullptr)
    {}

    /** Add a callback function
     *  @param stm - Address of the initialized statemachine
     *  @param member - Address of the action member
     */
    template <class DerivedStm>
    void setup(DerivedStm *stm, bool (DerivedStm::*action)(const StmEvent&))
    {
        stateMachine = static_cast<AbstractStateMachine *>(stm);
        stmAction = (bool (AbstractStateMachine::*)(const StmEvent&))(action);
        return;
    }

    /** Invoke the attached function
      *  @param arg - An argument that is passed into the function handler that is called
      *  @return The return from the function hanlder called by this class
      */
     bool operator()(const StmEvent& ev) const
     {
         if( nullptr != stateMachine ) {
             return (stateMachine->*stmAction)(ev);
         }
         return false;
     }

    /** Determine if an callback is currently hooked
     *  @return 1 if a method is hooked, 0 otherwise
     */
    bool valid() const
    {
        return static_cast<bool>(stateMachine);
    }

    /** Release a function from the callback hook
     */
    void remove()
    {
        stateMachine = nullptr;
    }

private:

    AbstractStateMachine *stateMachine;

    bool (AbstractStateMachine::*stmAction)(const StmEvent&);   /*!< Footprint for a member function */
};

#endif // ACTIONPTR_H
