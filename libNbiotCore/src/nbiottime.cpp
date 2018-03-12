#include "nbiottime.h"

NbiotTime_t nbiotTimer = {0, 0};

void NbiotTime::setTickFrequency(unsigned long ticks)
{
    if(0 == ticks)
    {
        ticks = CLOCKS_PER_SEC;
    }
    if(ONE_KHZ <= ticks)
    {
        ticksPerMs = ticks / ONE_KHZ;
        msPerTick = 0;
    }
    else
    {
        msPerTick = ONE_KHZ / ticks;
        ticksPerMs = 0;
    }
}

long long NbiotTime::getMillis() const
{
    return (long long)data->timestamp * ONE_KHZ + data->millis;
}

void NbiotTime::addMillis(unsigned long ms)
{
    long long millis = getMillis() + ms;
    data->timestamp = (time_t)(millis / ONE_KHZ);
    data->millis = (unsigned short)(millis % ONE_KHZ);
}

time_t NbiotTime::getTime()
{
    return data->timestamp;
}

void NbiotTime::setTime(time_t t)
{
    // lock tick() to prevent manipulation by ISR
    tickLock = true;

    data->timestamp = t;
    data->millis = 0;

    // unlock tick()
    tickLock = false;
}

void NbiotTime::tick()
{
    if(!tickLock)
    {
        if(0 < msPerTick)
        {
            data->millis += msPerTick;
        }
        else
        {
            if(1 < ticksPerMs)
            {
                ticksCurrentMilli++;
                if(ticksCurrentMilli >= ticksPerMs)
                {
                    data->millis++;
                    ticksCurrentMilli = 0;
                }
            }
            else
            {
                data->millis++;
            }
        }
        if(ONE_KHZ <= data->millis)
        {
            data->timestamp++;
            data->millis -= ONE_KHZ;
        }
    }
}

#pragma GCC optimize ("0")
void NbiotTime::delay(unsigned short ms)
{
    time_t expireSec = data->timestamp;
    unsigned short expireMs = data->millis;
    if(ONE_KHZ <= ms)
    {
        expireSec += ms / ONE_KHZ;
        expireMs += (ms % ONE_KHZ);
    }
    else
    {
        expireMs += ms;
    }
    if(ONE_KHZ <= expireMs)
    {
        expireMs -= ONE_KHZ;
        expireSec++;
    }
    while( expireSec >= data->timestamp )
    {
        if(data->timestamp > expireSec)
        {
            break;
        }
        if( (data->timestamp == expireSec) && (data->millis >= expireMs) )
        {
            break;
        }
    }
}
