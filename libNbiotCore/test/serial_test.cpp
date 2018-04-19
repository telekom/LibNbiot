/* ========================================================================
 * LibNbiot: serial_test.cpp
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

#include "serial_test.h"
#include <libnbiotcore.h>
#include "callbacktimer.h"

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::Sequence;
using ::testing::AtLeast;
using ::testing::Action;
using ::testing::HasSubstr;

// Tests that the serial standard constructor is initialized with no error.

TEST(SerialTest, SerialStandardConstructor) {
    Serial s;
    EXPECT_EQ(((int) Serial::NoError), s.getError())
                        << "Serial should be initalized with NoError";
    EXPECT_EQ(0, s.getDebugPrefix());
}

// Tests that the serial dbgprf constructor is initialized with no error and dbgprf is set as expected.

TEST(SerialTest, SerialDbgprfConstructor) {
    int dbgprf = 1;
    Serial s(dbgprf);
    EXPECT_EQ(((int) Serial::NoError), s.getError())
                        << "Serial should be initalized with NoError";
    EXPECT_EQ(dbgprf, s.getDebugPrefix());
}

// Tests that copied object is independent of original

TEST(SerialTest, SerialCopyConstructor) {
    int dbgprf = 1;

    Serial s(dbgprf);
    Serial s_copy(s);

    EXPECT_EQ(0, s_copy.getDebugPrefix());
    EXPECT_EQ(s.getError(), s_copy.getError());

    s_copy.setDebugPrefix(dbgprf + 1);

    EXPECT_NE(s.getDebugPrefix(), s_copy.getDebugPrefix());

    s_copy.configSetReadByte(nullptr);

    EXPECT_NE(s.getError(), s_copy.getError());
}

// Tests that assignment operator copies original content and assigned object is independent of the original object

TEST(SerialTest, SerialAssignmentOperator) {
    int dbgprf = 1;

    Serial s(dbgprf);
    Serial s_copy;

    s_copy = s;

    EXPECT_EQ(s.getDebugPrefix(), s_copy.getDebugPrefix());
    EXPECT_EQ(s.getError(), s_copy.getError());

    s_copy.setDebugPrefix(dbgprf + 1);

    EXPECT_NE(s.getDebugPrefix(), s_copy.getDebugPrefix());

    s_copy.configSetReadByte(nullptr);

    EXPECT_NE(s.getError(), s_copy.getError());
}

// Tests if debug prefix can be set

TEST(SerialTest, SetDebugPrefix) {
    int dbgprf = 1;
    Serial s(dbgprf);

    EXPECT_EQ(1, s.getDebugPrefix());

    s.setDebugPrefix(dbgprf + 1);

    EXPECT_EQ(dbgprf + 1, s.getDebugPrefix());
}

// Tests that error is set properly if read byte function wasn't set properly

TEST(SerialTest, NoReadByteFunction) {
    Serial s;
    s.configSetReadByte(nullptr);
    EXPECT_EQ(((int) Serial::NoReadFunction), s.getError());
}

// Tests that error is set properly if read status function wasn't set properly

TEST(SerialTest, NoReadStatusFunction) {
    Serial s;
    s.configSetReadStatus(nullptr);
    EXPECT_EQ(((int) Serial::NoReadFunction), s.getError());
}

// Tests that error is set properly if write byte function wasn't set properly

TEST(SerialTest, NoPutCharFunction) {
    Serial s;
    s.configSetPutChar(nullptr);
    EXPECT_EQ(((int) Serial::NoWriteFunction), s.getError());
}

// Tests that error is set properly if write status function wasn't set properly

TEST(SerialTest, NoWriteStatusFunction) {
    Serial s;
    s.configSetWriteStatus(nullptr);
    EXPECT_EQ(((int) Serial::NoError), s.getError());
}

// Tests that clearError resets error to NoError

TEST(SerialTest, ClearError) {
    Serial s;
    EXPECT_EQ(((int) Serial::NoError), s.getError());
    s.configSetPutChar(nullptr);
    EXPECT_NE(((int) Serial::NoError), s.getError());
    s.clearError();
    EXPECT_EQ(((int) Serial::NoError), s.getError());
}

// Tests that ReadRaw can read a character from buffer, when buffer has a character

TEST(SerialTest, ReadRawByteWithRxAvailable) {
    Serial s;
    pSerCom = new MockSerialCom();

    s.configSetWriteStatus(writeStatus);
    s.configSetReadStatus(readStatus);
    s.configSetReadByte(readByte);
    s.configSetPutChar(writeByte);

    EXPECT_CALL(*pSerCom, readByte())
            .WillRepeatedly(Return('c'));
    EXPECT_CALL(*pSerCom, readStatus())
            .WillRepeatedly(Return(rx_avail));

    unsigned char buffer;

    size_t result = s.readRaw(&buffer, 10);
    EXPECT_EQ('c', buffer);
    EXPECT_EQ((int) Serial::NoError, s.getError());
    EXPECT_EQ(1, result);

    delete (pSerCom);
}

// Tests ReadRaw timeout when trying to read a character from buffer but buffer is empty

TEST(SerialTest, ReadRawTimeoutRxEmpty) {
    RepeatedTimer tick_timer(1, tick);

    tick_timer.start();

    Serial s;
    pSerCom = new MockSerialCom();

    s.configSetWriteStatus(writeStatus);
    s.configSetReadStatus(readStatus);
    s.configSetReadByte(readByte);
    s.configSetPutChar(writeByte);

//    EXPECT_CALL(*pSerCom, readByte())
//            .WillRepeatedly(Return('c'));
    EXPECT_CALL(*pSerCom, readStatus())
            .WillRepeatedly(Return(rx_empty));

    unsigned char buffer;
    size_t result = s.readRaw(&buffer, 10);

    EXPECT_EQ((int) Serial::ReadTimeout, s.getError());
    EXPECT_EQ(0, result);

    tick_timer.stop();
    delete (pSerCom);
}

// Tests if error is set properly when trying to read although no ReadFunction is set

TEST(SerialTest, ReadWithoutReadFunction) {
    Serial s;
    pSerCom = new MockSerialCom();

    s.configSetWriteStatus(writeStatus);
    s.configSetPutChar(writeByte);

    unsigned char buffer;
    size_t result;
    result = s.readRaw(&buffer, 10);

    EXPECT_EQ((int) Serial::NoReadFunction, s.getError())
                        << "After calling readRaw without specifying a readByte function"
                        << "the error should be set to NoReadFunction";
    EXPECT_EQ(0, result);

    nbiot::string line;
    result = s.readLine(line, 10);

    EXPECT_EQ((int) Serial::NoReadFunction, s.getError())
                        << "After calling readLine without specifying a readByte function"
                        << "the error should be set to NoReadFunction";
    EXPECT_EQ(0, result);
    delete (pSerCom);
}

// Tests if ReadLine reads a line from buffer when there are characters in the buffer
// is set to simple

TEST(SerialTest, ReadLineModes) {
    Serial s;
    Sequence s1;
    pSerCom = new MockSerialCom();
    nbiot::string line, line2, line3;

    s.configSetWriteStatus(writeStatus);
    s.configSetReadStatus(readStatus);
    s.configSetReadByte(readByte);
    s.configSetPutChar(writeByte);

    EXPECT_CALL(*pSerCom, writeStatus())
            .WillRepeatedly(Return(tx_empty));

    EXPECT_CALL(*pSerCom, readStatus())
            .WillRepeatedly(Return(rx_avail));

    const char my_str[] = "blabla\n";
    size_t result;

    for (int i = 0; i < 7; ++i) {
        EXPECT_CALL(*pSerCom, readByte())
                .InSequence(s1)
                .WillOnce(Return(my_str[i]));
    }

    result = s.readLine(line, 10, Serial::SerialReadMode::Simple);

    EXPECT_STREQ("blabla", line.c_str())
                        << "SerialReadModeSimple should read exactly one line from buffer until a line escape character";
    EXPECT_EQ(6, result);

    const char my_str2[] = "\nblabla\n";

    for (int i = 0; i < 8; ++i) {
        EXPECT_CALL(*pSerCom, readByte())
                .InSequence(s1)
                .WillOnce(Return(my_str2[i]));
    }

    result = s.readLine(line2, 10, Serial::SerialReadMode::Normal);

    EXPECT_STREQ("blabla", line2.c_str())
                        << "SerialReadMode Normal should ignore the first line escape character";
    EXPECT_EQ(6, result);

    const char my_str3[] = "\nblabla\n";

    for (int i = 0; i < 8; ++i) {
        EXPECT_CALL(*pSerCom, readByte())
                .InSequence(s1)
                .WillOnce(Return(my_str3[i]));
    }

    result = s.readLine(line3, 10, Serial::SerialReadMode::Ignore);

    EXPECT_STREQ("", line3.c_str())
                        << "SerialReadMode Ignore should ignore the next line in the buffer";
    EXPECT_EQ(6, result);

    delete (pSerCom);
}

// Tests if ReadLine ignores Carriage Return characters \\r

TEST(SerialTest, ReadLineIgnoresCarriageReturn) {
    Serial s;
    Sequence s1;
    pSerCom = new MockSerialCom();
    nbiot::string line;

    s.configSetWriteStatus(writeStatus);
    s.configSetReadStatus(readStatus);
    s.configSetReadByte(readByte);
    s.configSetPutChar(writeByte);

    EXPECT_CALL(*pSerCom, writeStatus())
            .WillRepeatedly(Return(tx_empty));

    EXPECT_CALL(*pSerCom, readStatus())
            .WillRepeatedly(Return(rx_avail));

    const char my_str[] = "\r\rbla\rbla\n";
    size_t result;

    for (int i = 0; i < 10; ++i) {
        EXPECT_CALL(*pSerCom, readByte())
                .InSequence(s1)
                .WillOnce(Return(my_str[i]));
    }

    result = s.readLine(line, 10, Serial::SerialReadMode::Simple);

    EXPECT_STREQ("blabla", line.c_str())
                        << "ReadLine should ignore Carriage Return character '\\r'";
    EXPECT_EQ(6, result);

    delete (pSerCom);
}

// Tests that URC filters can be added and removed.

TEST(SerialTest, AddingAndRemovingUrcFilter) {
    Serial s;
    MockUrcCallback UHC;
    ASSERT_TRUE(s.addUrcFilter("+NSOMNI", do_nothing))
                                << "Failed to add URC Filter with free callback function.";
    ASSERT_FALSE(s.addUrcFilter<MockUrcCallback>("+NSOMNI", &UHC, &MockUrcCallback::do_nothing))
                                << "Failed to add already existing URC Filter with member method.";
    ASSERT_TRUE(s.hasFilter()) << "No filter was added.";
    s.removeUrcFilter("+NSOMNI");
    EXPECT_FALSE(s.hasFilter()) << "Failed to remove URC Filter.";

    ASSERT_TRUE(s.addUrcFilter("+NSOMNI", do_nothing))
                                << "Failed to add URC Filter with free callback function.";
    ASSERT_TRUE(s.addUrcFilter<MockUrcCallback>("+NSTAT", &UHC, &MockUrcCallback::do_nothing));
    ASSERT_TRUE(s.hasFilter()) << "Failed to add multiple URC Filter.";
    s.clearFilter();
    EXPECT_FALSE(s.hasFilter()) << "Failed to clear URC FilterList";
}

// Tests that URCs are filtered properly and the callback function is called.

TEST(SerialTest, UrcFilter) {
    Serial s;
    Sequence s1;
    pSerCom = new MockSerialCom();
    nbiot::string line;

    s.configSetWriteStatus(writeStatus);
    s.configSetReadStatus(readStatus);
    s.configSetReadByte(readByte);
    s.configSetPutChar(writeByte);

    EXPECT_CALL(*pSerCom, writeStatus())
            .WillRepeatedly(Return(tx_empty));

    EXPECT_CALL(*pSerCom, readStatus())
            .WillRepeatedly(Return(rx_avail));

    number_of_calls = 0;
    s.addUrcFilter("+NSOMNI", do_nothing);
    s.addUrcFilter("+NSTAT", do_nothing);

    const char my_str[] = "+NSOMNI\n+NSOMNI\n+NSTAT\nOK\n";
    size_t result;

    for (int i = 0; i < 26; ++i) {
        EXPECT_CALL(*pSerCom, readByte())
                .InSequence(s1)
                .WillOnce(Return(my_str[i]));
    }

    result = s.readLine(line, 10, Serial::SerialReadMode::Simple);

    EXPECT_TRUE(number_of_calls)
                        << "Callback function wasn't called.";

    EXPECT_THAT(line.c_str(), Not(HasSubstr("+NSOMNI")))
                        << "At least one URC was not filtered properly during readLine.";
    EXPECT_THAT(line.c_str(), Not(HasSubstr("+NSTAT")))
                        << "At least one URC was not filtered properly during readLine.";

    EXPECT_STREQ("OK", line.c_str());
    EXPECT_EQ(2, result);
    EXPECT_EQ(3, number_of_calls) << "URC callback function should be called " << 3 << " times";

    delete (pSerCom);
}

// Tests if a single character can be written to the buffer

TEST (SerialTest, WriteSingleChar) {
    Serial s;
    pSerCom = new MockSerialCom();

    s.configSetPutChar(writeByte);

    EXPECT_CALL(*pSerCom, writeByte('c'))
            .Times(1);

    EXPECT_EQ(1, s.write('c'));
    EXPECT_EQ(((int) Serial::NoError), s.getError());

    delete (pSerCom);
}

// Tests if error is set properly if trying to write to the buffer without specifying a write function

TEST (SerialTest, WriteSingleCharError) {
    Serial s;
    nbiot::string line;

    EXPECT_EQ(0, s.write('c'));
    EXPECT_EQ(((int) Serial::NoWriteFunction), s.getError());
}

// Tests if a const char array can be written to the buffer

TEST (SerialTest, WriteMultipleChars) {
    Serial s;
    pSerCom = new MockSerialCom();

    s.configSetPutChar(writeByte);

    EXPECT_CALL(*pSerCom, writeByte('c'))
            .Times(4);

    EXPECT_EQ(4, s.write("cccc", 4));
    EXPECT_EQ(((int) Serial::NoError), s.getError());

    delete (pSerCom);
}

// Tests if error is set properly if trying to write multiple characters to the buffer
// without specifying a write function

TEST (SerialTest, WriteMultipleCharsError) {
    Serial s;
    nbiot::string line;

    EXPECT_EQ(0, s.write("cccc"));
    EXPECT_EQ(((int) Serial::NoWriteFunction), s.getError());
}

// Tests if an nbiot string can be written to the buffer

TEST (SerialTest, WriteNbiotString) {
    Serial s;
    pSerCom = new MockSerialCom();

    s.configSetPutChar(writeByte);

    EXPECT_CALL(*pSerCom, writeByte('c'))
            .Times(4);

    nbiot::string my_string("cccc");

    EXPECT_EQ(4, s.write(my_string));
    EXPECT_EQ(((int) Serial::NoError), s.getError());

    delete (pSerCom);
}

// Tests if error is set properly if trying to write an nbiot string to the buffer
// without specifying a write function

TEST (SerialTest, WriteNbiotStringError) {
    Serial s;
    nbiot::string my_string("cccc");

    EXPECT_EQ(0, s.write(my_string));
    EXPECT_EQ(((int) Serial::NoWriteFunction), s.getError());
}
