//
// Created by Yassine Amraue on 16.01.18.
//

#include "gmock/gmock.h"

#include <thread>
#include "libnbiotcore.h"


bool running = true;

void invoke_tick() {
    std::chrono::duration<long long, std::ratio<1LL, 1000LL> > sleep_time = std::chrono::milliseconds(1);

    while (running) {
        tick();
        std::this_thread::sleep_for(sleep_time);
    }
}

int main(int argc, char **argv) {
    std::thread time_update(invoke_tick);

    int rc = 0;

    ::testing::InitGoogleMock(&argc, argv);
    //::testing::InitGoogleTest(&argc, argv);

    rc = RUN_ALL_TESTS();

    running = false;
    time_update.join();
    return rc;
}