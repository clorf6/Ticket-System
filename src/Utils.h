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
#include "Exception.h"
#include "vector.h"
#include "_time.h"

extern std::string ops[105];
extern int op_num;
static std::string now_op;

void DivideOperation(const std::string &);

void DivideStation(station *, const std::string &);

void DivideNumber(int *, const std::string &);

void GetDate(date &, const std::string &);

void GetTime(class time &, const std::string &);

void GetSaleDate(date &, date &, const std::string &);

#endif //TICKETSYSTEM_UTILS_H
