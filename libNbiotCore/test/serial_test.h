//
// Created by Yassine Amraue on 24.01.18.
//

#ifndef LIBNBIOTCORE_SERIAL_TEST_H
#define LIBNBIOTCORE_SERIAL_TEST_H

#endif //LIBNBIOTCORE_SERIAL_TEST_H

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