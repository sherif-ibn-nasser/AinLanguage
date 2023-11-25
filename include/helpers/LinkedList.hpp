#pragma once
#include <functional>
#include "SharedPtrTypes.hpp"
#include "LinkedListNode.hpp"
template<class T>
struct LinkedList{
    SharedLinkedListNode<T> head;
    SharedLinkedListNode<T> tail;
    int size;

    LinkedList();

    /**
    @returns the index of inserted node of @param val
    */
    int insert(T val);

    /**
    @returns the index of inserted @param node
    */
    int insert(SharedLinkedListNode<T> node);

    /**
    @returns the index of last inserted item
    */
    int insert(LinkedList<T> list);

    void forEachIndexed(std::function<void(SharedLinkedListNode<T>,int)> f);
    
    void forEachIndexed(std::function<void(T,int)> f);

    void forEach(std::function<void(SharedLinkedListNode<T>)> f);

    void forEach(std::function<void(T)> f);

    bool isEmpty();
};

template<typename T>
LinkedList<T>::LinkedList():size(0){}

template<typename T>
int LinkedList<T>::insert(T val){
    
    auto node=std::make_shared<LinkedListNode<T>>(val);

    return this->insert(node);
}

template<typename T>
int LinkedList<T>::insert(SharedLinkedListNode<T> node){
    if(isEmpty())
        this->head=node;
    else
        this->tail->next=node;
    
    this->tail=node;
    
    size++;

    return size-1;
}

template<typename T>
int LinkedList<T>::insert(LinkedList<T> list){
    if(isEmpty())
        this->head=list.head;
    else
        this->tail->next=list.head;
    
    this->tail=list.tail;
    
    size+=list.size;

    return size-1;
}


template<typename T>
void LinkedList<T>::forEachIndexed(std::function<void(SharedLinkedListNode<T>,int)> f){
    auto it=this->head;
    for(int i=0;i<size;i++){
        f(it,i);
        it=it->next;
    }
}

template<typename T>
void LinkedList<T>::forEachIndexed(std::function<void(T,int)> f){
    forEachIndexed([&](SharedLinkedListNode<T> node,int index){
        f(node->val,index);
    });
}

template<typename T>
void LinkedList<T>::forEach(std::function<void(SharedLinkedListNode<T>)> f){
    forEachIndexed([&](SharedLinkedListNode<T> node,int index){
        f(node);
    });
}

template<typename T>
void LinkedList<T>::forEach(std::function<void(T)> f){
    forEachIndexed([&](T val,int index){
        f(val);
    });
}

template<typename T>
bool LinkedList<T>::isEmpty(){
    return this->size==0;
}