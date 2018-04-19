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

#include <nbiottopic.h>
#include <list.h>

using namespace nbiot;
using namespace util;

using ::testing::Return;
using ::testing::Sequence;

TEST(NbiotTopicConstructionTest, ConstructStandard) {
    NbiotTopic* nt = new NbiotTopic();
    ASSERT_NE(nullptr, nt);

    EXPECT_EQ((int) MQTTSN_TOPIC_TYPE_NORMAL, nt->type);
    EXPECT_EQ(0, nt->id);
    EXPECT_EQ((int) MQTTSN_RC_ACCEPTED, nt->returnCode);
    EXPECT_EQ(nullptr, nt->mh);
    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_NONE));

    delete nt;
}

TEST(NbiotTopicConstructionTest, ConstructShortTopic) {
    char bytes[2] = "a";
    NbiotTopic* nt = new NbiotTopic(bytes);
    ASSERT_NE(nullptr, nt);

    EXPECT_EQ((int) MQTTSN_TOPIC_TYPE_SHORT, nt->type);
    EXPECT_EQ(0, nt->id);
    EXPECT_STREQ("a", nt->shortName.shortName);
    EXPECT_EQ((int) MQTTSN_RC_ACCEPTED, nt->returnCode);
    EXPECT_EQ(nullptr, nt->mh);
    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_NONE));

    delete nt;
}

TEST(NbiotTopicConstructionTest, ConstructWithTopicId) {
    unsigned short id = 0x001F;
    NbiotTopic* nt = new NbiotTopic(id);
    ASSERT_NE(nullptr, nt);

    EXPECT_EQ((int) MQTTSN_TOPIC_TYPE_PREDEFINED, nt->type);
    EXPECT_EQ(id, nt->id);
    EXPECT_EQ((int) MQTTSN_RC_ACCEPTED, nt->returnCode);
    EXPECT_EQ(nullptr, nt->mh);
    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_NONE));
}

TEST(NbiotTopicConstructionTest, ConstructWithTopicName) {
    string topicName("MyTopic");
    NbiotTopic* nt = new NbiotTopic(topicName.c_str());
    ASSERT_NE(nullptr, nt);

    EXPECT_EQ((int) MQTTSN_TOPIC_TYPE_NORMAL, nt->type);
    EXPECT_EQ(0, nt->id);
    EXPECT_STREQ("MyTopic", nt->topicName.c_str());
    EXPECT_EQ((int) MQTTSN_RC_ACCEPTED, nt->returnCode);
    EXPECT_EQ(nullptr, nt->mh);
    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_NONE));
}

//TEST(NbiotTopicConstructionTest, NullptrConstruction) {
//    char* bytes = nullptr;
//    NbiotTopic* nt = new NbiotTopic(bytes);
//
//    EXPECT_NE(nt, nullptr);
//
//    delete nt;
//}

TEST(NbiotTopicOperatorTest, EqualityOperator) {
    unsigned short id = 1;
    char bytes[2] = "a";
    string topicName("MyTopic");

    NbiotTopic nt_short(bytes);
    NbiotTopic nt_id(id);
    NbiotTopic nt_topic(topicName.c_str());


    EXPECT_TRUE(nt_short == bytes);
    EXPECT_FALSE(nt_short == id);
    EXPECT_FALSE(nt_short == topicName.c_str());

    EXPECT_FALSE(nt_id == bytes);
    EXPECT_TRUE(nt_id == id);
    EXPECT_FALSE(nt_id == topicName.c_str());

    EXPECT_FALSE(nt_topic == bytes);
    EXPECT_FALSE(nt_topic == id);
    EXPECT_TRUE(nt_topic == topicName.c_str());
}

//TEST(NbiotTopicOperatorTest, EqualityToNullptr) {
//    char *bytes = nullptr;
//    const char *c_bytes = nullptr;
//
//    NbiotShortTopic st1(bytes);
//
//    EXPECT_TRUE(st1 == bytes);
//    EXPECT_TRUE(st1 == c_bytes);
//}

TEST(NbiotTopicOperatorTest, AssignmentOperator) {
    unsigned short id = 1;
    char bytes[2] = "a";
    string topicName("MyTopic");

    NbiotTopic nt_short(bytes);
    NbiotTopic nt_id(id);
    NbiotTopic nt_topic(topicName.c_str());

    NbiotTopic nt_short_copy = nt_short;
    NbiotTopic nt_id_copy = nt_id;
    NbiotTopic nt_topic_copy = nt_topic;

    EXPECT_TRUE(nt_short_copy == bytes);
    EXPECT_FALSE(nt_short_copy == id);
    EXPECT_FALSE(nt_short_copy == topicName.c_str());

    EXPECT_FALSE(nt_id_copy == bytes);
    EXPECT_TRUE(nt_id_copy == id);
    EXPECT_FALSE(nt_id_copy == topicName.c_str());

    EXPECT_FALSE(nt_topic_copy == bytes);
    EXPECT_FALSE(nt_topic_copy == id);
    EXPECT_TRUE(nt_topic_copy == topicName.c_str());
}

//TEST(NbiotTopicOperatorTest, NullptrAssignment) {
//    char* bytes = nullptr;
//
//    NbiotShortTopic *st1 = new NbiotShortTopic(bytes);
//
//    *st1 = bytes;
//
//    ASSERT_NE(st1, nullptr);
//    EXPECT_TRUE(*st1 == bytes);
//}

