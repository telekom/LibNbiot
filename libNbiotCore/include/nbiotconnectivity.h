#ifndef NBIOTCONNECTIVITY_H
#define NBIOTCONNECTIVITY_H
#include <time.h>
#include "atcommands.h"
#include "nbiotstring.h"
#include "serial.h"
#include "inetwork.h"

class NbiotConnectivity:public INetwork
{
public:
    explicit NbiotConnectivity(Serial& serial);
    virtual ~NbiotConnectivity() {}

    virtual bool connect(const char* hostname, int port);
    virtual int read(unsigned char* buffer, int len, unsigned short timeout_ms);
    virtual bool write(unsigned char* buffer, unsigned long len, unsigned short timeout);
    virtual bool disconnect();
    virtual bool isConnected() { return (0 <= m_connectionNumber); }

    void parseFilterResult(const char* strFilterResult);

private:
    NbiotConnectivity(const NbiotConnectivity&);
    NbiotConnectivity& operator=(const NbiotConnectivity&);

    unsigned short ipAvailable();
    int ipRead(nbiot::string& data, int len, unsigned short timeout_ms);

    AtCommands m_cmd;
    int m_connectionNumber;
    nbiot::string m_hostname;
    int m_port;
    size_t m_bytesAvail;
    nbiot::string m_nsonmi;
    int m_lastListenPort;


    static const unsigned int readInterval = 100;
    static const unsigned int oneSecond = 1000;
    static const unsigned int threeSeconds = 3000;
    static const unsigned int tenSeconds = 10000;

    static const int maxPort = 65536;
    static const int listenPortBase = 10001;
    static const unsigned short byteCountMask = 0xFFFF;

    static const int nsorfRespIdx_connNum = 0;
    static const int nsorfRespIdx_bytesAvail = 3;
    static const int nsorfRespIdx_data = 4;
    static const int nsorfRespIdx_bytesRem = 5;

    static const char* cmdNSOCR_arg;
    static const char* respNSONMI_arg;
    static const char* cmdNSORF_arg;
    static const char* cmdNSOST_arg;
    static const char* cmdNSOCL_arg;
};

#endif // NBIOTCONNECTIVITY_H
