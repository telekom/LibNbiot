/* ========================================================================
 * LibNbiot 'C' example: serialcom.h
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

#ifndef SERIALCOM_
#define SERIALCOM_

#include <termios.h>
#include <string>
#include "readstatus.h"
#include "writestatus.h"

struct SerialSettings
{
    SerialSettings():baud(9600),databits(8),parity("None"),stop(1),softwareHandshake(false),hardwareHandshake(false) {}

    speed_t baud;
    unsigned databits;
    std::string parity;
    unsigned stop;
    bool softwareHandshake;
    bool hardwareHandshake;
};

class SerialCom
{
public:
    explicit SerialCom(const char* device):
        m_fd(-1),
        m_deviceName(device),
        m_isConnected(false),
        m_settings()
    {}
    virtual ~SerialCom() {}

    bool connect();

    unsigned char readByte();
    ReadStatus readStatus();

    void writeByte(unsigned char byte);
    WriteStatus writeStatus();

    bool disconnect();

    bool isConnected() const { return m_isConnected; }

private:
    SerialCom();
    SerialCom(const SerialCom&);
    SerialCom& operator = (const SerialCom&);

    void setOptions();

    int m_fd;
    std::string m_deviceName;
    bool m_isConnected;
    SerialSettings m_settings;
};

#endif // SERIALCOM_

