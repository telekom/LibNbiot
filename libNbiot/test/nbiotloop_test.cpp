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

#include <nbiotloop.h>

using namespace nbiot;

using ::testing::Return;
using ::testing::Sequence;

class LoopClientMock : public iLoopControlled {
public:
    MOCK_METHOD0(preLoopStep, NbiotResult());
    MOCK_METHOD0(loopStep, NbiotResult());
    MOCK_METHOD0(postLoopStep, NbiotResult());
    MOCK_CONST_METHOD0(getResult, NbiotResult());
    MOCK_CONST_METHOD0(getValue, int());
    MOCK_METHOD1(setValue, void(int));
    MOCK_CONST_METHOD0(getLoopId, NbiotLoopId());
};

TEST(NbiotLoopTest, ConstructEmptyLoop) {
    NbiotLoop *nl = new NbiotLoop();

    EXPECT_NE(nl, nullptr);
    EXPECT_EQ(0, nl->getLoopValue());
    EXPECT_EQ(((int) LI_Unknown), nl->getLoopId());
}

TEST(NbiotLoopTest, NullptrRegisterClearsLoop) {
    LoopClientMock lm;
    NbiotLoop nbl;

    // Is busy only true if m_clienList.count() > 0
    EXPECT_FALSE(nbl.isBusy());

    nbl.registerLoopClient(&lm);
    EXPECT_TRUE(nbl.isBusy());

    nbl.registerLoopClient(nullptr);
    EXPECT_FALSE(nbl.isBusy());
}

TEST(NbiotLoopTest, NbiotLoopEmptyClientList) {
    LoopClientMock lm;
    NbiotLoop nbl;

    EXPECT_EQ((int) LC_Idle, nbl.nbiotLoop());
}

TEST(NbiotLoopTest, NbiotLoop) {
    LoopClientMock *lm = new LoopClientMock();
    NbiotLoop nbl;

    EXPECT_CALL(*lm, getLoopId())
            .WillRepeatedly(Return(LI_Unknown));

    EXPECT_CALL(*lm, preLoopStep())
            .WillOnce(Return(LC_Pending));

    nbl.registerLoopClient(lm);

    EXPECT_EQ((int) LC_Pending, nbl.nbiotLoop());

    EXPECT_CALL(*lm, getValue())
            .WillRepeatedly(Return(0));

    EXPECT_CALL(*lm, loopStep())
            .WillOnce(Return(LC_Idle));

    EXPECT_CALL(*lm, getResult())
            .WillOnce(Return(LC_Idle));

    EXPECT_EQ((int) LC_Idle, nbl.nbiotLoop());

    nbl.registerLoopClient(lm);

    EXPECT_CALL(*lm, getLoopId())
            .WillRepeatedly(Return(LI_Unknown));

    EXPECT_CALL(*lm, preLoopStep())
            .WillOnce(Return(LC_Pending));

    EXPECT_EQ((int) LC_Pending, nbl.nbiotLoop());


    EXPECT_CALL(*lm, loopStep())
            .WillOnce(Return(LC_Fail));

    EXPECT_CALL(*lm, getResult())
            .WillOnce(Return(LC_Fail));

    EXPECT_EQ((int) LC_Fail, nbl.nbiotLoop());

    delete lm;
}
