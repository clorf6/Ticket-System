//
// Created by 屋顶上的小丑 on 2023/4/17.
//

#ifndef TICKETSYSTEM__STRING_H
#define TICKETSYSTEM__STRING_H

template<size_t size>
class string {
public:
    char index[size]{};

    string() {
        memset(index, 0, size);
    }

    string(const std::string &Index) {
        Index.copy(index, size, 0);
    }

    string(const char *Index) {
        memset(index, 0, size);
        strcpy(index, Index);
    }

    template<size_t other_size>
    string(const string<other_size> &other) {
        memcpy(index, other.index, other_size);
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

template<size_t size>
std::ostream &operator<<(std::ostream &out, const string<size> &x) {
    out << x.index;
    return out;
}

#endif //TICKETSYSTEM__STRING_H
