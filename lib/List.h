//
// Created by 屋顶上的小丑 on 2023/4/24.
//

#ifndef TICKETSYSTEM_LIST_H
#define TICKETSYSTEM_LIST_H

#include <cstdio>

template<class T>
class list {
private:
    struct node {
        T data;
        node* nex;
        node* pre;
        node() : nex(nullptr), pre(nullptr) {};
        node(const T& Data, node* Nex = nullptr, node* Pre = nullptr) :
             data(Data), nex(Nex), pre(Pre) {};
    };

    node* head, *tail;
    size_t length;
public:
    list() {
        head = new node;
        tail = new node;
        head->nex = tail;
        tail->pre = head;
        length = 0;
    }

    ~list() {
        node* now = head, *nex = nullptr;
        while (now) {
            nex = now->nex;
            delete now;
            now = nex;
        }
        head = tail = nullptr;
    }

    class iterator {
        friend class list;
        private:
            node* now;
        public:
            iterator(node* it = nullptr) : now(it) {}
            iterator(const iterator& it) : now(it.now) {};
            iterator& operator ++() {
                now = now->nex;
                return (*this);
            }
            iterator& operator --() {
                now = now->pre;
                return (*this);
            }
            iterator operator ++(int) {
                iterator ret(*this);
                now = now->nex;
                return ret;
            }
            iterator operator --(int) {
                iterator ret(*this);
                now = now->pre;
                return ret;
            }
            T& operator *() {
                return now->data;
            }
            bool operator ==(const iterator& other) {
                return now == other.now;
            }
            bool operator !=(const iterator& other) {
                return now != other.now;
            }
    };

    size_t size() {
        return length;
    }

    iterator front() {
        return iterator(head->nex);
    }

    T back() {
        return tail->pre->data;
    }

    iterator end() {
        return iterator(tail);
    }


    void push_front(T val) {
        node* new_node = new node(val, head->nex, head);
        head->nex->pre = new_node;
        head->nex = new_node;
        length++;
    }

    void erase(iterator it) {
        it.now->pre->nex = it.now->nex;
        it.now->nex->pre = it.now->pre;
        length--;
        delete it.now;
        it.now = nullptr;
    }

    void pop_back() {
        erase(iterator(tail->pre));
    }

};

#endif //TICKETSYSTEM_LIST_H
