#ifndef INETWORK_H
#define INETWORK_H

class INetwork
{
public:
    virtual ~INetwork() {}//;

    virtual bool connect(const char* hostname, unsigned short port) = 0;
    virtual int read(unsigned char* buffer, int len, unsigned short timeout) = 0;
    virtual bool write(unsigned char* buffer, unsigned long len, unsigned short timeout) = 0;
    virtual bool disconnect() = 0;
    virtual bool isConnected() = 0;
};

//INetwork::~INetwork()
//{
//}
#endif // INETWORK_H

