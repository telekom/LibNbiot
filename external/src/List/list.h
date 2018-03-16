#ifndef LIST_H
#define LIST_H

#include "listelement.h"

namespace util {


template <typename T>
class list
{
public:
    list() : elements(nullptr),lastElement(nullptr),invalidItem(),size(0)
    {
    }

    list(const T& invalid) : elements(nullptr),lastElement(nullptr),invalidItem(invalid),size(0)
    {
    }

    list(const list<T>& other) : elements(nullptr),lastElement(nullptr),invalidItem(other.invalidItem),size(0)
    {
        appendCopy(other);
    }

    ~list()
    {
        clear();
    }

    list<T>& operator = (const list<T>& other)
    {
        if(!isEmpty())
        {
            clear();
        }
        appendCopy(other);
        invalidItem = other.invalidItem;
        return *this;
    }


    void append(const T& item)
    {
        ListElement<T>* e = new ListElement<T>;
        e->data = (item);
        if(isEmpty())
        {
            elements = e;
        }
        else
        {
            lastElement->next = (e);
        }
        lastElement = e;
        size++;
    }

    const T& at(int index) const
    {
        if(validIndex(index))
        {
            ListElement<T>* tmp = element(index);
            return tmp->data;
        }
        else
        {
            return invalidItem;
        }
    }

    T& operator [](int index)
    {
        if(validIndex(index))
        {
            ListElement<T>* tmp = element(index);
            return tmp->data;
        }
        else
        {
            return invalidItem;
        }
    }

    void remove(int index)
    {
        if(validIndex(index))
        {
            ListElement<T>* current = element(index);
            ListElement<T>* next = static_cast<ListElement<T>*>(current->next);
            current->next = (nullptr);
            delete current;
            if(0 == index)
            {
                elements = next;
            }
            else
            {
                // valid and not 0 -> index - 1 is valid too
                ListElement<T>* previous = element(index - 1);
                previous->next = next;
            }
            size--;
        }
    }

    T take_front()
    {
        T ret = invalidItem;
        if(!isEmpty())
        {
            ListElement<T>* current = element(0);
            ListElement<T>* next = static_cast<ListElement<T>*>(current->next);
            current->next = (nullptr);
            ret = current->data;
            delete current;
            elements = next;
            size--;
        }
        return ret;
    }

    void pop()
    {
        if(!isEmpty())
        {
            ListElement<T>* current = element(0);
            ListElement<T>* next = static_cast<ListElement<T>*>(current->next);
            current->next = (nullptr);
            delete current;
            elements = next;
            size--;
        }
    }

    void push(const T& item)
    {
        ListElement<T>* e = new ListElement<T>;
        e->data = (item);
        if(!isEmpty())
        {
            ListElement<T>* firstElement = elements;
            e->next = (firstElement);
        }
        elements = e;
        size++;
    }

    template <typename compT>
    const T& find(const compT& item) const
    {
        if(!isEmpty())
        {
            ListElement<T>* tmp = elements;
            do
            {
                if(tmp->data == item)
                {
                    return tmp->data;
                }
                tmp = static_cast<ListElement<T>*>(tmp->next);
            }
            while(nullptr != tmp);
        }
        return invalidItem;
    }

    template <typename compT>
    int indexOf(const compT& item) const
    {
        int ret = -1;
        if(!isEmpty())
        {
            int n = 0;
            ListElement<T>* tmp = elements;
            do
            {
                if(tmp->data == item)
                {
                    ret = n;
                    break;
                }
                tmp = static_cast<ListElement<T>*>(tmp->next);
                n++;
            }
            while(nullptr != tmp);
        }
        return ret;
    }

    void clear()
    {
        if(!isEmpty())
        {
            delete elements;
            elements = nullptr;
            lastElement = nullptr;
            size = 0;
        }
    }

    int count() const
    {
        return size;
    }

    bool isEmpty() const { return (nullptr == elements); }

    ListIterator<T> begin() { return ListIterator<T>(elements, invalidItem); }
    ListIterator<T> cbegin() const { return ListIterator<T>(elements, invalidItem); }

    ListIterator<T> end() { return (ListIterator<T>(nullptr)); }
    ListIterator<T> cend() const { return (ListIterator<T>(nullptr)); }

private:
    ListElement<T>* element(int index) const
    {
        ListElement<T>* ret = elements;
        for(int n = 0; n < index; ++n)
        {
            ret = static_cast<ListElement<T>*>(ret->next);
        }
        return ret;
    }


    void appendCopy(const list<T>& other)
    {
        if(!other.isEmpty())
        {
            ListElement<T>* otherElement = other.elements;
            int otherSize = other.count();

            if(isEmpty())
            {
                elements = new ListElement<T>;
                elements->data = (otherElement->data);
                lastElement = elements;
                size = 1;
                otherSize--;
                otherElement = static_cast<ListElement<T>*>(otherElement->next);
            }

            while(0 < otherSize)
            {
                ListElement<T>* e = new ListElement<T>;
                e->data = (otherElement->data);
                lastElement->next = (e);
                otherElement = static_cast<ListElement<T>*>(otherElement->next);
                lastElement = e;
                size++;
                otherSize--;
            }
        }
    }

    bool validIndex(int index) const
    {
        return ( (0 <= index) && (index < size) );
    }

    ListElement<T>* elements;
    ListElement<T>* lastElement;
    T invalidItem;
    int size;
};
} // end namespace util
#endif // LIST_H

