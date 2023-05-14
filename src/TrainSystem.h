//
// Created by 屋顶上的小丑 on 2023/5/11.
//

#ifndef TICKETSYSTEM_TRAINSYSTEM_H
#define TICKETSYSTEM_TRAINSYSTEM_H

#include <string>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <fstream>
#include "BPlusTree.h"
#include "Exception.h"
#include "LinkedHashMap.h"
#include "_string.h"

using trainID = string<21>;
using stations = string<41>;

const int kMaxStationNum = 100;

struct Train {
    int StationNum;
    int Prices[kMaxStationNum + 1];
    trainID TrainID;
    stations Stations[kMaxStationNum + 1];

};

#endif //TICKETSYSTEM_TRAINSYSTEM_H
