#ifndef ITIMER_H
#define ITIMER_H

class ITimer
{
public:
    virtual ~ITimer() {}

    virtual void start(unsigned long) = 0;
    virtual unsigned long expired() const = 0;
    virtual unsigned long remaining() const = 0;
    virtual void clear() = 0;
    virtual bool valid() const = 0;
};

#endif // ITIMER_H

