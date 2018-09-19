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

        bool empty() const { return (!_full && (_head == _tail)); }

        bool full() const { return _full; }

        size_t capacity() const { return _max; };

        size_t size() const;

    private:
        void _advance();

        void _retreat();

        unsigned char _buffer[max_size];
        size_t _head;
        size_t _tail;
        size_t _max; // capacity of the buffer
        bool _full;
    };
}

namespace nbiot
{
    template <size_t max_size>
    void CircularBuffer<max_size>::reset() {
        _head = 0;
        _tail = 0;
        _full = false;
    }

    template <size_t max_size>
    size_t CircularBuffer<max_size>::size() const {
        size_t size = _max;

        if(!_full)
        {
            if(_head >= _tail)
            {
                size = (_head - _tail);
            }
            else
            {
                size = (_max + _head - _tail);
            }
        }

        return size;
    }

    template <size_t max_size>
    void CircularBuffer<max_size>::_advance() {
        if(_full)
        {
            _tail = (_tail + 1) % _max;
        }

        _head = (_head + 1) % _max;
        _full = (_head == _tail);
    }

    template <size_t max_size>
    void CircularBuffer<max_size>::_retreat() {
        _full = false;
        _tail = (_tail + 1) % _max;
    }

    template <size_t max_size>
    void CircularBuffer<max_size>::put(unsigned char data) {
        _buffer[_head] = data;

        _advance();
    }

    template <size_t max_size>
    bool CircularBuffer<max_size>::get(unsigned char* data) {
        int r = false;

        if(data != nullptr && !empty())
        {
            *data = _buffer[_tail];
            _retreat();

            r = true;
        }

        return r;
    }
}

#endif //LIBNBIOTOS_NBIOTCIRCULARBUFFER_H
