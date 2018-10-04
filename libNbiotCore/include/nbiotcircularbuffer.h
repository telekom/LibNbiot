/* ========================================================================
 * LibNbiot: nbiotcircularbuffer.h
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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ========================================================================
*/


#ifndef LIBNBIOTOS_NBIOTCIRCULARBUFFER_H
#define LIBNBIOTOS_NBIOTCIRCULARBUFFER_H

#include <cstddef>
#include <cstring>

namespace nbiot
{
    template <size_t max_size>
    class CircularBuffer
    {
    public:
        explicit CircularBuffer(){ reset(); }

        ~CircularBuffer() = default;

        void reset();

        bool put(unsigned char data);

        bool put(const unsigned char* data, size_t len);

        bool get(unsigned char&);

        bool get(unsigned char* data, size_t len);

        bool empty() const { return (!m_full && (m_head == m_tail)); }

        bool full() const { return m_full; }

        size_t capacity() const { return (max_size - size()); }

        size_t size() const;

    private:

    CircularBuffer(const CircularBuffer&) = delete;
    CircularBuffer& operator=(CircularBuffer&) = delete;

        void advance(size_t count=1);

        void retreat(size_t count=1);

        unsigned char m_buffer[max_size];
        size_t m_head;
        size_t m_tail;
        bool m_full;
    };
}

namespace nbiot
{
    template <size_t max_size>
    void CircularBuffer<max_size>::reset()
    {
        m_head = 0;
        m_tail = 0;
        m_full = false;
    }

    template <size_t max_size>
    size_t CircularBuffer<max_size>::size() const
    {
        size_t size = max_size;

        if(!m_full)
        {
            if(m_head >= m_tail)
            {
                size = (m_head - m_tail);
            }
            else
            {
                size = (max_size + m_head - m_tail);
            }
        }

        return size;
    }

    template <size_t max_size>
    void CircularBuffer<max_size>::advance(size_t count)
    {
        if(m_full)
        {
            m_tail = (m_tail + count) % max_size;
        }

        m_head = (m_head + count) % max_size;
        m_full = (m_head == m_tail);
    }

    template <size_t max_size>
    void CircularBuffer<max_size>::retreat(size_t count)
    {
        m_full = false;
        m_tail = (m_tail + count) % max_size;
    }

    template <size_t max_size>
    bool CircularBuffer<max_size>::put(unsigned char data)
    {
        bool r = false;

        if(!full())
        {
            m_buffer[m_head] = data;
            advance();

            r = true;
        }

        return r;
    }

    template <size_t max_size>
    bool CircularBuffer<max_size>::put(const unsigned char* data, size_t len)
    {
        bool r = false;

        if(capacity() >= len)
        {
            if((max_size - m_head) > len)
            {
                memcpy(&m_buffer[m_head], data, len);
                advance(len);
            }
            else
            {
                size_t l1 = (max_size - m_head);
                memcpy(&m_buffer[m_head], data, l1);
                advance(l1);

                size_t l2 = len - l1;
                memcpy(&m_buffer[m_head], &data[l1], l2);
                advance(l2);
            }

            r = true;
        }

        return r;
    }

    template <size_t max_size>
    bool CircularBuffer<max_size>::get(unsigned char& data)
    {
        bool r = false;

        if(!empty())
        {
            data = m_buffer[m_tail];
            retreat();

            r = true;
        }

        return r;
    }

    template <size_t max_size>
    bool CircularBuffer<max_size>::get(unsigned char* data, size_t len)
    {
        bool r = false;

        if(size() >= len)
        {
            if((max_size - m_tail) > len)
            {
                memcpy(data, &m_buffer[m_tail], len);
                retreat(len);
            }
            else
            {
                size_t l1 = (max_size - m_tail);
                memcpy(data, &m_buffer[m_tail], l1);
                retreat(l1);

                size_t l2 = len - l1;
                memcpy(&data[l1], &m_buffer[m_tail], l2);
                retreat(l2);
            }

            r = true;
        }

        return r;
    }
}

#endif //LIBNBIOTOS_NBIOTCIRCULARBUFFER_H
