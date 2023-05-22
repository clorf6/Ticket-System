//
// Created by 屋顶上的小丑 on 2023/5/17.
//

#ifndef TICKETSYSTEM_TICKETSYSTEM_H
#define TICKETSYSTEM_TICKETSYSTEM_H

#include "UserSystem.h"
#include "TrainSystem.h"
#include "Exception.h"
#include "Utils.h"

enum status {
    SUCCESS, PENDING, REFUNDED
};

struct Order {
    username UserName;
    trainID TrainID;
    station StartStation;
    station EndStation;
    class time ArrivalTime, DepartureTime;
    int Price, TicketNum, Delta;
    int StartNum, EndNum;
    status Status;

    Order() = default;

    Order(const username& _UserName, const trainID& _TrainID, const station& _StartStation,
          const station& _EndStation, const class time& _ArrivalTime, const class time& _DepartureTime,
          const int& _Price, const int& _TicketNum, const int& _Delta, const int& _StartNum,
          const int& _EndNum) :
          UserName(_UserName), TrainID(_TrainID), StartStation(_StartStation), EndStation(_EndStation),
          ArrivalTime(_ArrivalTime), DepartureTime(_DepartureTime), Price(_Price), TicketNum(_TicketNum),
          Delta(_Delta), StartNum(_StartNum), EndNum(_EndNum), Status(SUCCESS) {}
};

extern Order now_order, nex_order;

struct Pend_Order {
    int TicketNum;
    int StartNum, EndNum;
    int OrderPos;

    Pend_Order() = default;

    Pend_Order(const int& _TicketNum, const int& _StartNum,
               const int& _EndNum, const int& _OrderPos) :
               TicketNum(_TicketNum), StartNum(_StartNum),
               EndNum(_EndNum), OrderPos(_OrderPos) {}

    bool operator == (const Pend_Order &other) const {
        return OrderPos == other.OrderPos;
    }

    bool operator < (const Pend_Order &other) const {
        return OrderPos < other.OrderPos;
    }

    bool operator > (const Pend_Order &other) const {
        return OrderPos > other.OrderPos;
    }

    bool operator <= (const Pend_Order &other) const {
        return OrderPos <= other.OrderPos;
    }

    bool operator >= (const Pend_Order &other) const {
        return OrderPos >= other.OrderPos;
    }

    bool operator != (const Pend_Order &other) const {
        return OrderPos != other.OrderPos;
    }
};

static std::string op;
static username _Cur_UserName, _UserName;
static password _PassWord;
static name _Name;
static mailAddr _MailAddr;
static int _PriviLedge, _TicketNum, _OrderNum;
static trainID _TrainID;
static int _StationNum, _SeatNum;
static station _Stations[105], _StartStation, _EndStation;
static int _Prices[105], _TravelTimes[105], _StopoverTimes[105];
static class time _StartTime;
static date _StartDate, _EndDate, _Date;
static char _Type;
static bool _Flag;

class TicketSystem {
public:
    UserSystem user;
    TrainSystem train;
    BPlusTree<username, int> order_num;
    BPlusTree<username, int> order_pos;
    FileSystem<Order> order_data;
    BPlusTree<std::pair<trainID, int>, Pend_Order> pend_data;

    TicketSystem();

    void BuyTicket(const username &, const trainID &, const date &,
                   const int &, const station &, const station &, const bool &);

    void QueryOrder(const username &);

    void RefundTicket(const username &, const int &);

    void Clean();

    void Run();

    ~TicketSystem() = default;
};

#endif //TICKETSYSTEM_TICKETSYSTEM_H
