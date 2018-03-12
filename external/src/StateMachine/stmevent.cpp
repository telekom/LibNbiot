#include "stmevent.h"

StmEvent::StmEvent() :
    m_eventType(Invalid),
    m_eventState(Pending),
    m_eventId(-1)
{
}

StmEvent::StmEvent(EventType t) :
    m_eventType(t),
    m_eventState(Pending),
    m_eventId(-1)
{
}

StmEvent::~StmEvent()
{
}

StmEvent::StmEvent(const StmEvent& other) :
    m_eventType(other.m_eventType),
    m_eventState(other.m_eventState),
    m_eventId(other.m_eventId)
{
}

StmEvent& StmEvent::operator = (const StmEvent& other)
{
    m_eventType = other.m_eventType;
    m_eventState = other.m_eventState;
    m_eventId = other.m_eventId;
    return *this;
}

void StmEvent::setCustomEventType(int type)
{
    if((CustomMin <= type) && (CustomMax >= type))
    {
        m_eventType = type;
    }
}

const char* StmEvent::eventStateToString() const
{
    switch (m_eventState)
    {
        case Pending:
            return "pending";
        case Accepted:
            return "accepted";
        case Ignored:
            return "ignored";
        case Aborted:
            return "aborted";
        case Handled:
            return "handled";
        default:
            return "unknown";
    }
}
