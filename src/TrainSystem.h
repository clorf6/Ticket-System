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
#include "vector.h"
#include "Exception.h"
#include "LinkedHashMap.h"
#include "_string.h"
#include "_time.h"

using trainID = string<21>;
using station = string<41>;

const int kMaxStationNum = 100;
const int INF = 2e9;

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

struct Train {
    bool Release;
    int StationNum, SeatNum;
    int Prices[kMaxStationNum];
    int ArrivalTimes[kMaxStationNum];
    int DepartureTimes[kMaxStationNum - 1];
    char Type;
    trainID TrainID;
    station Stations[kMaxStationNum + 1];
    class time StartTime;
    date StartDate, EndDate;

    Train() = default;

    Train(const trainID& _TrainID, const int& _StationNum, const int& _SeatNum, station *_Stations,
          int *_Prices, const class time& _StartTime, int *TravelTimes, int *StopoverTimes,
          const date& _StartDate, const date& _Enddate, const char& _Type) :
          TrainID(_TrainID), StationNum(_StationNum), SeatNum(_SeatNum), StartTime(_StartTime),
          StartDate(_StartDate), EndDate(_Enddate), Type(_Type), Release(false) {
        StartTime.now = StartDate;
        for (int i = 1; i <= StationNum; i++) Stations[i] = _Stations[i];
        Prices[1] = 0;
        for (int i = 1; i < StationNum; i++) Prices[i + 1] = Prices[i] + _Prices[i];
        int now_time = 0;
        ArrivalTimes[1] = DepartureTimes[1] = 0;
        for (int i = 1; i < StationNum; i++) {
            now_time += TravelTimes[i];
            ArrivalTimes[i + 1] = now_time;
            now_time += StopoverTimes[i];
            DepartureTimes[i + 1] = now_time;
        }
    }
};

extern Train now_train, nex_train;
const size_t kSizeofTrain = sizeof(Train);

struct Station {
    trainID TrainID;
    int TrainPos;
    int Num;

    Station() = default;

    Station(const trainID& _TrainID, const int& _TrainPos, const int& _Num) :
            TrainID(_TrainID), TrainPos(_TrainPos), Num(_Num) {};

    bool operator == (const Station &other) const {
        return Num == other.Num && TrainID == other.TrainID;
    }

    bool operator < (const Station &other) const {
        return TrainID == other.TrainID ? Num < other.Num : TrainID < other.TrainID;
    }

    bool operator > (const Station &other) const {
        return TrainID == other.TrainID ? Num > other.Num : TrainID > other.TrainID;
    }

    bool operator <= (const Station &other) const {
        return TrainID == other.TrainID ? Num <= other.Num : TrainID < other.TrainID;
    }

    bool operator >= (const Station &other) const {
        return TrainID == other.TrainID ? Num >= other.Num : TrainID > other.TrainID;
    }

    bool operator != (const Station &other) const {
        return Num != other.Num || TrainID != other.TrainID;
    }
};

extern Station now_station;
const size_t kSizeofStation = sizeof(Station);

struct Ticket {
    int StationNum;
    int TicketNum[kMaxStationNum + 1];

    Ticket() = default;

    Ticket(const int& _StationNum, const int& _TicketNum) : StationNum(_StationNum) {
        std::fill(TicketNum + 1, TicketNum + StationNum, _TicketNum);
    }

    int Query(int l, int r) {
        int ret = INF;
        for (int i = l; i < r; i++) {
            if (TicketNum[i] < ret) ret = TicketNum[i];
        }
        return ret;
    }

    void Modify(int l, int r, int val) {
        for (int i = l; i < r; i++) TicketNum[i] += val;
    }
};

extern Ticket now_ticket, nex_ticket;
const size_t kSizeofTicket = sizeof(Ticket);

struct Direct_Ticket {
    trainID TrainID;
    class time ArrivalTime, DepartureTime;
    int Price, Seat, Time;

    Direct_Ticket() = default;

    Direct_Ticket(const trainID& _TrainID, const class time& _ArrivalTime,
                 const class time& _DepartureTime, const int& _Price,
                 const int& _Seat, const int& _Time) :
                 TrainID(_TrainID), ArrivalTime(_ArrivalTime), DepartureTime(_DepartureTime),
                 Price(_Price), Seat(_Seat), Time(_Time) {}
};

struct Transfer_Ticket {
    Direct_Ticket ticket1, ticket2;
    int WaitTime;
    int Time, Price;
    station Transfer_station;

    Transfer_Ticket() = default;

    Transfer_Ticket(const Direct_Ticket& _ticket1, const Direct_Ticket& _ticket2,
                    const int& _WaitTime, const station& _Transfer_station) :
                    ticket1(_ticket1), ticket2(_ticket2),
                    WaitTime(_WaitTime), Transfer_station(_Transfer_station) {
        Time = ticket1.Time + ticket2.Time + WaitTime;
        Price = ticket1.Price + ticket2.Price;
    }
};

class CompTime {
public:
    bool operator ()(const Direct_Ticket& x, const Direct_Ticket& y) const {
        return x.Time == y.Time ? x.TrainID < y.TrainID : x.Time < y.Time;
    }
    bool operator ()(const Transfer_Ticket& x, const Transfer_Ticket& y) const {
        if (x.Time != y.Time) return x.Time < y.Time;
        if (x.Price != y.Price) return x.Price < y.Price;
        if (x.ticket1.TrainID != y.ticket1.TrainID) return x.ticket1.TrainID < y.ticket1.TrainID;
        return x.ticket2.TrainID < y.ticket2.TrainID;
    }
};

extern CompTime compTime;

class CompCost {
public:
    bool operator ()(const Direct_Ticket& x, const Direct_Ticket& y) const {
        return x.Price == y.Price ? x.TrainID < y.TrainID : x.Price < y.Price;
    }
    bool operator ()(const Transfer_Ticket& x, const Transfer_Ticket& y) const {
        if (x.Price != y.Price) return x.Price < y.Price;
        if (x.Time != y.Time) return x.Time < y.Time;
        if (x.ticket1.TrainID != y.ticket1.TrainID) return x.ticket1.TrainID < y.ticket1.TrainID;
        return x.ticket2.TrainID < y.ticket2.TrainID;
    }
};

extern CompCost compCost;

extern class time now_time, nex_time, transfer_now_time, transfer_nex_time;
extern sjtu::vector<Station> StartStations;
extern sjtu::vector<Direct_Ticket> ret;
extern Transfer_Ticket best, now_transfer;

class TrainSystem {
public:
    BPlusTree<trainID, int> train_pos;
    FileSystem<Train> train_data;
    BPlusTree<std::pair<trainID, int>, int> ticket_pos;
    FileSystem<Ticket> ticket_data;
    BPlusTree<station, Station> station_data;
    LinkedHashMap<station, int, HashString> transfer;

    TrainSystem();

    void AddTrain(const trainID &, const int &, const int &, station *,
                  int *, const class time &, int *, int *,
                  const date &, const date &, const char &);

    void DeleteTrain(const trainID &);

    void ReleaseTrain(const trainID &);

    void QueryTrain(const trainID &, const date &);

    void QueryTicket(const station &, const station &,const date &, const bool &);

    void QueryTransfer(const station &, const station &,const date &, const bool &);

    void Clear();

    ~TrainSystem() = default;
};

#endif //TICKETSYSTEM_TRAINSYSTEM_H
