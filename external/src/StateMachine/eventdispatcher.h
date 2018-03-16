#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <list.h>
#include "stmevent.h"

class AbstractStateMachine;

class EventDispatcher
{
public:
    explicit EventDispatcher(AbstractStateMachine* stm);
    virtual ~EventDispatcher();

    void pushEvent(StmEvent& e) { m_events.append(e); }
    void postEvent(StmEvent& e) { m_pendingEvents.append(e); }
    void dispatch();
    void setTimeout(unsigned long ms);

private:
    EventDispatcher();
    EventDispatcher(EventDispatcher&);
    EventDispatcher& operator = (EventDispatcher&);

    AbstractStateMachine* m_stm;

    util::list<StmEvent> m_events;
    util::list<StmEvent> m_pendingEvents;
    int m_eventId;
};

#endif // EVENTDISPATCHER_H
