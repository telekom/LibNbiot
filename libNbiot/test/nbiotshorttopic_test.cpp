/* ========================================================================
 * LibNbiot: nbiotloop_test.cpp
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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "nbiottopic.h"

using namespace nbiot;

TEST(NbiotShortTopicConstruction, EmptyTopic) {
    NbiotShortTopic *st = new NbiotShortTopic();

    EXPECT_NE(st, nullptr);
    EXPECT_EQ(st->shortName[0], '\0');
    EXPECT_EQ(st->shortName[1], '\0');

    delete st;
}

TEST(NbiotShortTopicConstruction, TopicFromCharArray) {
    char bytes[2] = {'a', 'b'};
    NbiotShortTopic *st = new NbiotShortTopic(bytes);

    EXPECT_NE(st, nullptr);
    EXPECT_EQ(st->shortName[0], 'a');
    EXPECT_EQ(st->shortName[1], 'b');

    delete st;
}

//TEST(NbiotShortTopicConstruction, NullptrConstruction) {
//    char* bytes = nullptr;
//    NbiotShortTopic *st = new NbiotShortTopic(bytes);
//
//    EXPECT_NE(st, nullptr);
//
//    delete st;
//}

TEST(NbiotShortTopicConstruction, TopicFromChars) {
    char b0 = 'a';
    char b1 = 'b';

    NbiotShortTopic *st = new NbiotShortTopic(b0, b1);

    EXPECT_NE(st, nullptr);
    EXPECT_EQ(st->shortName[0], 'a');
    EXPECT_EQ(st->shortName[1], 'b');

    delete st;
}

TEST(NbiotShortTopicConstruction, CopyTopic) {
    char bytes[2] = {'a', 'b'};

    NbiotShortTopic st1(bytes);
    NbiotShortTopic *st2 = new NbiotShortTopic(st1);

    EXPECT_NE(st2, nullptr);
    EXPECT_EQ(st2->shortName[0], 'a');
    EXPECT_EQ(st2->shortName[1], 'b');
}

TEST(NbiotShortTopicOperator, AssignToTopic) {
    char bytes[2] = {'a', 'b'};

    NbiotShortTopic st1(bytes);
    NbiotShortTopic st2;

    EXPECT_EQ(st2.shortName[0], '\0');
    EXPECT_EQ(st2.shortName[1], '\0');

    st2 = st1;

    EXPECT_EQ(st2.shortName[0], 'a');
    EXPECT_EQ(st2.shortName[1], 'b');
}

TEST(NbiotShortTopicOperator, AssignCharArrayToTopic) {
    char bytes[2] = {'a', 'b'};

    NbiotShortTopic st1;

    EXPECT_EQ(st1.shortName[0], '\0');
    EXPECT_EQ(st1.shortName[1], '\0');

    st1 = bytes;

    EXPECT_EQ(st1.shortName[0], 'a');
    EXPECT_EQ(st1.shortName[1], 'b');
}

//TEST(NbiotShortTopicOperator, NullptrAssignment) {
//    char* bytes = nullptr;
//
//    NbiotShortTopic *st1 = new NbiotShortTopic(bytes);
//
//    *st1 = bytes;
//
//    EXPECT_NE(st1, nullptr);
//}

TEST(NbiotShortTopicOperator, EqualityOperator) {
    char bytes[2] = {'a', 'b'};
    const char c_bytes[2] = {'a', 'b'};

    NbiotShortTopic st1(bytes);

    EXPECT_TRUE(st1 == bytes);
    EXPECT_TRUE(st1 == c_bytes);
}

//TEST(NbiotShortTopicOperator, EqualityToNullptr) {
//    char *bytes = nullptr;
//    const char *c_bytes = nullptr;
//
//    NbiotShortTopic st1(bytes);
//
//    EXPECT_TRUE(st1 == bytes);
//    EXPECT_TRUE(st1 == c_bytes);
//}