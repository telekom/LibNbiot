/******************************************************************************
 * File: listelement.h
 * Author: Edgar Hindemith
 * This file is part of the 'Simple Statemachine for Embedded Systems'
 * see https://github.com/edgar4k/StateMachine
 *
 * Copyright (C) 2016,2018 Edgar Hindemith
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#ifndef LISTELEMENT_H
#define LISTELEMENT_H

namespace util
{

template <typename E>
class ListElement
{
public:
    ListElement() : data(),next(nullptr) { }
    virtual ~ListElement() { if(next) { delete next; } }

    E data;
    ListElement* next;
private:
    ListElement(const ListElement&);
    ListElement& operator = (const ListElement&);
};

template <typename T>
class ListIterator
{
public:
    ListIterator() : element(nullptr),invalidItem() {}
    ListIterator(ListElement<T>* e) : element(e),invalidItem() {}
    ListIterator(ListElement<T>* e, const T& invalid) : element(e),invalidItem(invalid) {}
    ListIterator(const ListIterator& other) : element(other.element),invalidItem(other.invalidItem) {}
    virtual ~ListIterator() {}

    ListIterator& operator=(const ListIterator& other)
    {
        element = other.element;
        invalidItem = other.invalidItem;
        return *this;
    }

    ListIterator& operator++()
    {
        if(nullptr != element)
        {
            element = element->next;
        }
        return *this;
    }

    ListIterator operator++(int)
    {
        ListIterator tmp(*this);
        operator++();
        return tmp;
    }

    bool operator==(const ListIterator& other)
    {
        return (other.element == element);
    }

    bool operator!=(const ListIterator& other)
    {
        return (other.element != element);
    }

    T& operator*()
    {
        return (nullptr != element)?
                    element->data :
                    invalidItem;
    }

    ListIterator next()
    {
        return (nullptr != element)?
                    ListIterator(element->next) :
                    ListIterator();
    }

private:
    ListElement<T>* element;
    T invalidItem;
};

} // end namespace util

#endif // LISTELEMENT_H

