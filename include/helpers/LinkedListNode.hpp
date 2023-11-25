#pragma once
#include "SharedPtrTypes.hpp"
#include <memory>
template<class T>
struct LinkedListNode{
    T val;
    SharedLinkedListNode<T> next;
    LinkedListNode(T val):val(val){}
};