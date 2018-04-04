/* ========================================================================
 * LibNbiot: nbiotregistrylist.h
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

