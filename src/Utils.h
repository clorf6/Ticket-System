//
// Created by 屋顶上的小丑 on 2023/5/4.
//

#ifndef TICKETSYSTEM_UTILS_H
#define TICKETSYSTEM_UTILS_H

#include <string>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <functional>
#include "TrainSystem.h"

template<class T, class comp = std::less<T>>
void sort(sjtu::vector<T>& data, int l, int r, const comp& cmp) {
    if (l >= r) return ;
    T pivot = data[(l + r) >> 1];
    int i = l - 1, j = r + 1;
    while (i < j) {
        do ++i; while (cmp(data[i], pivot));
        do --j; while (cmp(pivot, data[j]));
        if (i < j) std::swap(data[i], data[j]);
    }
    sort(data, l, j, cmp);
    sort(data, j + 1, r, cmp);
}

#endif //TICKETSYSTEM_UTILS_H
