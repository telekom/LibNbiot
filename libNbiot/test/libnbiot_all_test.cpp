//
// Created by Yassine Amraue on 14.02.18.
//

#include "gmock/gmock.h"

int main(int argc, char **argv) {
    int rc = 0;

    ::testing::InitGoogleMock(&argc, argv);
    //::testing::InitGoogleTest(&argc, argv);

    rc = RUN_ALL_TESTS();

    return rc;
}