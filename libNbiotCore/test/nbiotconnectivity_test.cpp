//
// Created by Yassine Amraue on 05.02.18.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <nbiotconnectivity.h>

// Tests that an NbiotConnectivity object can be constructed from an instance of class Serial.
// Expect that the constructed object is not null.

TEST(NbiotConnectivityTest, ConstructObjectFromSerial) {

}

// Tests that connect() sends out the right NSOCR command and reads out the modem response.

TEST(NbiotConnectivityConnectTest, ConnectToHostnameAndPort) {

}

// Tests that after connecting to a host, multiple arriving +NSOMNIs are intercepted properly and
// the UrcCallback is called.

TEST(NbiotConnectivityConnectTest, InterceptNsomniUrc) {

}

// Tests that connect() can connect be called multiple times without breaking.

TEST(NbiotConnectivityConnectTest, ConnectMultipleTimes) {

}

// Tests that connect() returns false if a serial error occurred.

TEST(NbiotConnectivityConnectTest, FalseOnSerialError) {

}

// Tests that disconnect() closes the connection. Expect that you can establish a new connection
// after disconnection.

TEST(NbiotConnectivityDisconnectTest, DisconnectAlreadyConnected) {

}

// Tests that disconnect() closes the connection. Expect that you can establish a new connection
// after disconnection.

TEST(NbiotConnectivityDisconnectTest, DisconnectNotYetConnected) {

}

// Tests that disconnect() returns false if a serial error occurred.

TEST(NbiotConnectivityDisconnectTest, FalseOnSerialError) {

}

// Tests that getFilterResult() stores the number of bytes available given by the last NSOMNI

TEST(NbiotConnectivityGetFilterResultTest, ReadAvailableBytes) {

}

// Tests that getFilterResult() stores 0 bytes available if strFilterResult is empty

TEST(NbiotConnectivityGetFilterResultTest, EmptyFilterResultString) {

}

// Tests that getFilterResult() stores 0 bytes available if strFilterResult is not an NSOMNI but
// just an arbitrary string.

TEST(NbiotConnectivityGetFilterResultTest, NoNsomniResultString) {

}

// Tests that the message is sent to the host:port given after the last connect().
// Expect that write returns true if the message was sent properly.

TEST(NbiotConnectivityWriteTest, SendMessageToHost) {

}

// Tests the behaviour of write() if the message is an empty string.

TEST(NbiotConnectivityWriteTest, SendEmptyMessageToHost) {

}

// Tests the behaviour of write() if buffer is a nullptr

TEST(NbiotConnectivityWriteTest, NullptrBuffer) {

}

// Tests that write() returns false if a serial error occurred.

TEST(NbiotConnectivityWriteTest, FalseOnSerialError) {

}

// Tests that write() returns false if there is no open connection.

TEST(NbiotConnectivityWriteTest, FalseIfNoConnectionEstablished) {

}

// Tests that read() returns -1 if there is no open connection.

TEST(NbiotConnectivityReadTest, NoConnectionEstablished) {

}

// Tests that read() returns -1 if a serial error occurred.

TEST(NbiotConnectivityReadTest, SerialError) {

}

// Tests that read() can read a packet if the modem received one and the specified length is
// exactly the length of the received packet.

TEST(NbiotConnectivityReadTest, ReadFromModemExactPacket) {

}

// Tests that read() can read a part of a packet if the modem received one and the specified length is
// smaller than the length of the received packet.

TEST(NbiotConnectivityReadTest, ReadFromModemPartialPacket) {

}

// Tests that read() can read a packet if the modem received one and the specified length is
// greater than the length of the received packet.

TEST(NbiotConnectivityReadTest, ReadFromModemMoreThanReceivedPacket) {

}

// Tests that read() returns -1 if trying to read although there is no packet received from the modem

TEST(NbiotConnectivityReadTest, ReadFromModemNoPacketReceived) {

}