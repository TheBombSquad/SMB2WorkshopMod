#pragma once

#include <mkb.h>
#include "list.h"
#include "log.h"
#include "heap.h"

template <typename T>
struct Element {
    Element* next = nullptr;
    T* val;
};

template <typename T>
class List {
public:
    void append(T* obj);
    void remove_first(T* obj);
    bool empty();
    Element<T>* first = nullptr;
};


template<typename T>
void List<T>::append(T* obj)
{
    Element<T>* e = new Element<T>(nullptr, obj);
    if (empty()) {
        first = e;
        return;
    }
    else {
        Element<T>* i = first;
        while (i->next != nullptr) {
            i = i->next;
        }
        i->next = e;
    }
    mkb::OSReport("appending %X to list\n", obj);
}

// This won't remove duplicate values
template<typename T>
void List<T>::remove_first(T* obj)
{
    Element<T>* i = first;
    if (i != nullptr && i->val == obj) {
        if (i->next == nullptr) {
            first = nullptr;
        }
        else {
            first = i->next;
        }
        heap::free(i);
        heap::free(obj);
        return;
    }

    else {
        while (i->next != nullptr) {
            if (i->next->val == obj) {
                if (i->next->next != nullptr) {
                    Element<T>* n = i->next->next;
                    heap::free(i->next);
                    i->next = n;
                }
                else {
                    i->next = nullptr;
                }
                heap::free(obj);
                mkb::OSReport("destroyed %X\n", obj);
                return;
            }
            i = i->next;
        }
    }

    mkb::OSReport("[mod] WARNING: Could not remove element %X from list as it was not found in the list\n", obj);
}

template<typename T>
bool List<T>::empty()
{
    return (first == nullptr);
}

