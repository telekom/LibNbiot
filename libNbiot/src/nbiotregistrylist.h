#ifndef NBIOTREGISTRYLIST
#define NBIOTREGISTRYLIST

#include <list.h>
#include <nbiotdebug.h>

#include "nbiottopic.h"

namespace nbiot {

class NbiotRegistryList : public util::list<nbiot::NbiotTopic>
{
public:
    int indexOf(const char* topicName) const
    {
        return util::list<nbiot::NbiotTopic>::indexOf(topicName);
    }

    int indexOf(char topicShort[2]) const
    {
        return util::list<nbiot::NbiotTopic>::indexOf(topicShort);
    }

    int indexOf(unsigned short topicId) const
    {
        return util::list<nbiot::NbiotTopic>::indexOf(topicId);
    }
};

} // end of ns nbiot

#endif // NBIOTREGISTRYLIST

