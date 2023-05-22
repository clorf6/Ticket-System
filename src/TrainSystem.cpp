//
// Created by 屋顶上的小丑 on 2023/5/16.
//

#ifndef TICKETSYSTEM_TRAINSYSTEM_CPP
#define TICKETSYSTEM_TRAINSYSTEM_CPP

#include "TrainSystem.h"

Train now_train, nex_train;
Station now_station;
Ticket now_ticket, nex_ticket;
class time now_time, nex_time, transfer_now_time, transfer_nex_time;
sjtu::vector<Station> StartStations;
sjtu::vector<Direct_Ticket> ret;
Transfer_Ticket best, now_transfer;
CompTime compTime;
CompCost compCost;

TrainSystem::TrainSystem() : train_pos("train_pos"), train_data("train_data"),
                             ticket_pos("ticket_pos"), ticket_data("ticket_data"),
                             station_data("station_data") {}

void TrainSystem::AddTrain(const trainID& _TrainID, const int& _StationNum, const int& _SeatNum, station *_Stations,
                           int *_Prices, const class time& _StartTime, int *TravelTimes, int *StopoverTimes,
                           const date& _StartDate, const date& _Enddate, const char& _Type) {
    train_pos.Find(_TrainID);
    if (!train_pos.ans.empty()) throw Exception("The train ID already exists");
    Train new_train(_TrainID, _StationNum, _SeatNum, _Stations,_Prices,
                    _StartTime, TravelTimes, StopoverTimes, _StartDate, _Enddate, _Type);
    int train_count = train_data.size();
    train_data.Write(++train_count, new_train);
    train_pos.Insert(Element<trainID, int>{_TrainID, train_count});
    std::cout << "0\n";
}

void TrainSystem::DeleteTrain(const trainID &_TrainID) {
    train_pos.Find(_TrainID);
    if (train_pos.ans.empty()) throw Exception("Train does not exist");
    train_data.Read(train_pos.ans[0], now_train);
    if (now_train.Release) throw Exception("Train already released");
    train_pos.Erase(Element<trainID, int>{_TrainID, train_pos.ans[0]});
    std::cout << "0\n";
}

void TrainSystem::ReleaseTrain(const trainID &_TrainID) {
    train_pos.Find(_TrainID);
    if (train_pos.ans.empty()) throw Exception("Train does not exist");
    int pos = train_pos.ans[0];
    train_data.Read(pos, now_train);
    if (now_train.Release) throw Exception("Train already released");
    now_train.Release = true;
    train_data.Write(pos, now_train);
    Ticket new_ticket(now_train.StationNum, now_train.SeatNum);
    int ticket_count = ticket_data.size(), Duration = now_train.EndDate - now_train.StartDate;
    for (int i = 0; i <= Duration; i++) {
        ticket_data.Write(++ticket_count, new_ticket);
        ticket_pos.Insert(Element<std::pair<trainID, int>, int>
                {std::make_pair(_TrainID, i), ticket_count});
    }
    for (int i = 1; i <= now_train.StationNum; i++) {
        Station new_station(_TrainID, pos, i);
        station_data.Insert(Element<station, Station>{now_train.Stations[i], new_station});
    }
    std::cout << "0\n";
}

void TrainSystem::QueryTrain(const trainID &_TrainID, const date &Date) {
    train_pos.Find(_TrainID);
    if (train_pos.ans.empty()) throw Exception("Train does not exist");
    int pos = train_pos.ans[0];
    train_data.Read(pos, now_train);
    if (Date < now_train.StartDate || Date > now_train.EndDate) {
        throw Exception("Not within the departure period");
    }
    now_time = now_train.StartTime;
    now_time.now = Date;
    std::cout << _TrainID << ' ' << now_train.Type << '\n';
    for (int i = 1; i <= now_train.StationNum; i++) {
        std::cout << now_train.Stations[i] << ' ';
        if (i == 1) std::cout << "xx-xx xx:xx ";
        else std::cout << now_time + now_train.ArrivalTimes[i] << ' ';
        std::cout << "-> ";
        if (i == now_train.StationNum) std::cout << "xx-xx xx:xx ";
        else std::cout << now_time + now_train.DepartureTimes[i] << ' ';
        std::cout << now_train.Prices[i] << ' ';
        if (i == now_train.StationNum) std::cout << "x\n";
        else {
            if (!now_train.Release) std::cout << now_train.SeatNum << '\n';
            else {
                ticket_pos.Find(std::make_pair(_TrainID, Date - now_train.StartDate));
                ticket_data.Read(ticket_pos.ans[0], now_ticket);
                std::cout << now_ticket.TicketNum[i] << '\n';
            }
        }
    }
}

