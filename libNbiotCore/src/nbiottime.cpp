/* ========================================================================
 * LibNbiot: nbiottime.cpp
 *
 * Copyright (c) 2018, Edgar Hindemith, Yassine Amraue, Thorsten
 * Krautscheid, Kolja Vornholt, T-Systems International GmbH
 * contact: libnbiot@t-systems.com, opensource@telekom.de
 *
 * This file is distributed under the conditions of the Apache License,
 * Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * For details see the file LICENSE at the toplevel.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either expressed or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ========================================================================
*/

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
