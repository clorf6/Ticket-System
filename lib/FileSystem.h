//
// Created by 屋顶上的小丑 on 2023/5/4.
//

#ifndef TICKETSYSTEM_FILESYSTEM_H
#define TICKETSYSTEM_FILESYSTEM_H

#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>

template<class T>
class FileSystem {
public:
    std::fstream file;
    const int kSizeofData = sizeof(T);

    FileSystem(const std::string &file_name) {
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        }
    }

    ~FileSystem() {
        file.close();
    }

    void Read(int pos, T &ret) {
        file.seekg((pos - 1) * kSizeofData);
        file.read(reinterpret_cast<char *>(&ret), kSizeofData);
    }

    void Write(int pos, T &ret) {
        file.seekp((pos - 1) * kSizeofData);
        file.write(reinterpret_cast<char *>(&ret), kSizeofData);
    }

    bool empty() {
        file.seekp(0, std::ios::end);
        return !file.tellp();
    }
};

#endif //TICKETSYSTEM_FILESYSTEM_H
