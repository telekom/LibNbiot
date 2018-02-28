#ifndef URCFILTERLIST_H
#define URCFILTERLIST_H

#include <nbiotstring.h>
#include <list.h>
#include "FP.h"

namespace nbiot {

typedef void (*urchandler)(const char*);

class UrcFilter
{
public:
    UrcFilter() : urc(), handler() {}
    UrcFilter(const char* str) : urc(str), handler() {}
    UrcFilter(const char* str, urchandler urch) : urc(str), handler()
    {
        handler.attach(urch);
    }
    UrcFilter(const UrcFilter& other) : urc(other.urc), handler(other.handler) {}

    virtual ~UrcFilter() {}

    UrcFilter& operator = (const UrcFilter& other)
    {
        urc = other.urc;
        handler = other.handler;
        return *this;
    }

    bool operator == (const char* str) { return (urc == str); }
    bool operator == (const nbiot::string& str) { return (0 == str.find(urc)); }

    template<class UHC>
    void setHandler(UHC* uhc, void (UHC::*urch)(const char*))
    {
        if(handler.attached())
        {
            handler.detach();
        }
        handler.attach(uhc, urch);
    }

    void setHandler(urchandler urch)
    {
        if(handler.attached())
        {
            handler.detach();
        }
        handler.attach(urch);
    }

    bool isValid()
    {
        return ((!urc.empty()) && (handler.attached()));
    }

    string urc;
    FP<void, const char*> handler;
};

class UrcFilterList : public util::list<UrcFilter>
{
public:
    int indexOf(const char* urcName) const
    {
        return util::list<UrcFilter>::indexOf(urcName);
    }

    int indexOf(const nbiot::string& urcName) const
    {
        return util::list<UrcFilter>::indexOf(urcName);
    }
};

} // end of ns nbiot

#endif // URCFILTERLIST_H