void TrainSystem::QueryTicket(const station &StartStation, const station &EndStation, const date &Date, const bool &op) {
    station_data.Find(StartStation);
    StartStations.swap(station_data.ans);
    station_data.Find(EndStation);
    ret.clear();
    int l = 0, r = 0, tim;
    while (l < StartStations.size()) {
        while (r < station_data.ans.size() && station_data.ans[r].TrainID < StartStations[l].TrainID) ++r;
        if (r >= station_data.ans.size()) break;
        if (StartStations[l].TrainID == station_data.ans[r].TrainID) {
            int &L = StartStations[l].Num, &R = station_data.ans[r].Num;
            if (L >= R) continue;
            train_data.Read(StartStations[l].TrainPos, now_train);
            now_time = now_train.StartTime + now_train.DepartureTimes[L];
            int delta = Date - now_time.now;
            if (delta < 0 || delta > now_train.EndDate - now_train.StartDate) continue;
            now_time.now = Date;
            tim = now_train.ArrivalTimes[R] - now_train.DepartureTimes[L];
            nex_time = now_time + tim;
            ticket_pos.Find(std::make_pair(StartStations[l].TrainID, delta));
            ticket_data.Read(ticket_pos.ans[0], now_ticket);
            ret.push_back(Direct_Ticket(StartStations[l].TrainID, nex_time, now_time,
                          now_train.Prices[R] - now_train.Prices[L], now_ticket.Query(L, R), tim));
        }
        ++l;
    }
    if (!op) sort(ret, 0, ret.size() - 1, compTime);
    else sort(ret, 0, ret.size() - 1, compCost);
    std::cout << ret.size() << '\n';
    for (int i = 0; i < ret.size(); i++) {
        std::cout << ret[i].TrainID << ' ' << StartStation << ' ' << ret[i].DepartureTime << " -> ";
        std::cout << EndStation << ' ' << ret[i].ArrivalTime << ' ' << ret[i].Price << ' ' << ret[i].Seat << '\n';
    }
}

void TrainSystem::QueryTransfer(const station &StartStation, const station &EndStation, const date &Date, const bool &op) {
    bool flag = false;
    station_data.Find(StartStation);
    StartStations.swap(station_data.ans);
    station_data.Find(EndStation);
    for (int i = 0; i < StartStations.size(); i++) {
        int &L = StartStations[i].Num;
        train_data.Read(StartStations[i].TrainPos, now_train);
        now_time = now_train.StartTime + now_train.DepartureTimes[L];
        int delta_start = Date - now_time.now;
        if (delta_start < 0 || delta_start > now_train.EndDate - now_train.StartDate) continue;
        now_time.now = Date;
        transfer.clear();
        for (int k = L + 1; k <= now_train.StationNum; k++) transfer[now_train.Stations[k]] = k;
        for (int j = 0; j < station_data.ans.size(); j++) {
            if (StartStations[i].TrainID == station_data.ans[j].TrainID) continue;
            int &R = station_data.ans[j].Num;
            train_data.Read(station_data.ans[j].TrainPos, nex_train);
            for (int k = 1; k < R; k++) {
                if (!transfer.find(nex_train.Stations[k])) continue;
                int Mid = transfer[nex_train.Stations[k]];
                int fir_time = now_train.ArrivalTimes[Mid] - now_train.DepartureTimes[L];
                transfer_now_time = now_time + fir_time;
                transfer_nex_time = nex_train.StartTime + nex_train.DepartureTimes[k];
                int delta_transfer = transfer_now_time.now - transfer_nex_time.now;
                if (delta_transfer < 0) delta_transfer = 0;
                transfer_nex_time.now = transfer_now_time.now;
                if (transfer_nex_time < transfer_now_time) {
                    ++delta_transfer;
                    ++transfer_nex_time.now;
                }
                if (delta_transfer > nex_train.EndDate - nex_train.StartDate) continue;
                int sec_time = nex_train.ArrivalTimes[R] - nex_train.DepartureTimes[k];
                nex_time = transfer_nex_time + sec_time;
                ticket_pos.Find(std::make_pair(now_train.TrainID, delta_start));
                ticket_data.Read(ticket_pos.ans[0], now_ticket);
                ticket_pos.Find(std::make_pair(nex_train.TrainID, delta_transfer));
                ticket_data.Read(ticket_pos.ans[0], nex_ticket);
                now_transfer = Transfer_Ticket(
                        Direct_Ticket(now_train.TrainID, transfer_now_time, now_time,
                                      now_train.Prices[Mid] - now_train.Prices[L], now_ticket.Query(L, Mid), fir_time),
                        Direct_Ticket(nex_train.TrainID, nex_time, transfer_nex_time,
                                      nex_train.Prices[R] - nex_train.Prices[k], nex_ticket.Query(k, R), sec_time),
                        transfer_nex_time - transfer_now_time, now_train.Stations[k]
                        );
                if (!flag) {
                    best = now_transfer;
                    flag = true;
                } else {
                    if (!op) {
                        if (compTime(now_transfer, best)) best = now_transfer;
                    } else {
                        if (compCost(now_transfer, best)) best = now_transfer;
                    }
                }
            }
        }
    }
    if (flag) std::cout << "0\n";
    else {
        std::cout << best.ticket1.TrainID << ' ' << StartStation << ' ' << best.ticket1.DepartureTime << " -> " << best.Transfer_station
        << ' ' << best.ticket1.ArrivalTime << ' ' << best.ticket1.Price << ' ' << best.ticket1.Seat << '\n';
        std::cout << best.ticket2.TrainID << ' ' << best.Transfer_station << ' ' << best.ticket2.DepartureTime << " -> " << EndStation
                  << ' ' << best.ticket2.ArrivalTime << ' ' << best.ticket2.Price << ' ' << best.ticket2.Seat << '\n';
    }
}

void TrainSystem::Clear() {
    train_pos.clear();
    train_data.clear();
    ticket_pos.clear();
    ticket_data.clear();
    station_data.clear();
    transfer.clear();
}

#endif //TICKETSYSTEM_TRAINSYSTEM_CPP