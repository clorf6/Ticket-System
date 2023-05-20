//
// Created by 屋顶上的小丑 on 2023/5/4.
//

#ifndef TICKETSYSTEM_CACHE_H
#define TICKETSYSTEM_CACHE_H

#include <iostream>
#include <cstdio>
#include <functional>
#include "LinkedHashMap.h"
#include "List.h"
#include "FileSystem.h"

const int kCacheCapacity = 1 << 20;

template<class T, class Hash>
class LRUCache {
public:
    FileSystem<T>* file;
    int capacity;
    list<std::pair<int, T>> cache;
    LinkedHashMap<int, typename list<std::pair<int, T>>::iterator, Hash> map;
    LRUCache(FileSystem<T>* _file, const int& Capacity = kCacheCapacity / sizeof(T)) :
    file(_file), capacity(Capacity), map(capacity << 1) {};

    void dump() {
        typename list<std::pair<int, T>>::iterator it = cache.front();
        std::pair<int, T> now;
        while (it != cache.end()) {
            now = *it;
            file->Write(now.first, now.second);
            it++;
        }
    }

    void get(int key, T& now) {
        if (!map.find(key)) {
            file->Read(key, now);
            put(key, now);
            return ;
        }
        std::pair<int, T> ret(*map[key]);
        now = ret.second;
        cache.erase(map[key]);
        cache.push_front(std::make_pair(key, now));
        map[key] = cache.front();
    }

    void put(int key, const T& val) {
        if (!map.find(key)) {
            if (cache.size() == capacity) {
                std::pair<int, T> now = cache.back();
                file->Write(now.first, now.second);
                map.erase(now.first);
                cache.pop_back();
            }
        } else cache.erase(map[key]);
        cache.push_front(std::make_pair(key, val));
        map[key] = cache.front();
    }

    void clear() {
        cache.clear();
        map.clear();
    }
};

#endif //TICKETSYSTEM_CACHE_H
