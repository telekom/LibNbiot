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

#include "callbacktimer.h"

using namespace nbiot;

class TimerTest : public ::testing::Test {
public:
    void SetUp() {
        tick_timer = new RepeatedTimer(1, tick);
    }

    void start_tick_timer() {
        tick_timer->start();
    }

    void pass_ms(unsigned int ms) {
        for (int i = 0; i < ms; ++i) {
            tick();
        }
    }

    void pass_sec(int s) {
        for (int i = 0; i < 1000 * s; ++i) {
            tick();
        }
    }

    void pass_min(int min) {
        for (int i = 0; i < 60 * 1000 * min; ++i) {
            tick();
        }
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

TEST(TimerSetupTest, SettingTime) {
    Timer t;
    t.setTime(10);
    EXPECT_EQ(10, t.getTime());
    t.setTime(0);
    EXPECT_EQ(0, t.getTime());
    t.setTime(-10);
    EXPECT_EQ(-10, t.getTime());
    t.setTime((unsigned long) 0 - 1);
    EXPECT_EQ((unsigned long) 0 - 1, t.getTime());
}

TEST_F(TimerTest, StartTimer) {
    Timer t;
    t.setTime(100);
    t.start();
    EXPECT_NE(t.remaining(), 0);
    pass_ms(100);
    EXPECT_EQ(t.remaining(), 0);
}

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