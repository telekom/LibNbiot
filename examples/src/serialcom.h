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
    virtual ~SerialCom() = default;

    bool connect();

    unsigned char readByte();
    ReadStatus readStatus();

    void writeByte(unsigned char byte);
    WriteStatus writeStatus();

    bool disconnect();

    bool isConnected() const { return m_isConnected; }

private:
    SerialCom() {};
    SerialCom(const SerialCom&);
    SerialCom& operator = (const SerialCom&);

    void setOptions();

    int m_fd;
    std::string m_deviceName;
    bool m_isConnected;
    SerialSettings m_settings;
};

#ifdef __cplusplus
extern "C" {
#endif

unsigned char readByte();

ReadStatus readStatus();

void writeByte(unsigned char byte);

WriteStatus writeStatus();

#ifdef __cplusplus
}
#endif

#endif // SERIALCOM_

