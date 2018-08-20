/* ========================================================================
 * LibNbiot: nbiottopicregistry_test.cpp
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

#include <nbiottopicregistry.h>

using namespace nbiot;

class NbiotTopicRegistryCallback : public ::testing::Test {
public:

    virtual void SetUp() {
        number_of_calls = 0;
    };

    virtual void TearDown() {};

    static void do_nothing(MessageData* m) {
        number_of_calls++;
    };

    static int number_of_calls;
};

int NbiotTopicRegistryCallback::number_of_calls = 0;

TEST(NbiotTopicRegistryTest, ConstructTopicRegistry) {
    NbiotTopicRegistry *tr = new NbiotTopicRegistry();

    EXPECT_NE(nullptr, tr);
}

TEST(NbiotTopicRegistryTest, InsertTopics) {
    NbiotTopicRegistry tr;

    EXPECT_FALSE(tr.hasTopic("MyTopic", NbiotTopic::REG_NONE));

    tr.insertTopic(NbiotTopic("MyTopic"));

    EXPECT_TRUE(tr.hasTopic("MyTopic", NbiotTopic::REG_NONE));
    EXPECT_FALSE(tr.hasTopic("MyTopic", NbiotTopic::REG_SUB));
}

TEST(NbiotTopicRegistryTest, FindTopic) {
    NbiotTopicRegistry tr;

    EXPECT_EQ(-1, tr.findTopic("0"));

    tr.insertTopic(NbiotTopic("0"));

    EXPECT_EQ(0, tr.findTopic("0"));

    tr.insertTopic(NbiotTopic("1"));
    tr.insertTopic(NbiotTopic("2"));
    tr.insertTopic(NbiotTopic("3"));

    EXPECT_EQ(1, tr.findTopic("1"));
    EXPECT_EQ(2, tr.findTopic("2"));
    EXPECT_EQ(3, tr.findTopic("3"));

    NbiotTopic t1("4");
    NbiotTopic t2("5");
    t1.id = 0x001E;
    t2.id = 0x001F;
    tr.insertTopic(t1);
    tr.insertTopic(t2);

    EXPECT_EQ(4, tr.findTopic(0x001E));
    EXPECT_EQ(5, tr.findTopic(0x001F));

    EXPECT_EQ(6, tr.topicCount());
}

TEST(NbiotTopicRegistryTest, SetTopicFlag) {
    NbiotTopicRegistry tr;

    tr.insertTopic(NbiotTopic("MyTopic"));

    EXPECT_TRUE(tr.hasTopic("MyTopic", NbiotTopic::REG_NONE));
    EXPECT_FALSE(tr.hasTopic("MyTopic", NbiotTopic::REG_SUB));
    tr.setTopicFlag(0, NbiotTopic::REG_SUB);
    EXPECT_TRUE(tr.hasTopic("MyTopic", NbiotTopic::REG_SUB));
    tr.unsetTopicFlag(0, NbiotTopic::REG_SUB);
    EXPECT_TRUE(tr.hasTopic("MyTopic", NbiotTopic::REG_NONE));
}

TEST(NbiotTopicRegistryTest, HasAnyTopic) {
    NbiotTopicRegistry tr;

    tr.insertTopic(NbiotTopic("MyTopic"));

    EXPECT_TRUE(tr.hasAnyTopic(NbiotTopic::REG_NONE));
    EXPECT_FALSE(tr.hasAnyTopic(NbiotTopic::REG_SUB));
    EXPECT_FALSE(tr.hasAnyTopic(NbiotTopic::REG_PUB));
    EXPECT_FALSE(tr.hasAnyTopic(NbiotTopic::REG_BOTH));

    tr.setTopicFlag(0, NbiotTopic::REG_PUB);

    EXPECT_FALSE(tr.hasAnyTopic(NbiotTopic::REG_NONE));
    EXPECT_TRUE(tr.hasAnyTopic(NbiotTopic::REG_PUB));

    tr.setTopicFlag(0, NbiotTopic::REG_SUB);

    EXPECT_FALSE(tr.hasAnyTopic(NbiotTopic::REG_NONE));
    EXPECT_TRUE(tr.hasAnyTopic(NbiotTopic::REG_SUB));

    tr.insertTopic(NbiotTopic("MySecondTopic"));

    EXPECT_TRUE(tr.hasAnyTopic(NbiotTopic::REG_NONE));
    EXPECT_TRUE(tr.hasAnyTopic(NbiotTopic::REG_SUB));
}

TEST(NbiotTopicRegistryTest, RemoveTopics) {
    NbiotTopicRegistry tr;

    EXPECT_EQ(0 , tr.topicCount());
    tr.removeTopic(0);
    EXPECT_EQ(0 , tr.topicCount());

    tr.insertTopic(NbiotTopic("1"));
    EXPECT_EQ(1 , tr.topicCount());
    tr.removeTopic(0);
    EXPECT_EQ(0 , tr.topicCount());

    tr.insertTopic(NbiotTopic("2"));
    tr.insertTopic(NbiotTopic("3"));
    tr.insertTopic(NbiotTopic("4"));
    tr.insertTopic(NbiotTopic("5"));

    EXPECT_EQ(4 , tr.topicCount());
    EXPECT_EQ(2, tr.findTopic("4"));

    tr.removeTopic(2);

    EXPECT_EQ(3 , tr.topicCount());
    EXPECT_EQ(2, tr.findTopic("5"));

    tr.removeTopic(0);
    tr.removeTopic(0);
    tr.removeTopic(0);

    EXPECT_EQ(0 , tr.topicCount());
}

TEST(NbiotTopicRegistryTest, ClearRegistry) {
    NbiotTopicRegistry tr;

    EXPECT_EQ(0 , tr.topicCount());
    tr.clearRegistry();
    EXPECT_EQ(0 , tr.topicCount());

    tr.insertTopic(NbiotTopic("1"));
    tr.insertTopic(NbiotTopic("2"));
    tr.insertTopic(NbiotTopic("3"));

    EXPECT_NE(0 , tr.topicCount());
    tr.clearRegistry();
    EXPECT_EQ(0 , tr.topicCount());
}

TEST_F(NbiotTopicRegistryCallback, AttachMessageHandler) {
    NbiotTopicRegistry tr;
    MessageData *m;

    EXPECT_FALSE(tr.attachMessageHandler(-1, do_nothing));
    EXPECT_FALSE(tr.attachMessageHandler(0, do_nothing));

    tr.insertTopic(NbiotTopic("MyTopic"));

    EXPECT_FALSE(tr.attachMessageHandler(1, do_nothing));

    EXPECT_TRUE(tr.attachMessageHandler(0, do_nothing));

    EXPECT_EQ(number_of_calls, 0);

    tr[0].mh(m);

    EXPECT_EQ(number_of_calls, 1);
}

TEST_F(NbiotTopicRegistryCallback, AttachMessageHandlerToWildcard) {
    NbiotTopicRegistry tr;
    MessageData *m;

    tr.insertTopic(NbiotTopic("MyTopic"));
    tr.insertTopic(NbiotTopic("MyTopic/Test/Test"));
    tr.insertTopic(NbiotTopic("MyTopic/#"));
    tr.insertTopic(NbiotTopic("MyTopic/Test"));
    tr.insertTopic(NbiotTopic("MyTopic"));

    tr.attachMessageHandler(2, do_nothing);

    EXPECT_EQ(number_of_calls, 0);

    EXPECT_FALSE(tr[0].hasHandler());

    tr[1].mh(m);
    EXPECT_EQ(number_of_calls, 1);

    tr[2].mh(m);
    EXPECT_EQ(number_of_calls, 2);

    tr[3].mh(m);
    EXPECT_EQ(number_of_calls, 3);

    EXPECT_FALSE(tr[4].hasHandler());
}

TEST_F(NbiotTopicRegistryCallback, MessageDispatcher) {
    NbiotTopicRegistry tr;

    MQTTSN_topicid id;
    MQTTSN::Message m;

    id.type = MQTTSN_TOPIC_TYPE_NORMAL;
    id.data.id = 0;

    m.payload = (void*) "";
    m.qos = MQTTSN::QOS0;
    m.dup = m.id = m.payloadlen = m.retained = 0;

    MQTTSN::MessageData md(id, m);

    tr.insertTopic(NbiotTopic("MyTopic"));

    tr.attachMessageHandler(0, do_nothing);
    tr.messageDispatcher(md);

    EXPECT_EQ(number_of_calls, 1);
    EXPECT_EQ(tr.arrivedMessages(), 1);

    tr.messageDispatcher(md);
    tr.messageDispatcher(md);
    tr.messageDispatcher(md);

    EXPECT_EQ(number_of_calls, 4);
    EXPECT_EQ(tr.arrivedMessages(), 4);
}

// @TODO: attachWildcardMH, updateWildcardMH