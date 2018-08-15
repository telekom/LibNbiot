/* ========================================================================
 * LibNbiot: urcfilter_test.cpp
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

#include "urcfilterlist.h"

using ::testing::IsNull;

using namespace nbiot;

class UrcFixture : public ::testing::Test {
public:

    virtual void SetUp() {
        number_of_calls = 0;
    };

    virtual void TearDown() {};

    static void do_nothing(const char* c) {
        number_of_calls++;
    };

    static int number_of_calls;
};

int UrcFixture::number_of_calls = 0;

class _UrcCallback {
public:
    virtual ~_UrcCallback() = default;

    virtual void do_nothing(const char *) = 0;
};

class MockUrcCallback : _UrcCallback {
public:
    MOCK_METHOD1(do_nothing, void(
            const char*));
};

// Test object construction from normal string without setting a handler

TEST(UrcFilterTest, ConstructFromString) {
    UrcFilter u("+NSOMNI");
    EXPECT_STREQ("+NSOMNI", u.urc.c_str());
}

// Test object construction from normal string with setting a handler

TEST_F(UrcFixture, ConstructFromStringAndHandler) {
    UrcFilter u("+NSOMNI", do_nothing);
    EXPECT_TRUE(u.handler.attached());
    EXPECT_STREQ("+NSOMNI", u.urc.c_str());
    number_of_calls = 0;
    u.handler("bla");
    EXPECT_EQ(1, number_of_calls);
    number_of_calls = 0;
}

// Tests that copy constructor copies the object and both objects call
// the same callback function

TEST_F(UrcFixture, CopyConstructor) {
    UrcFilter u("+NSOMNI", do_nothing);
    UrcFilter u_copy(u);
    EXPECT_STREQ(u.urc.c_str(), u_copy.urc.c_str());
    number_of_calls = 0;
    u.handler("bla");
    EXPECT_EQ(1, number_of_calls);
    u_copy.handler("bla");
    EXPECT_EQ(2, number_of_calls);
    number_of_calls = 0;
}

// Tests that the assignment operator copies the object and both objects call
// the same callback function

TEST_F(UrcFixture, AssignmentOperator) {
    UrcFilter u("+NSOMNI", do_nothing);
    UrcFilter u_copy;
    u_copy = u;
    EXPECT_STREQ(u.urc.c_str(), u_copy.urc.c_str());
    number_of_calls = 0;
    u.handler("bla");
    EXPECT_EQ(1, number_of_calls);
    u_copy.handler("bla");
    EXPECT_EQ(2, number_of_calls);
    number_of_calls = 0;
}

// Tests that the equality comparision operator returns true the urc filter
// contains the string and false otherwise

TEST(UrcFilterTest, EqComparisionOperator) {
    UrcFilter u("+NSOMNI");
    nbiot::string my_string("+NSOMNI");
    nbiot::string my_false_string("bla");

    EXPECT_TRUE(u == "+NSOMNI");
    EXPECT_FALSE(u == "bla");
    EXPECT_TRUE(u == my_string);
    EXPECT_FALSE(u == my_false_string);
}

// Tests that handlers can be set properly and call the right function exactly once.

TEST_F(UrcFixture, SetHandler) {
    UrcFilter u("+NSOMNI");
    MockUrcCallback UHC;
    u.setHandler(do_nothing);
    number_of_calls = 0;
    u.handler("bla");
    EXPECT_EQ(1, number_of_calls);
    number_of_calls = 0;

    EXPECT_CALL(UHC, do_nothing("bla"))
            .Times(1);

    u.setHandler<MockUrcCallback>(&UHC, &MockUrcCallback::do_nothing);
    u.handler("bla");
}

// Tests that isValid only returns true when a urc filter has both a string
// and a handler and otherwise returns false

TEST_F(UrcFixture, IsValid) {
    UrcFilter u_nonvalid("+NSOMNI");
    UrcFilter u_valid("+NSOMNI", do_nothing);

    ASSERT_TRUE(u_valid.isValid());
    ASSERT_FALSE(u_nonvalid.isValid());
}

// Tests that IndexOf returns the correct index of a given string and -1 otherwise.

TEST(UrcFilterListTest, IndexOfCharArray) {
    const char *my_first_string = "+NSOMNI";
    const char *my_second_string = "+NSTAT";
    const char *my_third_string = "blabla";
    const char *my_false_string = "bad";

    UrcFilterList u_l;

    u_l.append(my_first_string);
    u_l.append(my_second_string);
    u_l.append(my_third_string);

    EXPECT_EQ(0, u_l.indexOf(my_first_string));
    EXPECT_EQ(1, u_l.indexOf(my_second_string));
    EXPECT_EQ(2, u_l.indexOf(my_third_string));
    EXPECT_EQ(-1, u_l.indexOf(my_false_string));
}

// Tests that IndexOf returns the correct index of a given string and -1 otherwise
// when using nbiot strings.

TEST(UrcFilterListTest, IndexOfNbiotString) {
    const char *my_first_string = "+NSOMNI";
    const char *my_second_string = "+NSTAT";
    const char *my_third_string = "blabla";
    const char *my_false_string = "bad";

    nbiot::string my_first_nbiot_string(my_first_string);
    nbiot::string my_second_nbiot_string(my_second_string);
    nbiot::string my_third_nbiot_string(my_third_string);
    nbiot::string my_false_nbiot_string(my_false_string);

    UrcFilterList u_l;

    u_l.append(my_first_string);
    u_l.append(my_second_string);
    u_l.append(my_third_string);

    EXPECT_EQ(0, u_l.indexOf(my_first_nbiot_string));
    EXPECT_EQ(1, u_l.indexOf(my_second_nbiot_string));
    EXPECT_EQ(2, u_l.indexOf(my_third_nbiot_string));
    EXPECT_EQ(-1, u_l.indexOf(my_false_nbiot_string));
}
