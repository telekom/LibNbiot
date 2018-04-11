/* ========================================================================
 * LibNbiot: nbiotstring.cpp
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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h> // for va_list
#include <ctype.h> // for tolower
#include "nbiotstringlist.h"

#include "nbiotdebug.h"

namespace nbiot {

unsigned char h2c(char c)
{
    unsigned char ret = 0;
    if(('0' <= c) && ('9' >= c))
    {
        ret = c - '0';
    }
    else if (('a' <= c) && ('f' >= c)) {
        ret = c - 'a' + 0x0a;
    }
    else if (('A' <= c) && ('F' >= c)) {
        ret = c - 'A' + 0x0a;
    }
    return ret;
}

char c2h(unsigned char c)
{
    char ret = '0';
    if(9 >= c)
    {
        ret += c;
    }
    else if((0xa <= c) && (0xf >= c))
    {
        ret = 'A' + (c - 0xa);
    }
    return ret;
}

const size_t string::npos = static_cast<size_t>(-1);

// constructs an empty zero terminated string
string::string() :
    data(NULL),
    sz(0),
    mem_size(0)
{
    data = (char*)malloc(sizeof(char));
    if(NULL != data)
    {
        data[0] = 0;
        mem_size = sizeof(char);
#ifdef DEBUG_STRING
        stringMem++;
#endif
    }
}

string::string(const string& str) :
    data(NULL),
    sz(0),
    mem_size(0)
{
    dup(str.data, str.sz);
}

string::string(const char* str, size_t len) :
    data(NULL),
    sz(0),
    mem_size(0)
{
    dup(str, len);
}

string::~string()
{
    free(data);
#ifdef DEBUG_STRING
    stringMem -= (sz + 1);
#endif
}

string& string::operator = (const string& str)
{
    dup(str.data, str.sz);
    return *this;
}

string& string::operator = (const char* str)
{
    dup(str);
    return *this;
}

bool string::operator == (const char* str) const
{
    bool ret = false;
    if((NULL == data) && NULL == str)
    {
        ret = true;
    }
    else
    {
        if((NULL != data) && NULL != str)
        {
            if(0 == strcmp(data, str))
            {
                ret = true;
            }
        }
    }
    return ret;
}

void string::push_back(char c)
{
    size_t newSize = sz + 1;
    newSize++; // terminating zero

    char* tmp = (char*)realloc(data, newSize * sizeof(char));
    if(NULL != tmp)
    {
        tmp[sz] = c;
        tmp[sz + 1] = 0;
        data = tmp;
        sz++;
        mem_size = newSize;
#ifdef DEBUG_STRING
        stringMem++;
#endif
    }
#ifdef DEBUG_STRING
    else
    {
        debugPrintf("CAN'T REALLOC MEMORY (%d bytes)\r\n", newSize);
    }
#endif
}

char string::take_front()
{
    char ret = 0;
    if(0 < sz)
    {
        ret = data[0];
        sz--;
        if(0 == sz)
        {
            clear();
        }
        else
        {
            memmove(data, &data[1], sz);
            data[sz] = 0;
        }
    }
    return ret;
}

size_t string::find(const char* str) const
{
    size_t result = npos;
    if(valid() && (nullptr != str))
    {
        const char* found = strstr(data, str);
        if(NULL != found)
        {
            result = static_cast<size_t>(found - data);
        }
    }
    return result;
}

size_t string::find(const char c) const
{
    size_t result = npos;
    if(valid())
    {
        const char* found = strchr(data, static_cast<int>(c));
        if(NULL != found)
        {
            result = static_cast<size_t>(found - data);
        }
    }
    return result;
}

string string::substr (size_t pos, size_t len) const
{
    if(sz <= pos)
    {
        return string();
    }
    if(sz < (pos + len))
    {
        len = sz - pos;
    }
    return string(&data[pos], len);
}

void string::append(const char* str, size_t len)
{
    if(npos == len)
    {
        len = (nullptr != str)?strlen(str):0;
    }

    size_t mem_incr = 0;
    char* tmp = nullptr;
    if(0 != len)
    {
        if(mem_size < (sz + len + 1))
        {
            tmp = (char*)realloc(data, (sz + len + 1) * sizeof(char));
            mem_incr = (sz + len + 1) - mem_size;
        }
        else
        {
            tmp = data;
        }
    }

    if(nullptr != tmp)
    {
        if(nullptr != str)
        {
            if(len)
            {
                memcpy(&tmp[sz], str, len);
            }
            tmp[sz + len] = 0;
            sz += len;
        }
        else
        {
            memset(&tmp[sz], 0, len * sizeof(char));
        }
        data = tmp;
        mem_size += mem_incr;
#ifdef DEBUG_STRING
        stringMem += mem_incr;
#endif
    }
}

void string::append(const string& str)
{
    append(str.data, str.size());
}

StringList string::split(char sep, bool keepSeparator) const
{
    StringList ret;
    size_t last_pos = 0;
    for(size_t i = 0; i < sz; ++i)
    {
        if(data[i] == sep)
        {
            if(i > last_pos)
            {
                ret.append(substr(last_pos, (i - last_pos)));
                last_pos = i;
            }
            if(keepSeparator)
            {
                ret.append(substr(last_pos, 1));
            }
            last_pos++;
        }
    }
    if((0 < last_pos) && (sz > last_pos))
    {
        ret.append(substr(last_pos));
    }
    if(ret.isEmpty())
    {
        ret.append(substr());
    }
    return ret;
}

StringList string::split(const char* sep, bool keepSeparator) const
{
    StringList ret;
    if(nullptr != sep)
    {
        size_t last_pos = 0;
        size_t sep_len = strlen(sep);
        if(0 < sep_len)
        {
            for(size_t i = 0; i < sz; ++i)
            {
                for(size_t j = 0; j < sep_len; ++j)
                {
                    if(data[i] == sep[j])
                    {
                        if(i > last_pos)
                        {
                            ret.append(substr(last_pos, (i - last_pos)));
                            last_pos = i;
                        }
                        if(keepSeparator)
                        {
                            ret.append(substr(last_pos, 1));
                        }
                        last_pos++;
                        break;
                    }
                }
            }
            if((0 < last_pos) && (sz > last_pos))
            {
                ret.append(substr(last_pos));
            }
        }
    }
    if(ret.isEmpty())
    {
        ret.append(substr());
    }
    return ret;
}

bool string::compareNoCase(const char* str) const
{
    bool ret = false;
    if(valid() && (nullptr != str))
    {
        ret = (0 == strcmp(string(str).toLower().c_str(), toLower().c_str()));
    }
    else
    {
        if((nullptr == str) && (0 == sz))
        {
            ret = true;
        }
    }
    return ret;
}

size_t string::copy(char* dest, size_t len) const
{
    size_t i = 0;
    if((nullptr != dest) && (0 < len))
    {
        while((i < len) && (i < sz))
        {
            dest[i] = data[i];
            ++i;
        }
        dest[i] = 0;
    }
    return i;
}

size_t string::copy(unsigned char* dest, size_t len) const
{
    size_t i = 0;
    if((nullptr != dest) && (0 < len))
    {
        while((i < len) && (i < sz))
        {
            dest[i] = static_cast<unsigned char>(data[i]);
            ++i;
        }
        dest[i] = 0;
    }
    return i;
}

void string::clear()
{
    if(NULL != data)
    {
        free(data);
        data = NULL;
#ifdef DEBUG_STRING
        stringMem -= (sz + 1);
#endif
        sz = 0;
        mem_size = 0;
    }
}

string string::toHex() const
{
    string ret;
    for(size_t i = 0; i < sz; ++i)
    {
        char hiNib = c2h((static_cast<unsigned char>(data[i]) >> 4) & 0x0f);
        ret.push_back(hiNib);
        char loNib = c2h(static_cast<unsigned char>(data[i]) & 0x0f);
        ret.push_back(loNib);
    }
    return ret;
}

string string::join(StringList list, size_t pos, size_t len)
{
    string ret;
    if(npos == len)
    {
        len = list.count();
    }
    for(size_t i = pos; i < len; ++i)
    {
        ret.append(list[i]);
    }
    return ret;
}

string string::fromHex(const char* hex)
{
    string ret;
    size_t len = strlen(hex);
    size_t i = 0;
    while(i < len)
    {
        char c = (h2c(hex[i++]) << 4);
        if(i < len)
        {
            c |= h2c(hex[i++]);
        }
        ret.push_back(c);
    }
    return ret;
}

string string::Printf(const char* format, ...)
{
    char buffer[PRINTF_BUFFER_SIZE];
    va_list vl;
    va_start(vl, format);
    int n = vsnprintf((char*)buffer, PRINTF_BUFFER_SIZE, format, vl);
    va_end(vl);
    string ret;
    if((0 < n) && (PRINTF_BUFFER_SIZE > (unsigned int)n))
    {
        ret = string(buffer, static_cast<size_t>(n));
    }
    return ret;
}

bool string::check7bit() const
{
    bool ret = true;
    size_t i = 0;
    while( i < sz )
    {
        if((0x20 > data[i]) || (0x7e < data[i]))
        {
            ret = false;
            break;
        }
        ++i;
    }
    return ret;
}

string string::toLower() const
{
    string ret;
    for(size_t i = 0; i < sz; ++i)
    {
        ret.push_back(static_cast<char>(tolower(static_cast<int>(data[i]))));
    }
    return ret;
}

void string::dup(const char* str, size_t len)
{
    clear();
    if(npos == len)
    {
        len = (nullptr == str)?0:strlen(str);
    }
    data = (char*)malloc((len + 1) * sizeof(char));
    if(NULL != data)
    {
        if(nullptr != str)
        {
            memcpy(data, str, len);
            data[len] = 0;
            sz = len;
        }
        else
        {
            memset(data, 0, (len + 1) * sizeof(char));
            sz = 0;
        }
        mem_size = (len + 1);
#ifdef DEBUG_STRING
        stringMem += (len + 1);
#endif
    }
}

}