TEST(NbiotTopicTest, IsValid) {
    unsigned short id = 1;
    char bytes[2] = "a";
    string topicName("MyTopic");

    NbiotTopic nt_short_valid(bytes);
    NbiotTopic nt_id_valid(id);
    NbiotTopic nt_topic_valid(topicName.c_str());

    char bytes_notvalid[2];
    bytes_notvalid[0] = '\0';
    bytes_notvalid[1] = '\0';
    string topic_notvalid("");

    NbiotTopic nt_short_notvalid(bytes_notvalid);
    NbiotTopic nt_id_notvalid((unsigned short) 0);
    NbiotTopic nt_topic_notvalid(topic_notvalid.c_str());

    EXPECT_TRUE(nt_short_valid.valid());
    EXPECT_TRUE(nt_id_valid.valid());
    EXPECT_TRUE(nt_topic_valid.valid());

    EXPECT_FALSE(nt_short_notvalid.valid());
    EXPECT_FALSE(nt_id_notvalid.valid());
    EXPECT_FALSE(nt_topic_notvalid.valid());
}

TEST(NbiotTopicTest, RegistrationFlag) {
    NbiotTopic *nt = new NbiotTopic();

    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_NONE));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_BOTH));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_PUB));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_SUB));

    delete nt;
    nt = new NbiotTopic();

    nt->setReg(NbiotTopic::REG_SUB);

    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_NONE));
    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_BOTH));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_PUB));
    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_SUB));

    nt->unsetReg(NbiotTopic::REG_SUB);

    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_NONE));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_BOTH));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_PUB));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_SUB));

    delete nt;
    nt = new NbiotTopic();

    nt->setReg(NbiotTopic::REG_PUB);

    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_NONE));
    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_BOTH));
    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_PUB));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_SUB));

    nt->unsetReg(NbiotTopic::REG_PUB);

    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_NONE));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_BOTH));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_PUB));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_SUB));

    delete nt;
    nt = new NbiotTopic();

    nt->setReg(NbiotTopic::REG_BOTH);

    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_NONE));
    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_BOTH));
    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_PUB));
    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_SUB));

    nt->unsetReg(NbiotTopic::REG_SUB);

    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_NONE));
    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_BOTH));
    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_PUB));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_SUB));

    nt->unsetReg(NbiotTopic::REG_PUB);

    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_NONE));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_BOTH));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_PUB));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_SUB));

    delete nt;
    nt = new NbiotTopic();

    nt->setReg(NbiotTopic::REG_BOTH);
    nt->unsetReg(NbiotTopic::REG_BOTH);

    EXPECT_TRUE(nt->isReg(NbiotTopic::REG_NONE));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_BOTH));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_PUB));
    EXPECT_FALSE(nt->isReg(NbiotTopic::REG_SUB));

    delete nt;
}

TEST(NbiotTopicTest, WildcardTopic) {
    list<string> sl;
    NbiotTopic *nt;

    sl.append("MyTo/+/pic");
    sl.append("+/MyTopic");
    sl.append("MyTopic/+");
    sl.append("MyTo/#/pic");
    sl.append("#/MyTopic");
    sl.append("MyTopic/#");
    sl.append("MyTopic");
    sl.append("");

    for(int i = 0; i < 6; ++i) {
        nt = new NbiotTopic(sl[i].c_str());
        EXPECT_TRUE(nt->isWildcardTopic());
        delete nt;
    }

    for(int i = 6; i < 8; ++i) {
        nt = new NbiotTopic(sl[i].c_str());
        EXPECT_FALSE(nt->isWildcardTopic());
        delete nt;
    }
}

TEST(NbiotTopicTest, ConvertToSNTopic) {
    // Topic type predefined
    unsigned short id = 0x001F;
    NbiotTopic nt_predefined(id);

    MQTTSN_topicid topicid;

    nt_predefined.toSNTopic(topicid);

    EXPECT_EQ(topicid.type, nt_predefined.type);
    EXPECT_EQ(topicid.data.id, nt_predefined.id);


    // Topic type short
    char bytes[2] = "a";
    NbiotTopic nt_short(bytes);
    nt_short.toSNTopic(topicid);

    EXPECT_EQ(topicid.type, nt_short.type);
    EXPECT_TRUE(nt_short.shortName == topicid.data.short_name);

    // Topic type normal
    NbiotTopic nt_normal;
    nt_normal.toSNTopic(topicid);

    EXPECT_STREQ(topicid.data.long_.name, nt_normal.topicName.getData());
    EXPECT_EQ(topicid.data.long_.len, nt_normal.topicName.size());
}

TEST(NbiotTopicTest, IsTopicMatched) {
    unsigned short id = 0x001F;
    NbiotTopic nt_predefined(id);
    NbiotTopic nt_predefined_copy;
    nt_predefined_copy = nt_predefined;

    char bytes[2] = "a";
    NbiotTopic nt_short(bytes);
    NbiotTopic nt_short_copy;
    nt_short_copy = nt_short;

    NbiotTopic nt_normal;
    NbiotTopic nt_normal_copy;
    nt_normal_copy = nt_normal;

    EXPECT_TRUE(nt_predefined.isTopicMatched(nt_predefined_copy));
    EXPECT_TRUE(nt_predefined.isTopicMatched(nt_short_copy));
    EXPECT_TRUE(nt_predefined.isTopicMatched(nt_normal_copy));
}