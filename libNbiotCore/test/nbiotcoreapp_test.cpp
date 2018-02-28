//
// Created by Yassine Amraue on 09.02.18.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <nbiotcoreapp.h>

// Tests that getInstance() creates a new NbiotCoreApp instance if never called before.

TEST(NbiotCoreAppTest, GetInstanceFirstTime) {

}

// Tests that getInstance() always returns the exact same instance of NbiotCoreApp

TEST(NbiotCoreAppTest, GetInstanceMultipleTimes) {

}

// Tests that NbiotCoreApp returns CoreNoError when called the first time with a valid NbiotCoreConf.

TEST(NbiotCoreAppTest, ConfigureCoreApp) {

}

// Tests that NbiotCoreApp returns CoreErrorMultipleConfig when called the multiple times with a valid NbiotCoreConf.

TEST(NbiotCoreAppTest, MultipleConfigureCoreApp) {

}

// Tests that NbiotCoreApp sets the right flags on different error or warning cases.

TEST(NbiotCoreAppTest, ErrorHandling) {

}

// Tests that destroy() destroys an instance that was already created.

TEST(NbiotCoreAppTest, DestroyAlreadyCreatedInstance) {

}

// Tests that destroy() does nothing, when there is no instance to destroy.

TEST(NbiotCoreAppTest, DestroyNotCreatedInstance) {

}

// Tests that getNetworkInstance() always returns the same INetwork instance

TEST(NbiotCoreAppTest, GetNetworkInstance) {

}

// Tests that getModemInstance() always returns the same IModem instance

TEST(NbiotCoreAppTest, GetModemInstance) {

}

// Tests that getTimeInstance() always returns the same ITime instance

TEST(NbiotCoreAppTest, GetTimeInstance) {

}

// Tests that addUrcFilter returns true if serial instance successfully registered a urc filter.

TEST(NbiotCoreAppTest, AddUrcFilterSuccess) {

}

// Tests that addUrcFilter returns false if serial instance fails to registered a urc filter.

TEST(NbiotCoreAppTest, AddUrcFilterFailure) {

}

// Tests that removeUrcFilter returns true if serial instance successfully registered a urc filter.

TEST(NbiotCoreAppTest, RemoveUrcFilterSuccess) {

}

// Tests that removeUrcFilter returns false if serial instance fails to registered a urc filter.

TEST(NbiotCoreAppTest, RemoveUrcFilterFailure) {

}