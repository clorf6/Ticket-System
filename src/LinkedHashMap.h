//
// Created by 屋顶上的小丑 on 2023/4/24.
//

#ifndef TICKETSYSTEM_LINKEDHASHMAP_H
#define TICKETSYSTEM_LINKEDHASHMAP_H

#include <cstdio>
#include <iostream>
#include <functional>
#include "List.h"

const size_t kMapSize = 1000;

template<class Key, class T>
class LinkedHashMap {
friend class List;
private:
    struct node {
        Key key;
        T val;
        node* nex;
        node() : nex(nullptr) {};
    };

    node** head;
    size_t siz;
public:
    LinkedHashMap(const size_t& _siz = kMapSize) : siz(kMapSize) {
        head = new node* [_siz];
        for (int i = 0; i < _siz; i++) {
            head[i] = nullptr;
        }
    }

    ~LinkedHashMap() {
        for (int i = 0; i < siz; i++) {
            node *now = head[i], *nex;
            while (now) {
                nex = now->nex;
                delete now;
                now = nex;
            }
            now = nullptr;
        }
        delete []head;
    }


    T& operator[](const Key& _key) {
        unsigned long long hash_val = _key % kMapSize;
        node* now = head[hash_val];
        while (now) {
            if (now->key == _key) {
                return now->val;
            }
            now = now->nex;
        }
        now = new node();
        now->key = _key;
        now->nex = head[hash_val];
        head[hash_val] = now;
        return now->val;
    }

    bool find(const Key& key) const {
        unsigned long long hash_val = key % kMapSize;
        node* now = head[hash_val];
        while (now) {
            if (now->key == key) return true;
            now = now->nex;
        }
        return false;
    }

    void erase(const Key& key) {
        unsigned long long hash_val = key % kMapSize;
        node* now = head[hash_val], *pre = nullptr;
        while (now) {
            if (now->key == key) {
                if (!pre) head[hash_val] = now->nex;
                else pre->nex = now->nex;
                delete now;
                return ;
            }
            pre = now;
            now = now->nex;
        }
    }
};

#endif //TICKETSYSTEM_LINKEDHASHMAP_H
