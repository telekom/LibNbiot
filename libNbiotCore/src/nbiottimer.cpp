#include "nbiottime.h"
#include "nbiotcoreapp.h"

#include "nbiottimer.h"

namespace nbiot
{

void Timer::start()
{
    start_ms = NbiotCoreApp::getInstance().getTimeInstance().getMillis();
    end_ms += start_ms;
}

void Timer::start(unsigned long end)
{
    start_ms = NbiotCoreApp::getInstance().getTimeInstance().getMillis();
    end_ms = start_ms + end;
}

void Timer::setTime(unsigned long end)
{
    end_ms = end;
}

unsigned long Timer::getTime() const
{
    return ((0 != start_ms) && (start_ms <= end_ms))?
                (static_cast<unsigned long>(end_ms - start_ms)) :
                (static_cast<unsigned long>(end_ms));
}

unsigned long Timer::expired() const
{
    return (0 == start_ms)?
                0 :
                (0 == remaining())?
                    (static_cast<unsigned long>(end_ms - start_ms)) :
                    (static_cast<unsigned long>(NbiotCoreApp::getInstance().getTimeInstance().getMillis() - start_ms));
}

unsigned long Timer::remaining() const
{
    return ((0 == end_ms) || (NbiotCoreApp::getInstance().getTimeInstance().getMillis() > end_ms))?
                0 :
                static_cast<unsigned long>(end_ms - NbiotCoreApp::getInstance().getTimeInstance().getMillis());
}

void Timer::clear()
{
    start_ms = 0;
    end_ms = 0;
}

bool Timer::valid() const
{
    return ((0 != start_ms) && (0 != end_ms));
}

}
