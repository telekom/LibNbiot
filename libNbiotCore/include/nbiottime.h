#ifndef NBIOTTIME_H
#define NBIOTTIME_H

#include "nbiottime_t.h"

class NbiotTime
{
public:
    NbiotTime(): data(new NbiotTime_t),ticksPerMs(1),msPerTick(1),ticksCurrentMilli(0), tickLock(false)
    {
        data->timestamp = 0;
        data->millis = 0;
    }
    virtual ~NbiotTime()
    {
        delete data;
    }

    void delay(unsigned short ms);
    void setTickFrequency(unsigned long ticks);
    unsigned short getMinDelay() const { return ((0 == msPerTick)? 1 : msPerTick); }
    long long getMillis() const;
    void addMillis(unsigned long ms);
    time_t getTime();
    void setTime(time_t t);

    void tick();

private:
    NbiotTime(const NbiotTime&);
    NbiotTime& operator=(const NbiotTime&);

    NbiotTime_t* data;

    unsigned short ticksPerMs;
    unsigned short msPerTick;
    unsigned short ticksCurrentMilli;
    bool tickLock;


    static const unsigned short ONE_KHZ = 1000;

};


#endif // NBIOTTIME_H
