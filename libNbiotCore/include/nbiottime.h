/* ========================================================================
 * LibNbiot: nbiottime.h
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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ========================================================================
*/

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
