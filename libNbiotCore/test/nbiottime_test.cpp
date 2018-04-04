/* ========================================================================
 * LibNbiot: nbiottime_test.cpp
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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <nbiottime.h>

#define ONE_KHZ 1000

// Tests that the standard constructor creates an object with timestamp 0.

TEST(NbiotTimeTest, ConstructStandardObject) {
    NbiotTime nbiot_time;
    EXPECT_EQ(0, nbiot_time.getTime());
    EXPECT_EQ(0, nbiot_time.getMillis());
}

// Tests that time can be set properly

TEST(NbiotTimeTest, SettingTime) {
    NbiotTime nbiot_time;
    time_t t = time(0);

    nbiot_time.setTime(t);
    EXPECT_EQ(t, nbiot_time.getTime());
}

// Tests that getMillis returns the correct time

TEST(NbiotTimeTest, GetMillis) {
    NbiotTime nbiot_time;
    time_t t = time(0);

    nbiot_time.setTime(t);
    EXPECT_EQ(ONE_KHZ * t, nbiot_time.getMillis());
}

// Tests that Milliseconds are correctly added and returned by getMillis

TEST(NbiotTimeTest, AddingMillis) {
    NbiotTime nbiot_time;
    time_t t = time(0);

    nbiot_time.setTime(t);
    EXPECT_EQ(t, nbiot_time.getTime());

    nbiot_time.addMillis(10);
    EXPECT_EQ(t * ONE_KHZ + 10, nbiot_time.getMillis());
}

// Tests that tick() correctly updates millis and timestamp according to the set frequency

TEST(NbiotTimeTest, Tick) {
    NbiotTime nbiot_time;
    unsigned short ticksPerMs;

    // Set tick frequency to default and call tick until exactly one millisecond is updated

    nbiot_time.setTickFrequency(0);
    ticksPerMs = CLOCKS_PER_SEC / ONE_KHZ;

    for (int i = 0; i < ticksPerMs; ++i) {
        nbiot_time.tick();
    }

    ASSERT_EQ(1, nbiot_time.getMillis())
                                << "Exactly one millisecond should have passed if "
                                << "tick frequency is set to default";

    // Set tick frequency so that exactly one tick updates one millisecond

    nbiot_time.setTickFrequency(1000);

    nbiot_time.tick();

    ASSERT_EQ(2, nbiot_time.getMillis())
                                << "One tick should update exactly one millisecond if "
                                << "tick frequency is ONE_KHZ";

    // Set tick frequency so that one tick updates more than one millisecond to test behaviour
    // when tick frequency is smaller than ONE_KHZ

    nbiot_time.setTickFrequency(10);

    nbiot_time.tick();

    ASSERT_EQ(102, nbiot_time.getMillis())
                                << "One tick should update 100 milliseconds if"
                                << "tick frequency is 10";

    // Call tick until milliseconds are greater than 1000, so that timestamp has to be updated

    for (int i = 0; i < 9; ++i) {
        nbiot_time.tick();
    }

    ASSERT_EQ(1002, nbiot_time.getMillis());
    ASSERT_EQ(1, nbiot_time.getTime())
                                << "Timestamp wasn't updated although milliseconds passed 1000";
}

// TODO: Test delay
TEST(NbiotTimeTest, Delay) {
}


