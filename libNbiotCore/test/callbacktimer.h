//
// Created by Yassine Amraue on 19.04.18.
//

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
