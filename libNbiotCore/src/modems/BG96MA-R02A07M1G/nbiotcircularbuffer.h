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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either expressed or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ========================================================================
*/


#ifndef LIBNBIOTOS_NBIOTCIRCULARBUFFER_H
#define LIBNBIOTOS_NBIOTCIRCULARBUFFER_H

#include <cstddef>

namespace nbiot
{
    template <size_t max_size>
    class CircularBuffer
    {
    public:
        CircularBuffer():_max(max_size) { reset(); }

        void reset();

        void put(unsigned char data);

        bool get(unsigned char*);

        bool empty() const { return (!m_full && (m_head == m_tail)); }

        bool full() const { return m_full; }

        size_t capacity() const { return _max; };

        size_t size() const;

    private:
        void advance();

        void retreat();

        unsigned char m_buffer[max_size];
        size_t m_head;
        size_t m_tail;
        size_t _max; // capacity of the buffer
        bool m_full;
    };
}

namespace nbiot
{
    template <size_t max_size>
    void CircularBuffer<max_size>::reset() {
        m_head = 0;
        m_tail = 0;
        m_full = false;
    }

    template <size_t max_size>
    size_t CircularBuffer<max_size>::size() const {
        size_t size = _max;

        if(!m_full)
        {
            if(m_head >= m_tail)
            {
                size = (m_head - m_tail);
            }
            else
            {
                size = (_max + m_head - m_tail);
            }
        }

        return size;
    }

    template <size_t max_size>
    void CircularBuffer<max_size>::advance() {
        if(m_full)
        {
            m_tail = (m_tail + 1) % _max;
        }

        m_head = (m_head + 1) % _max;
        m_full = (m_head == m_tail);
    }

    template <size_t max_size>
    void CircularBuffer<max_size>::retreat() {
        m_full = false;
        m_tail = (m_tail + 1) % _max;
    }

    template <size_t max_size>
    void CircularBuffer<max_size>::put(unsigned char data) {
        m_buffer[m_head] = data;

        advance();
    }

    template <size_t max_size>
    bool CircularBuffer<max_size>::get(unsigned char* data) {
        int r = false;

        if(data != nullptr && !empty())
        {
            *data = m_buffer[m_tail];
            retreat();

            r = true;
        }

        return r;
    }
}

#endif //LIBNBIOTOS_NBIOTCIRCULARBUFFER_H
