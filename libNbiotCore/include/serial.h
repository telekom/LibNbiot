#ifndef SERIAL_H
#define SERIAL_H
#include <stdio.h> // for size_t and NULL
#include "readstatus.h"
#include "writestatus.h"

#include "nbiotstring.h"
#include "urcfilterlist.h"

class Serial
{
public:
    enum SerialError
    {
        NoError = 0,
        ReadError,
        WriteError,
        LineParsingError,
        ReadTimeout,
        NoReadFunction,
        NoWriteFunction
    };

    enum SerialReadMode
    {
        Normal,
        Simple,
        Ignore
    };

    Serial();
    Serial(int dbgprf);
    Serial(const Serial& other);

    virtual ~Serial() {}

    Serial& operator=(const Serial& other);

    void configSetReadStatus(ReadStatus (*readstatus_fu)());
    void configSetReadByte(unsigned char (*readbyte_fu)());
    void configSetPutChar(void (*putchar_fu)(unsigned char));
    void configSetWriteStatus(WriteStatus (*writestatus_fu)());

    int getError() const { return serial_error; }
    void clearError() { setError(NoError); }

    size_t readLine(nbiot::string& line, unsigned short timeout_ms, SerialReadMode readMode=Normal);
    size_t readRaw(unsigned char* buffer, unsigned short timeout_ms);

    size_t write(char c);

    size_t write(const char* buffer, size_t size);
    size_t write(const nbiot::string& str);

    template<class UHC>
    bool addUrcFilter(const char* urc, UHC* uhc, void (UHC::*urc_handler)(const char*))
    {
        int index = -1;
        bool ret = false;

        if(0 > (index = m_filterList.indexOf(urc)))
        {
            nbiot::UrcFilter filter(urc);
            filter.setHandler(uhc, urc_handler);
            m_filterList.append(filter);
            ret = true;
        }
        else
        {
            m_filterList[index].setHandler(uhc, urc_handler);
        }

        return ret;
    }

    bool addUrcFilter(const char* urc, void (*urc_handler)(const char*));
    bool removeUrcFilter(const char* urc);

    void clearFilter() { m_filterList.clear(); }
    bool hasFilter() const { return !m_filterList.isEmpty(); }

    void setDebugPrefix(int prefix) { dbg_prefix = prefix; }
    int getDebugPrefix() const { return dbg_prefix; }

private:
    ReadStatus (*serialReadStatus)();
    unsigned char (*serialReadByte)();
    void (*serialPutChar)(unsigned char);
    WriteStatus (*serialWriteStatus)();

    void setError(int err) { serial_error = err; }
    void waitForTx() const;

    int serial_error;

    nbiot::UrcFilterList m_filterList;

    int dbg_prefix;

    static const long long txBufferWaitTimeout = 1000;
};

#endif // SERIAL_H
