/* ========================================================================
 * LibNbiot: timer_test.cpp
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

#include <nbiottimer.h>
#include <chrono>

using namespace nbiot;
using namespace std::chrono;

TEST(TimerTest, StandardConstructor) {
    Timer t;
    EXPECT_FALSE(t.valid());
}

TEST(TimerTest, SettingTime) {
    Timer t;
    t.setTime(10);
    EXPECT_EQ(10, t.getTime());
    t.setTime(0);
    EXPECT_EQ(0, t.getTime());
    t.setTime(-10);
    EXPECT_EQ(-10, t.getTime());
}

TEST(TimerTest, StartTimer) {
    Timer t;
    t.setTime(100);
    high_resolution_clock::time_point t1, t2;

    t.start();
    t1 = high_resolution_clock::now();
    while (t.remaining() > 0) {
    }
    t2 = high_resolution_clock::now();
    EXPECT_NEAR(0.1, duration_cast<duration<double>>(t2 - t1).count(), 0.1);
}

TEST(TimerTest, StartTimerWithTimeSetting) {
    Timer t;
    t.setTime(10);

    high_resolution_clock::time_point t1, t2;
    t.start(100);
    t1 = high_resolution_clock::now();
    while (t.remaining() > 0) {
    }
    t2 = high_resolution_clock::now();
    EXPECT_NEAR(0.1, duration_cast<duration<double>>(t2 - t1).count(), 0.1);
}

TEST(TimerTest, ConstructAndStart) {
    high_resolution_clock::time_point t1, t2;
    Timer t(100);
    t1 = high_resolution_clock::now();
    while (t.remaining() > 0) {
    }
    t2 = high_resolution_clock::now();
    EXPECT_NEAR(0.1, duration_cast<duration<double>>(t2 - t1).count(), 0.1);
}

TEST(TimerTest, ClearTime) {
    Timer t;

    EXPECT_FALSE(t.valid());
    EXPECT_EQ(0, t.expired());
    EXPECT_EQ(0, t.remaining());

    t.start(100);

    EXPECT_GT(t.remaining(), 0);
    EXPECT_TRUE(t.valid());
    EXPECT_NE(0, t.remaining());

    while (0 < t.remaining()) {};
    EXPECT_NE(0, t.expired());

    t.setTime(100);
    t.clear();

    EXPECT_FALSE(t.valid());
    EXPECT_EQ(0, t.expired());
    EXPECT_EQ(0, t.remaining());
}
