/* ========================================================================
 * LibNbiot: nbiotmodemtools_test.cpp
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

#include <nbiotmodemtools.h>

// Tests that NbiotModemTools Constructor works properly. Assert that new object is not null.

TEST(NbiotModemToolsTest, ConstructObjectFromSerial) {

}

// Tests the behaviour of sendAtCommand if "const char* command" is a nullptr. Expect that function doesn't
// break and false is returned.

TEST(NbiotModemToolsSendAtCommandsTest, SendNullptrAtCommand) {

}

// Tests the behaviour of sendAtCommand if "const char* response" is a nullptr. Expect that functions doesn't
// break and false is returned.

TEST(NbiotModemToolsSendAtCommandsTest, ReadNullptrResponse) {

}

// Tests that a command returns false, when response is longer than len.

TEST(NbiotModemToolsSendAtCommandsTest, ResponseTooLong) {

}

// Tests the behaviour of sendAtCommand when modem doesn't response in given timeout period.
// Expect that the function exits after the timeout and returns false.

TEST(NbiotModemToolsSendAtCommandsTest, NoResponseInTimeout) {

}

// Tests that AtCommand can be send to modem and the response is read properly.
// Expect that the function returns true and the correct response is stored in "char* response".

TEST(NbiotModemToolsSendAtCommandsTest, SendAtCommand) {

}

// Tests that the correct reboot command is send to the modem and that the function waits until the modem is
// rebooted properly. Expect that the function returns true after receiving an OK from the modem.

TEST(NbiotModemToolsRebootTest, RebootModem) {

}

// Tests that the correct reboot command is send to the modem and that the function waits for an answer.
// Expect that the function returns false when not receiving an OK for 65 seconds.

TEST(NbiotModemToolsRebootTest, RebootModemFailure) {

}

// Tests the behaviour of initializing modem when turning on the modem failed.
// Expect initialize() to return false.

TEST(NbiotModemToolsInitializeTest, TurnOnRadioFailed) {

}

// Tests the behaviour of initializing modem when turning on the modem succeeds but cgdcont was not set properly.
// Expect initialize() to return false.

TEST(NbiotModemToolsInitializeTest, CannotSetCGDContext) {

}

// Tests initialization of modem. Expect initialize() to return true if modem is turned on and cgdcont set properly.

TEST(NbiotModemToolsInitializeTest, ModemInitialized) {

}

// Tests that attach() returns true if modem replies with an OK after sending COPS command and false otherwise.

TEST(NbiotModemToolsAttach, ModemAttach) {

}

// Tests that detach returns true if modem replies with an OK after sending at+cgatt=0 and false otherwise.

TEST(NbiotModemToolsAttach, ModemDetach) {

}

// Tests that isAttached() returns true if modem is attached and false otherwise.

TEST(NbiotModemToolsAttach, IsAttached) {

}

// Tests that getAttachState returns the exact return code of the cgatt query and -1 if an error occured.

TEST(NbiotModemToolsAttach, getAttachState) {

}

// Tests that getImsi() stores the correct imsi as a string when called with usePw=false and returns true.

TEST(NbiotModemTools, getImsi) {

}

// Tests that getImsi() stores the correct imsi+pw as a string when called with usePw=true and returns true.

TEST(NbiotModemTools, getImsiPw) {

}

// Tests that getImsi() returns false when a Serial error occurred or the modem didn't return an OK.

TEST(NbiotModemTools, getImsiFail) {

}

// Tests that getDeviceId() returns the Imsi + Pw combination.

TEST(NbiotModemTools, getDeviceId) {

}

// Tests that isModemOn() returns the correct current modem state.

TEST(NbiotModemTools, CheckModemState) {

}

// Tests that MODEM_ON switches the modem on and MODEM_OFF switches modem off.
// Also tests the behaviour of calling switchModem(MODEM_ON) when modem is already on
// and calling switchModem(MODEM_OFF) when modem is already off.

TEST(NbiotModemTools, SwitchModemState) {

}
