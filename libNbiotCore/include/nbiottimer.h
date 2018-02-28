#ifndef NBIOTTIMER_H
#define NBIOTTIMER_H

#include "itimer.h"

namespace nbiot {

class Timer:public ITimer
{
public:
    Timer(): start_ms(0), end_ms(0) {}
    Timer(unsigned long end): start_ms(0), end_ms(0) { start(end); }
    virtual ~Timer() {}

    virtual void start();
    virtual void start(unsigned long end);
    virtual void setTime(unsigned long end);
    virtual unsigned long getTime() const;
    virtual unsigned long expired() const;
    virtual unsigned long remaining() const;
    virtual void clear();
    virtual bool valid() const;
    bool _expired() { return (0 == remaining()); }
    void countdown_ms (unsigned long ms) { start(ms); }
    void countdown (unsigned long s) { start(ms_per_sec * s); }
    int left_ms()
    {
        int ret = static_cast<int>(remaining());
        return ret;
    }

private:
    long long start_ms;
    long long end_ms;

    static const unsigned long ms_per_sec = 1000;
};

}


#endif // NBIOTTIMER_H
