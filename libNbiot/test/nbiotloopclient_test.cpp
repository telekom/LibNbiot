//
// Created by Yassine Amraue on 14.02.18.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <nbiotloopclient.h>

using namespace nbiot;

using ::testing::Return;
using ::testing::Sequence;

class _LoopHandlerCallback {
public:
    virtual ~_LoopHandlerCallback() = default;
    virtual bool loopstartHandler(int&) = 0;
    virtual bool loopstepHandler(int&) = 0;
    virtual bool loopstopHandler(int&) = 0;
};

class MockLHC : public _LoopHandlerCallback {
public:
    MOCK_METHOD1(loopstartHandler, bool(int&));
    MOCK_METHOD1(loopstepHandler, bool(int&));
    MOCK_METHOD1(loopstopHandler, bool(int&));
};

TEST(NbiotLoopClientTest, ConstructLoopClientWithLoopId) {
    LoopClient* lc = new LoopClient(LI_Unknown);

    EXPECT_NE(lc, nullptr);
    EXPECT_EQ((int) LI_Unknown, lc->getLoopId());
    EXPECT_EQ((int) LC_Fail, lc->getResult());
    EXPECT_EQ(0, lc->getValue());
}

TEST(NbiotLoopClientTest, PreLoopStep) {
    LoopClient lc(LI_Unknown);
    MockLHC lm;
    int c = 0;

    lc.setLoopStartHandler<MockLHC>(&lm, &MockLHC::loopstartHandler);
    EXPECT_CALL(lm, loopstartHandler((int&) c))
                .WillRepeatedly(Return(true));

    EXPECT_EQ((int) LC_Pending, lc.preLoopStep());
}

TEST(NbiotLoopClientTest, PreLoopStepWithoutHandler) {
    LoopClient lc(LI_Unknown);

    EXPECT_EQ((int) LC_Pending, lc.preLoopStep());
}

TEST(NbiotLoopClientTest, PreLoopStepFailToPending) {
    LoopClient lc(LI_Unknown);
    MockLHC lm;
    Sequence s1;
    int c = 0;

    EXPECT_CALL(lm, loopstartHandler((int&) c))
            .InSequence(s1)
            .WillOnce(Return(false))
            .WillOnce(Return(false))
            .WillOnce(Return(true))
            .WillRepeatedly(Return(false));

    lc.setLoopStartHandler<MockLHC>(&lm, &MockLHC::loopstartHandler);

    EXPECT_EQ((int) LC_Fail, lc.preLoopStep());
    EXPECT_EQ((int) LC_Fail, lc.preLoopStep());
    EXPECT_EQ((int) LC_Pending, lc.preLoopStep());
    EXPECT_EQ((int) LC_Pending, lc.preLoopStep());
}

TEST(NbiotLoopClientTest, LoopStepPendingAfterPreLoopStepPending) {
    LoopClient lc(LI_Unknown);
    MockLHC lm;
    int c = 0;

    EXPECT_CALL(lm, loopstartHandler(c))
            .WillOnce(Return(false))
            .WillRepeatedly(Return(true));

    EXPECT_CALL(lm, loopstepHandler(c))
            .WillOnce(Return(true))
            .WillRepeatedly(Return(true));

    lc.setLoopStartHandler<MockLHC>(&lm, &MockLHC::loopstartHandler);
    lc.setLoopStepHandler<MockLHC>(&lm, &MockLHC::loopstepHandler);

    EXPECT_EQ((int) LC_Fail, lc.preLoopStep());
    EXPECT_EQ((int) LC_Fail, lc.loopStep());
    EXPECT_EQ((int) LC_Pending, lc.preLoopStep());
    EXPECT_EQ((int) LC_Pending, lc.loopStep());
}

TEST(NbiotLoopClientTest, LoopStepIdleAfterPostLoopStepSuccess) {
    LoopClient lc(LI_Unknown);
    MockLHC lm;
    int c = 0;

    EXPECT_CALL(lm, loopstartHandler(c))
            .WillOnce(Return(false))
            .WillRepeatedly(Return(true));

    EXPECT_CALL(lm, loopstepHandler(c))
            .WillOnce(Return(false));

    EXPECT_CALL(lm, loopstopHandler(c))
            .WillRepeatedly(Return(true));

    lc.setLoopStartHandler<MockLHC>(&lm, &MockLHC::loopstartHandler);
    lc.setLoopStepHandler<MockLHC>(&lm, &MockLHC::loopstepHandler);
    lc.setLoopStopHandler<MockLHC>(&lm, &MockLHC::loopstopHandler);

    EXPECT_EQ((int) LC_Fail, lc.preLoopStep());
    EXPECT_EQ((int) LC_Fail, lc.loopStep());
    EXPECT_EQ((int) LC_Pending, lc.preLoopStep());
    EXPECT_EQ((int) LC_Idle, lc.loopStep());
}

TEST(NbiotLoopClientTest, PostLoopStepFail) {
    LoopClient *lc = new LoopClient(LI_Unknown);
    MockLHC lm;
    Sequence(s1);
    int c = 0;

    lc->setLoopStopHandler<MockLHC>(&lm, &MockLHC::loopstopHandler);

    EXPECT_EQ((int) LC_Fail, lc->postLoopStep());

    delete lc;

    lc = new LoopClient(LI_Unknown);

    lc->setLoopStopHandler<MockLHC>(&lm, &MockLHC::loopstopHandler);
    lc->setLoopStepHandler<MockLHC>(&lm, &MockLHC::loopstepHandler);
    lc->setLoopStartHandler<MockLHC>(&lm, &MockLHC::loopstartHandler);

    EXPECT_CALL(lm, loopstartHandler(c))
            .WillOnce(Return(true));

    EXPECT_CALL(lm, loopstopHandler(c))
            .WillOnce(Return(false));

    EXPECT_EQ((int) LC_Pending, lc->preLoopStep());
    EXPECT_EQ((int) LC_Fail, lc->postLoopStep());

    delete lc;
}

TEST(NbiotLoopClientTest, PostLoopStepSuccess) {
    LoopClient lc(LI_Unknown);
    MockLHC lm;
    int c = 0;

    lc.setLoopStartHandler<MockLHC>(&lm, &MockLHC::loopstartHandler);

    EXPECT_CALL(lm, loopstartHandler(c))
            .WillOnce(Return(true));

    EXPECT_EQ((int) LC_Pending, lc.preLoopStep());
    EXPECT_EQ((int) LC_Success, lc.postLoopStep());

    lc.setLoopStopHandler<MockLHC>(&lm, &MockLHC::loopstopHandler);
    lc.setLoopStepHandler<MockLHC>(&lm, &MockLHC::loopstepHandler);
    lc.setLoopStartHandler<MockLHC>(&lm, &MockLHC::loopstartHandler);

    EXPECT_CALL(lm, loopstartHandler(c))
            .WillOnce(Return(true));

    EXPECT_CALL(lm, loopstopHandler(c))
            .WillOnce(Return(true));

    EXPECT_EQ((int) LC_Pending, lc.preLoopStep());
    EXPECT_EQ((int) LC_Success, lc.postLoopStep());
}