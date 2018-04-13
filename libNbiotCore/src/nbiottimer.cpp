/* ========================================================================
 * LibNbiot: nbiottimer.cpp
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

void Timer::start(unsigned short end) {
    start(static_cast<unsigned long> (end));
}

    void Timer::start(int end) {
        start(static_cast<unsigned long> (end));
    }

void Timer::setTime(unsigned long end)
{
    end_ms = static_cast<long long>(end);
}

void Timer::setTime(int end)
{
    end_ms = static_cast<long long>(end);
}

void Timer::setTime(unsigned short end)
{
    end_ms = static_cast<long long>(end);
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
    long long current_millis = NbiotCoreApp::getInstance().getTimeInstance().getMillis();
    return ((0 == end_ms) || (current_millis >= end_ms))?
                0 :
                static_cast<unsigned long>(end_ms - current_millis);
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
