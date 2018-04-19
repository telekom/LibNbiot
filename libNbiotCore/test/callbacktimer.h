/* ========================================================================
 * LibNbiot: nbiotdebug.cpp
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

#ifndef LIBNBIOTOS_CALLBACKTIMER_H
#define LIBNBIOTOS_CALLBACKTIMER_H

#include <chrono>
#include <atomic>
#include <functional>
#include <thread>

class RepeatedTimer
{
public:
    RepeatedTimer(int interval, std::function<void(void)> func)
            :_running(false), _func(func), _interval(interval)
    {}

    ~RepeatedTimer() {
        if( _running.load(std::memory_order_acquire) ) {
            stop();
        };
    }

    void stop()
    {
        _running.store(false, std::memory_order_release);
        if( _thd.joinable() )
            _thd.join();
    }

    void start()
    {
        if( _running.load(std::memory_order_acquire) ) {
            stop();
        };
        _running.store(true, std::memory_order_release);
        _thd = std::thread([this]()
                           {
                               while (_running.load(std::memory_order_acquire)) {
                                   _func();
                                   std::this_thread::sleep_for(
                                           std::chrono::milliseconds(_interval));
                               }
                           });
    }

    bool is_running() const noexcept {
        return ( _running.load(std::memory_order_acquire) &&
                 _thd.joinable() );
    }

private:
    std::atomic<bool> _running;
    std::thread _thd;
    std::function<void(void)> _func;
    int _interval;
};

#endif //LIBNBIOTOS_CALLBACKTIMER_H
