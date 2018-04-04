/* ========================================================================
 * LibNbiot: nbiotcoreapp_test.cpp
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
