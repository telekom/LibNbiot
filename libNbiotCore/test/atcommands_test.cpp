//
// Created by Yassine Amraue on 26.01.18.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "serial.h"

#include <atcommands.h>

using ::testing::Return;

//class _Serial {
//public:
//    _Serial();
//    _Serial(int);
//    _Serial(const Serial&);
//
//    virtual ~_Serial() = 0;
//
//    virtual _Serial& operator=(const Serial&) = 0;
//
//    virtual void configSetReadStatus(ReadStatus (*)()) = 0;
//    virtual void configSetReadByte(unsigned char (*)()) = 0;
//    virtual void configSetPutChar(void (*)(unsigned char)) = 0;
//    virtual void configSetWriteStatus(WriteStatus (*)()) = 0;
//
//    virtual int getError() = 0;
//    virtual void clearError() = 0;
//
//    virtual size_t readLine(nbiot::string&, unsigned short, Serial::SerialReadMode) = 0;
//    virtual size_t readRaw(unsigned char*, unsigned short) = 0;
//
//    virtual size_t write(char) = 0;
//
//    virtual size_t write(const char*, size_t) = 0;
//    virtual size_t write(const nbiot::string&) = 0;
//
////    template<class UHC>
////    virtual bool addUrcFilter(const char*, UHC*, void (UHC::*)(const char*)) = 0;
//    virtual bool addUrcFilter(const char*, void (*)(const char*)) = 0;
//    virtual bool removeUrcFilter(const char*) = 0;
//    virtual void clearFilter() = 0;
//    virtual void hasFilter() = 0;
//
//    virtual void setDebugPrefix(int) = 0;
//    virtual int getDebugPrefix() const = 0;
//};

//class MockSerial : public _Serial {
//public:
//    MOCK_METHOD1(configSetReadStatus, void( ReadStatus (*)() ) );
//    MOCK_METHOD1(configSetReadByte, void( unsigned char (*)() ));
//    MOCK_METHOD1(configSetPutChar, void( void (*)(unsigned char) ) );
//    MOCK_METHOD1(configSetWriteStatus, void( WriteStatus (*)() ) );
//
//    MOCK_METHOD0(getError, int());
//    MOCK_METHOD0(clearError, void());
//
//    MOCK_METHOD3(readLine, size_t( nbiot::string&, unsigned short, Serial::SerialReadMode ));
//    MOCK_METHOD2(readRaw, size_t( unsigned char*, unsigned short ) );
//
//    MOCK_METHOD1(write, size_t( char ));
//    MOCK_METHOD2(write, size_t( const char*, size_t ));
//    MOCK_METHOD1(write, size_t( const nbiot::string& ));
//
//    MOCK_METHOD2(addUrcFilter, bool( const char*, void (*)(const char*) ));
//    MOCK_METHOD1(removeUrcFilter, bool( const char* ));
//    MOCK_METHOD0(clearFilter, void());
//    MOCK_METHOD0(hasFilter, void());
//
//    MOCK_METHOD1(setDebugPrefix, void(int));
//    MOCK_CONST_METHOD0(getDebugPrefix, int());
//};

// Tests if AtCommands object can be constructed from a Serial object and the debug prefix is set properly

TEST(AtCommandsTest, ConstructObjectFromSerial) {

}

// Tests that an AtCommand can be send to the buffer. Expect that sendCommand returns true if command
// was written completely without an error and false otherwise

TEST(AtCommandsTest, SendCommandToBuffer) {

}

// Tests that readResponse returns true if function readResponse is called with REPLY_OK and "OK" is
// in buffer. Expect that response contains "OK" afterwards.

TEST(AtCommandsReadResponseTest, ExpectedReplyOkInBuffer) {

}

// Tests that readResponse returns false if function readResponse is called with REPLY_OK but there is
// no "OK" in buffer. Expect that response is an empty string.

TEST(AtCommandsReadResponseTest, ExpectedReplyOkNotInBuffer) {

}

// Tests that readResponse reads exactly two lines from buffer when called with REPLY_ANY where the first line
// can be any response and the second line has to be an "OK". In this case the function returns true.
// Expect that function returns false if serial error occurred or second line wasn't "OK" as expected.

TEST(AtCommandsReadResponseTest, ResponseFollowedByOkReply) {

}

// Tests that readResponse reads exactly two lines from buffer when called with REPLY_ANY where the first line
// can be any response and the second line is NOT an "OK".
// Expect that function returns false if serial error occurred or second line wasn't "OK" as expected and
// that response is an empty string.

TEST(AtCommandsReadResponseTest, ResponseNotFollowedByOkReply) {

}

// Tests that readResponse reads multiple lines, until it reads an "OK" from buffer, when called with
// REPLY_COPY. Expect that every line is stored in response and the function returns true.

TEST(AtCommandsReadResponseTest, ExpectedReplyCopy) {

}

// Tests that readResponse returns false if no line was read or serial error occurred.

TEST(AtCommandsReadResponseTest, ExpectedReplyCopyError) {

}

// Tests the behaviour of readResponse with expected reply REPLY_COPY when there are multiple lines in buffer
// but no OK as the last line. Expect that the function prevents an unexpected memory overflow.

TEST(AtCommandsReadResponseTest, ExpectedReplyCopyOverflow) {

}

// Tests that readResponse with expected reply REPLY_EXACT reads exactly one line and stores it in response.
// Expect that readResponse returns true;

TEST(AtCommandsReadResponseTest, ExpectedReplyExactInBuffer) {

}

// Tests that readResponse with expected reply REPLY_EXACT returns false if a serial error occurred or no
// line was read. Expect that response is an empty string.

TEST(AtCommandsReadResponseTest, ExpectedReplyExactError) {

}

// Why do I through away a character that I read?

TEST(AtCommandsReadResponseTest, ExpectedReplyNoneWithNonEmptyBuffer) {

}

// Tests that readResponse with expected reply REPLY_NONE waits until timeout and returns true if there was
// no char in buffer. Expect that response is an empty string.

TEST(AtCommandsReadResponseTest, ExpectedReplyNoneWithEmptyBuffer) {

}

// Tests that readResponse ignores everything that is in the buffer when called with expected reply REPLY_IGNORE.
// Expect that response is an empty string and function returns true if there was no Serial error.

TEST(AtCommandsReadResponseTest, ExpectedReplyIgnore) {

}

// Tests that readResponse with every expected reply option returns false on Serial Error.

TEST(AtCommandsReadResponseTest, ReturnFalseOnSerialError) {

}

// Tests that readUntil returns false if a Serial error occurs.

TEST(AtCommandsReadUntilTest, ReturnFalseOnSerialError) {

}

// Tests that readUntil returns true if the expected string was exactly read from serial buffer,
// i.e. if the line only consists of the expected string independently of other lines in the buffer.

TEST(AtCommandsReadUntilTest, ExpectedStringFoundExact) {

}

// Tests that readUntil returns true if the expected string was read from serial buffer,
// independently of other lines in the buffer and if the line doesn't only consist of the expected string
// but also other characters.

TEST(AtCommandsReadUntilTest, ExpectedStringFoundNotExact) {

}

// Tests that readUntil returns false on timeout, i.e. if the expected string wasn't found.

TEST(AtCommandsReadUntilTest, FalseOnTimeout) {

}