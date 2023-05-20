//
// Created by 屋顶上的小丑 on 2023/5/4.
//

#ifndef TICKETSYSTEM_POOL_H
#define TICKETSYSTEM_POOL_H

#include <iostream>
#include <fstream>
#include <cstring>
#include "vector.h"
#include "FileSystem.h"

class Pool {
public:
    FileSystem<int> Pool_data;
    sjtu::vector<int> pool;
    int Pool_count;
    int Node_count;
    static const int kSizeofInt = sizeof(int);
    static const int kAppendixSize = 2 * kSizeofInt;

    Pool(const std::string &file_name): Pool_data(file_name + ".pool") {
        if (Pool_data.empty()) {
            Pool_count = Node_count = 0;
        } else {
            Pool_data.file.seekg(-kAppendixSize, std::ios::end);
            Pool_data.file.read(reinterpret_cast<char *>(&Node_count), kSizeofInt);
            Pool_data.file.read(reinterpret_cast<char *>(&Pool_count), kSizeofInt);
            int pool_now = 0;
            for (int i = 1; i <= Pool_count; i++) {
                Pool_data.Read(i, pool_now);
                pool.push_back(pool_now);
            }
        }
    }

    ~Pool() {
        Pool_count = pool.size();
        for (int i = 0; i < Pool_count; i++) Pool_data.Write(i + 1, pool[i]);
        Pool_data.file.seekp(0, std::ios::end);
        Pool_data.file.write(reinterpret_cast<char *>(&Node_count), kSizeofInt);
        Pool_data.file.write(reinterpret_cast<char *>(&Pool_count), kSizeofInt);
    }

    int Alloc() {
        if (pool.empty()) return (++Node_count);
        int ret = pool.back();
        pool.pop_back();
        return ret;
    }

    void Recycle(int& ret) {
        pool.push_back(ret);
    }

    void clear() {
        Pool_count = Node_count = 0;
        pool.clear();
        Pool_data.clear();
    }
};

#endif //TICKETSYSTEM_POOL_H
