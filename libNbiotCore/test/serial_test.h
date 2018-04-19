/* ========================================================================
 * LibNbiot: serial_test.h
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

#ifndef LIBNBIOTCORE_SERIAL_TEST_H
#define LIBNBIOTCORE_SERIAL_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "serial.h"

class _SerialCom {
public:
    virtual ~_SerialCom() = default;

    virtual unsigned char readByte() = 0;

    virtual int readStatus() = 0;

    virtual void writeByte(unsigned char) = 0;

    virtual int writeStatus() = 0;
};

class MockSerialCom : public _SerialCom {
public:
    MOCK_METHOD0(readByte, unsigned char());

    MOCK_METHOD0(readStatus, int());

    MOCK_METHOD1(writeByte, void(unsigned char));

    MOCK_METHOD0(writeStatus, int());
};

MockSerialCom *pSerCom = nullptr;

unsigned char readByte() {
    unsigned char ret = 0;
    if (nullptr != pSerCom) {
        ret = pSerCom->readByte();
    }
    return ret;
}

ReadStatus readStatus() {
    ReadStatus ret = rx_empty;
    if (nullptr != pSerCom) {
        ret = (ReadStatus) pSerCom->readStatus();
    }
    return ret;
}

void writeByte(unsigned char byte) {
    if (nullptr != pSerCom) {
        pSerCom->writeByte(byte);
    }
}

WriteStatus writeStatus() {
    WriteStatus ret = tx_full;
    if (nullptr != pSerCom) {
        ret = (WriteStatus) pSerCom->writeStatus();
    }
    return ret;
}

int number_of_calls = 0;

void do_nothing(const char *) {
    number_of_calls++;
}

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

#endif //LIBNBIOTCORE_SERIAL_TEST_H