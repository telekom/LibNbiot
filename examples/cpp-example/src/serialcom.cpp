/* ========================================================================
 * LibNbiot: serialcom.cpp
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

#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <sys/ioctl.h>
#include <sys/termios.h>
#include <fcntl.h>
#include "serialcom.h"

bool SerialCom::connect() {

    if (!m_deviceName.empty()) {
        int flags = O_RDWR | O_NDELAY;// | O_NONBLOCK;
        m_fd = open(m_deviceName.c_str(), flags);
        if (0 <= m_fd) {
            m_isConnected = true;
            setOptions();
        } else {
            m_fd = -1;
            m_isConnected = false;
        }
    }
    return m_isConnected;
}

unsigned char SerialCom::readByte() {
    unsigned char ret = 0;
    if (m_isConnected) {
        int byteRead = ::read(m_fd, &ret, 1);
        if (1 != byteRead) {
            ret = 0;
        }
    }
    return ret;
}

ReadStatus SerialCom::readStatus() {
    ReadStatus ret = rx_empty;
    if (m_isConnected) {
        ret = rx_avail;
    }
    return ret;
}

void SerialCom::writeByte(unsigned char byte) {
    if (m_isConnected) {
        ::write(m_fd, &byte, 1);
    }
}

WriteStatus SerialCom::writeStatus() {
    WriteStatus ret = tx_full;
    if (m_isConnected) {
        ret = tx_empty;
    }
    return ret;
}

bool SerialCom::disconnect() {
    return !m_isConnected;
}

void SerialCom::setOptions() {
    struct termios newtio;
    if (tcgetattr(m_fd, &newtio) != 0) {
        std::cerr << "tcgetattr() 3 failed" << std::endl;
    }

    cfsetospeed(&newtio, m_settings.baud);
    cfsetispeed(&newtio, m_settings.baud);

    /* We generate mark and space parity ourself. */
    if (m_settings.databits == 7 && (m_settings.parity == "Mark" || m_settings.parity == "Space")) {
        m_settings.databits = 8;
    }
    switch (m_settings.databits) {
        case 5:
            newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS5;
            break;
        case 6:
            newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS6;
            break;
        case 7:
            newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS7;
            break;
        case 8:
        default:
            newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS8;
            break;
    }
    newtio.c_cflag |= CLOCAL | CREAD;

    //parity
    newtio.c_cflag &= ~(PARENB | PARODD);
    if (m_settings.parity == "Even") {
        newtio.c_cflag |= PARENB;
    } else if (m_settings.parity == "Odd") {
        newtio.c_cflag |= (PARENB | PARODD);
    }

    newtio.c_cflag &= ~CRTSCTS;

    //stopbits
    if (m_settings.stop == 2) {
        newtio.c_cflag |= CSTOPB;
    } else {
        newtio.c_cflag &= ~CSTOPB;
    }

    newtio.c_iflag = IGNBRK;

    //software handshake
    if (m_settings.softwareHandshake) {
        newtio.c_iflag |= IXON | IXOFF;
    } else {
        newtio.c_iflag &= ~(IXON | IXOFF | IXANY);
    }

    newtio.c_lflag = 0;
    newtio.c_oflag = 0;

    newtio.c_cc[VTIME] = 1;
    newtio.c_cc[VMIN] = 60;

    if (tcsetattr(m_fd, TCSANOW, &newtio) != 0) {
        std::cerr << "tcsetattr() 1 failed" << std::endl;
    }

    int mcs = 0;
    ioctl(m_fd, TIOCMGET, &mcs);
    mcs |= TIOCM_RTS;
    ioctl(m_fd, TIOCMSET, &mcs);

    if (tcgetattr(m_fd, &newtio) != 0) {
        std::cerr << "tcgetattr() 4 failed" << std::endl;
    }

    //hardware handshake
    if (m_settings.hardwareHandshake) {
        newtio.c_cflag |= CRTSCTS;
    } else {
        newtio.c_cflag &= ~CRTSCTS;
    }
    if (tcsetattr(m_fd, TCSANOW, &newtio) != 0) {
        std::cerr << "tcsetattr() 2 failed" << std::endl;
    }

}

SerialCom *pSerCom = nullptr;

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
        ret = pSerCom->readStatus();
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
        ret = pSerCom->writeStatus();
    }
    return ret;
}
