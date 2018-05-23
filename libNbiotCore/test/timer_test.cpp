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

#include <libnbiotcore.h>
#include <nbiottimer.h>

#include "repeatedtimer.h"
#include <limits.h>

using namespace nbiot;

class TimerTest : public ::testing::Test {
public:
    void SetUp() {
        setTime(0);
        tick_timer = new RepeatedTimer(1, tick);
    }

    void start_tick_timer() {
        tick_timer->start();
    }

    void pass_ms(unsigned long ms) {
        updateMillis(ms);
    }

    void pass_sec(unsigned long s) {
        updateMillis(1000 * s);
    }

    void pass_min(unsigned long min) {
        updateMillis(60 * 1000 * min);
    }

    void TearDown() {
        tick_timer->stop();
        delete tick_timer;
    }

    RepeatedTimer *tick_timer;
};

TEST(TimerSetupTest, StandardConstructor) {
    Timer t;
    EXPECT_FALSE(t.valid());
}

TEST_F(TimerTest, SettingTime) {
    Timer t;
    t.setTime(10);
    EXPECT_EQ(10, t.getTime());
    t.setTime(0);
    EXPECT_EQ(0, t.getTime());
    t.setTime(-10);
    EXPECT_EQ(-10, t.getTime());
}

TEST_F(TimerTest, StartTimer) {
    Timer t;
    t.setTime(100);
    t.start();
    EXPECT_NE(t.remaining(), 0);
    pass_ms(100);
    EXPECT_EQ(t.remaining(), 0);
}

//TEST_F(TimerTest, StartTimerWithMaxULONG) {
//    // Test max unsigned long
//
//    Timer t;
//
//    t.setTime(ULONG_MAX);
//    EXPECT_EQ(ULONG_MAX, t.getTime())
//                        << "There was an error setting the time to ULONG_MAX.";
//
//    t.start();
//    EXPECT_NE(0, t.remaining())
//                        << "Timer.remaining() should not return 0 here, because timer was set to ULONG_MAX "
//                        << "and no time passed yet";
//    pass_ms(ULONG_MAX);
//    EXPECT_EQ(0, t.remaining())
//                        << "Timer.remaining() should be zero here, because ULONG_MAX passed.";
//}

//TEST_F(TimerTest, StartTimerWithMillisAtMaxLONGLONG) {
//    // Test max unsigned long
//
//    Timer t;
//    unsigned short time = 1000;
//
//    setTime(0);
//    updateMillis(LONG_LONG_MAX);
//    EXPECT_EQ(LONG_LONG_MAX, getMillis());
//
//    t.setTime(time);
//    EXPECT_EQ(time, t.getTime())
//                        << "There was an error setting the time to LONG_MAX.";
//
//    t.start();
//    EXPECT_EQ(time, t.remaining())
//                        << "Timer.remaining() should equal time here, because timer was set to time "
//                        << "and no time passed yet";
//    pass_ms(time);
//    EXPECT_EQ(0, t.remaining())
//                        << "Timer.remaining() should be zero here, because time passed.";
//}

TEST_F(TimerTest, StartTimerWithTimeSetting) {
    Timer t;
    t.setTime(10);

    t.start(100);
    EXPECT_NE(t.remaining(), 0);
    pass_ms(100);
    EXPECT_EQ(t.remaining(), 0);
}

TEST_F(TimerTest, ConstructAndStart) {
    Timer t(100);
    EXPECT_NE(t.remaining(), 0);
    pass_ms(100);
    EXPECT_EQ(t.remaining(), 0);
}

TEST_F(TimerTest, ClearTime) {
    tick();

    Timer t;

    EXPECT_FALSE(t.valid());
    EXPECT_EQ(0, t.expired());
    EXPECT_EQ(0, t.remaining());

    t.start(1000);

    EXPECT_TRUE(t.valid());
    EXPECT_EQ(0, t.expired());
    EXPECT_NE(0, t.remaining());

    pass_sec(1);

    EXPECT_NE(0, t.expired());
    EXPECT_EQ(0, t.remaining());

    t.setTime(100);
    t.clear();

    EXPECT_FALSE(t.valid());
    EXPECT_EQ(0, t.expired());
    EXPECT_EQ(0, t.remaining());
}