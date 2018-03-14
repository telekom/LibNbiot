#ifndef IMODEM_H
#define IMODEM_H

namespace nbiot
{
class string;
}

class IModem
{
public:
    enum ModemState
    {
        MODEM_OFF = 0,
        MODEM_ON
    };

    virtual ~IModem() {}

    virtual bool reboot() = 0;
    virtual bool initialize() = 0;
    virtual bool attach() = 0;
    virtual bool detach() = 0;
    virtual bool isAttached() = 0;
    virtual bool sendAtCommand(const char* command, char* response, size_t len, unsigned short timeout) = 0;
    virtual bool getImsi(nbiot::string& imsi, bool usePw = false) = 0;
    virtual const char* getDeviceId() = 0;
    virtual ModemState switchModem(ModemState state) = 0;
    virtual ModemState isModemOn() = 0;

};
#endif // IMODEM_H
