//
// Created by 屋顶上的小丑 on 2023/4/17.
//

#ifndef TICKETSYSTEM__STRING_H
#define TICKETSYSTEM__STRING_H

#include <cstdio>
#include <string>
#include <iostream>

template<size_t size>
class string {
public:
    char index[size]{};

    string() {
        memset(index, 0, size);
    }

    string(const std::string &Index) {
        strcpy(index, Index.c_str());
    }

    string(const char *Index) {
        strcpy(index, Index);
    }

    template<size_t other_size>
    string(const string<other_size> &other) {
        strcpy(index, other.index);
    }

    string& operator = (const std::string& other) {
        other.copy(index, size, 0);
        return (*this);
    }

    string& operator = (const char *Index) {
        strcpy(index, Index);
        return (*this);
    }

    bool operator==(const string &x) const {
        return !strcmp(index, x.index);
    }

    bool operator<(const string &x) const {
        return strcmp(index, x.index) < 0;
    }

    bool operator>(const string &x) const {
        return strcmp(index, x.index) > 0;
    }

    bool operator<=(const string &x) const {
        return strcmp(index, x.index) <= 0;
    }

    bool operator>=(const string &x) const {
        return strcmp(index, x.index) >= 0;
    }

    bool operator!=(const string &x) const {
        return strcmp(index, x.index) != 0;
    }

    operator std::string() const {
        return index;
    }

    bool empty() const {
        return !strlen(index);
    }

    void clear() {
        memset(index, 0, size);
    }

    ~string() = default;
};

template<size_t size>
std::ostream &operator<<(std::ostream &out, const string<size> &x) {
    out << x.index;
    return out;
}

class HashString {
public:
    template<size_t size>
    size_t operator () (const string<size> &str) const {
        size_t ret = 0, x = 0;
        for (int i = 0; i < strlen(str.index); i++) {
            ret = (ret << 4) + str.index[i];
            if ((x = ret & 0xF000000L) != 0) {
                ret ^= (x >> 24);
                ret &= ~x;
            }
        }
        return (ret & 0x7FFFFFFF);
    }
};

#endif //TICKETSYSTEM__STRING_H
