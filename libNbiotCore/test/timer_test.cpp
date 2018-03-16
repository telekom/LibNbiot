//
// Created by Yassine Amraue on 25.01.18.
//

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
