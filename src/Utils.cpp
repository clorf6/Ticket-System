//
// Created by 屋顶上的小丑 on 2023/5/20.
//
#ifndef TICKETSYSTEM_UTILS_CPP
#define TICKETSYSTEM_UTILS_CPP

#include "Utils.h"

sjtu::vector<std::string> ops;

void DivideOperation(const std::string &Op) {
    now_op.clear();
    ops.clear();
    for (auto &i: Op) {
        if (i == ' ') {
            if (!now_op.empty()) ops.push_back(now_op);
            now_op.clear();
        } else now_op += i;
    }
    if (!now_op.empty()) ops.push_back(now_op);
}

void DivideStation(station* Stations, const std::string& str) {
    int cnt = 0;
    now_op.clear();
    for (auto &i : str) {
        if (i == '|') {
            Stations[++cnt] = now_op;
            now_op.clear();
        } else now_op += i;
    }
    if (!now_op.empty()) Stations[++cnt] = now_op;
}

void DivideNumber(int* data, const std::string& str) {
    int cnt = 0;
    now_op.clear();
    for (auto &i : str) {
        if (i == '|') {
            data[++cnt] = std::stoi(now_op);
            now_op.clear();
        } else now_op += i;
    }
    if (!now_op.empty()) data[++cnt] = std::stoi(now_op);
}

void GetDate(date& now, const std::string& str) {
    now.month = std::stoi(str.substr(0, 2));
    now.day = std::stoi(str.substr(3, 2));
}

void GetTime(class time& now, const std::string& str) {
    now.hour = std::stoi(str.substr(0, 2));
    now.minute = std::stoi(str.substr(3, 2));
}

void GetSaleDate(date& StartDate, date& EndDate, const std::string& str) {
    GetDate(StartDate, str);
    GetDate(EndDate, str.substr(6));
}

#endif //TICKETSYSTEM_UTILS_CPP
