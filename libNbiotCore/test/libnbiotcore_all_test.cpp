/* ========================================================================
 * LibNbiot: libnbiotcore_all_test.cpp
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