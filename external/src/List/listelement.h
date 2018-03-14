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

