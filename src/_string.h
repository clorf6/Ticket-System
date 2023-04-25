//
// Created by 屋顶上的小丑 on 2023/4/17.
//

#ifndef TICKETSYSTEM__STRING_H
#define TICKETSYSTEM__STRING_H

const int kMaxIndexLength = 64;
const size_t kSizeofIndex = 64;

class string {
public:
    char index[kMaxIndexLength]{};

    string() {
        memset(index, 0, kSizeofIndex);
    }

    string(const std::string &Index) {
        Index.copy(index, kSizeofIndex, 0);
    }

    string (const char* Index) {
        memset(index, 0, kSizeofIndex);
        strcpy(index, Index);
    }

    string(const string &other) {
        memcpy(index, other.index, kSizeofIndex);
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

    ~string() = default;
};

std::ostream &operator<<(std::ostream &out, const string &x) {
    out << x.index;
    return out;
}

#endif //TICKETSYSTEM__STRING_H
