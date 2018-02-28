#ifndef NBIOTSTRINGLIST_H
#define NBIOTSTRINGLIST_H

#include <list.h>
#include "nbiotstring.h"

namespace nbiot
{
class StringList : public util::list<nbiot::string>
{
};
}
#endif // NBIOTSTRINGLIST_H

