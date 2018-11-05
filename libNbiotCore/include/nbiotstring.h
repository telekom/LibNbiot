/* ========================================================================
 * LibNbiot: nbiotstring.h
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

#ifndef STRING_H
#define STRING_H

#include <string.h>

namespace nbiot
{

class StringList;

unsigned char h2c(char);
char c2h(unsigned char);

class string
{
public:
    /*!
     * \brief string constructs an empty string. Allocates one byte for the terminating zero in data.
     */
    string();
    /*!
     * \brief string copy constructor
     * \param str
     */
    string(const string& str);
    /*!
     * \brief string
     * \param str
     * \param len
     */
    string(const char* str, size_t len = npos);
    /*!
     * \brief d'tor
     */
    ~string();

    /*!
     * \brief operator =
     * \param str
     * \return
     */
    string& operator = (const string& str);
    /*!
     * \brief operator =
     * \param str
     * \return
     */
    string& operator = (const char* str);

    /*!
     * \brief operator +=
     * \param str
     * \return
     */
    string& operator += (const string& str)
    {
        append(str);
        return *this;
    }

    /*!
     * \brief operator +=
     * \param str
     * \return
     */
    string& operator += (const char* str)
    {
        append(str);
        return *this;
    }

    /*!
     * \brief operator +=
     * \param c
     * \return
     */
    string& operator += (const char c)
    {
        push_back(c);
        return *this;
    }

    /*!
     * \brief operator ==
     * \param str
     * \return
     */
    bool operator == (const string& str) const
    {
        return *this == str.data;
    }

    /*!
     * \brief operator ==
     * \param str
     * \return
     */
    bool operator == (const char* str) const;

    /*!
     * \brief operator []
     * \param index
     * \return
     */
    char operator [](size_t index) const
    {
        if((sz > index))
        {
            return data[index];
        }
        else
        {
            return 0;
        }
    }

    /*!
     * \brief at
     * \param index
     * \return
     */
    char at(size_t index) const
    {
        if((sz > index))
        {
            return data[index];
        }
        else
        {
            return 0;
        }
    }

    /*!
     * \brief c_str
     * \return
     */
    const char* c_str() const
    {
        return data;
    }

    /*!
     * \brief getData
     * \return
     */
    char* getData()
    {
        return data;
    }

    /*!
     * \brief size
     * \return
     */
    size_t size() const
    {
        return sz;
    }

    /*!
     * \brief empty
     * \return
     */
    bool empty() const
    {
        return (0 == sz);
    }

    /*!
     * \brief push_back
     * \param c
     */
    void push_back(char c);
    /*!
     * \brief take_front
     * \return
     */
    char take_front();
    /*!
     * \brief find
     * \param str
     * \return
     */
    size_t find(const string& str) const
    {
        return find(str.data);
    }
    /*!
     * \brief find
     * \param str
     * \return
     */
    size_t find(const char* str) const;
    /*!
     * \brief find
     * \param c
     * \return
     */
    size_t find(const char c) const;
    /*!
     * \brief substr
     * \param pos
     * \param len
     * \return
     */
    string substr (size_t pos = 0, size_t len = npos) const;

    /*!
     * \brief append
     * \param str
     * \param len
     */
    void append(const char* str, size_t len = npos);
    /*!
     * \brief append
     * \param str
     */
    void append(const string& str);

    /*!
     * \brief split
     * \param sep
     * \param keepSeparator
     * \return
     */
    StringList split(char sep, bool keepSeparator=false) const;
    /*!
     * \brief split
     * \param sep
     * \param keepSeparator
     * \return
     */
    StringList split(const char* sep, bool keepSeparator=false) const;

    /*!
     * \brief compareNoCase
     * \param str
     * \return
     */
    bool compareNoCase(const char* str) const;

    /*!
     * \brief copy copies string content to a char pointer destination. The memory at the destination must be allocated before.
     * \param dest destination char*
     * \param len maximum bytes to copy. The destination needs one more byte of space for the terminating zero.
     * \return number of copied bytes
     *
     * Example:
     * \code
     * char chPtr[16];
     * string str("Hello World!");
     *
     * size_t copied = str.copy(chPtr, 15); // max 15 bytes
     * \endcode
     */
    size_t copy(char* dest, size_t len) const;

    /*!
     * \brief copy copies string content to an unsigned char pointer destination. The memory at the destination must be allocated before.
     * \param dest destination unsigned char*
     * \param len maximum bytes to copy. The destination needs one more byte of space for the terminating zero.
     * \return number of copied bytes
     */
    size_t copy(unsigned char* dest, size_t len) const;

    /*!
     * \brief toHex
     * \return
     */
    string toHex() const;

    /*!
     * \brief join
     * \param list
     * \param pos
     * \param len
     * \return
     */
    static string join(StringList list, size_t pos = 0, size_t len = npos);

    /*!
     * \brief fromHex
     * \param hex
     * \return
     */
    static string fromHex(const char* hex);

    /*!
     * \brief Printf
     * \param format
     * \return
     */
    static string Printf(const char* format, ...);

    /*!
     * \brief npos
     */
    static const size_t	npos;

    // if you write directly to data, you need to set the size after
    /*!
     * \brief setSize
     * \param len
     */
    void setSize(size_t len) { if(len < mem_size) { sz = len; } }

    /*!
     * \brief check7bit
     * \return
     */
    bool check7bit() const;

    /*!
     * \brief toLower
     * \return
     */
    string toLower() const;

    /*!
     * \brief strip
     * \param c
     * \return
     */
    string strip(char c) const;

    /*!
     * \brief valid
     * \return
     */
    bool valid() const { return (nullptr != data); }

private:
    void clear();
    void dup(const char* str, size_t len = npos);

    char* data;
    size_t sz;
    size_t mem_size;

    static const size_t PRINTF_BUFFER_SIZE = 80;
};

}

#endif // STRING_H
