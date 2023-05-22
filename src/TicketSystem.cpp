//
// Created by 屋顶上的小丑 on 2023/5/11.
//

#ifndef TICKETSYSTEM_TICKETSYSTEM_CPP
#define TICKETSYSTEM_TICKETSYSTEM_CPP

#include "TicketSystem.h"

Order now_order, nex_order;

TicketSystem::TicketSystem(): user(), train(), order_num("order_num"), order_pos("order_pos"),
                              order_data("order_data"), pend_data("pend_data") {}

void TicketSystem::BuyTicket(const username &_UserName, const trainID &_TrainID, const date &Date, const int &_TicketNum,
                             const station &_StartStation, const station &_EndStation, const bool &Type) {
    if (!user.is_login.find(_UserName)) throw Exception("User is not logged in or does not exist");
    train.train_pos.Find(_TrainID);
    if (train.train_pos.ans.empty()) throw Exception("Train does not exist");
    train.train_data.Read(train.train_pos.ans[0], now_train);
    if (!now_train.Release) throw Exception("Train is not released");
    int L = 0, R = 0;
    for (int i = 1; i <= now_train.StationNum; i++) {
        if (now_train.Stations[i] == _StartStation) L = i;
        if (now_train.Stations[i] == _EndStation) R = i;
    }
    if (!L || !R || L >= R) throw Exception("Ticket is not found");
    now_time = now_train.StartTime + now_train.DepartureTimes[L];
    int delta = Date - now_time.now;
    if (delta < 0 || delta > now_train.EndDate - now_train.StartDate) throw Exception("Not within the departure period");
    now_time.now = Date;
    nex_time = now_time + (now_train.ArrivalTimes[R] - now_train.DepartureTimes[L]);
    if (_TicketNum > now_train.SeatNum) throw Exception("Remaining ticket is not enough");
    train.ticket_pos.Find(std::make_pair(_TrainID, delta));
    train.ticket_data.Read(train.ticket_pos.ans[0], now_ticket);
    Order new_order(_UserName, _TrainID, _StartStation, _EndStation,
                    nex_time, now_time, now_train.Prices[R] - now_train.Prices[L],
                    _TicketNum, delta, L, R);
    order_num.Find(_UserName);
    int OrderNum = 0;
    if (!order_num.ans.empty()) {
        OrderNum = order_num.ans[0];
        order_num.Erase(Element<username, int>{_UserName, OrderNum});
    }
    if (now_ticket.Query(L, R) >= _TicketNum) {
        ++OrderNum;
        int pos = order_data.size();
        order_data.Write(++pos, new_order);
        order_pos.Insert(Element<username, int>{_UserName, pos});
        now_ticket.Modify(L, R, -_TicketNum);
        train.ticket_data.Write(train.ticket_pos.ans[0], now_ticket);
        std::cout << 1ll * _TicketNum * new_order.Price << '\n';
    } else {
        if (Type) {
            ++OrderNum;
            new_order.Status = PENDING;
            int pos = order_data.size();
            order_data.Write(++pos, new_order);
            order_pos.Insert(Element<username, int>{_UserName, pos});
            Pend_Order new_pend(_TicketNum, L, R, pos);
            pend_data.Insert(Element<std::pair<trainID, int>, Pend_Order>{std::make_pair(_TrainID, delta), new_pend});
            std::cout << "queue\n";
        } else throw Exception("Remaining ticket is not enough");
    }
    order_num.Insert(Element<username, int>{_UserName, OrderNum});
}

void TicketSystem::QueryOrder(const username &_UserName) {
    if (!user.is_login.find(_UserName)) throw Exception("User is not logged in or does not exist");
    order_pos.Find(_UserName);
    std::cout << order_pos.ans.size() << '\n';
    for (int i = order_pos.ans.size() - 1; i >= 0; i--) {
        order_data.Read(order_pos.ans[i], now_order);
        std::cout << '[';
        if (now_order.Status == SUCCESS) std::cout << "success";
        else if (now_order.Status == PENDING) std::cout << "pending";
        else std::cout << "refunded";
        std::cout << "] ";
        std::cout << now_order.TrainID << ' ' << now_order.StartStation << ' ' << now_order.DepartureTime << " -> " << now_order.EndStation << ' '
        << now_order.ArrivalTime << ' ' << now_order.Price << ' ' << now_order.TicketNum << '\n';
    }
}

void TicketSystem::RefundTicket(const username &_UserName, const int &n) {
    if (!user.is_login.find(_UserName)) throw Exception("User is not logged in or does not exist");
    int OrderNum = 0;
    order_num.Find(_UserName);
    if (!order_num.ans.empty()) OrderNum = order_num.ans[0];
    if (OrderNum < n) throw Exception("Order is not found");
    order_pos.Find(_UserName);
    order_data.Read(order_pos.ans[OrderNum - n], now_order);
    if (now_order.Status == REFUNDED) throw Exception("Order is already refunded");
    if (now_order.Status == SUCCESS) {
        train.ticket_pos.Find(std::make_pair(now_order.TrainID, now_order.Delta));
        train.ticket_data.Read(train.ticket_pos.ans[0], now_ticket);
        now_ticket.Modify(now_order.StartNum, now_order.EndNum, now_order.TicketNum);
        pend_data.Find(std::make_pair(now_order.TrainID, now_order.Delta));
        for (int i = 0; i < pend_data.ans.size(); i++) {
            Pend_Order& now_pend = pend_data.ans[i];
            if (now_pend.TicketNum <= now_ticket.Query(now_pend.StartNum, now_pend.EndNum)) {
                order_data.Read(now_pend.OrderPos, nex_order);
                pend_data.Erase(Element<std::pair<trainID, int>, Pend_Order>
                        {std::make_pair(nex_order.TrainID, nex_order.Delta), now_pend});
                if (nex_order.Status == REFUNDED) continue;
                nex_order.Status = SUCCESS;
                order_data.Write(now_pend.OrderPos, nex_order);
                now_ticket.Modify(now_pend.StartNum, now_pend.EndNum, -now_pend.TicketNum);
            }
        }
        train.ticket_data.Write(train.ticket_pos.ans[0], now_ticket);
    }
    now_order.Status = REFUNDED;
    order_data.Write(order_pos.ans[OrderNum - n], now_order);
    std::cout << "0\n";
}

void TicketSystem::Clean() {
    user.Clear();
    train.Clear();
    order_num.clear();
    order_pos.clear();
    order_data.clear();
    pend_data.clear();
    std::cout << "0\n";
}

void TicketSystem::Run() {
    while (getline(std::cin, op)) {
        try {
            DivideOperation(op);
            std::cout << ops[0] << ' ';
            if (ops[1] == "add_user") {
                _Cur_UserName.clear();
                _UserName.clear();
                _PassWord.clear();
                _Name.clear();
                _MailAddr.clear();
                _PriviLedge = 0;
                for (int i = 2; i < ops.size(); i += 2) {
                    if (ops[i] == "-c") _Cur_UserName = ops[i + 1];
                    else if (ops[i] == "-u") _UserName = ops[i + 1];
                    else if (ops[i] == "-p") _PassWord = ops[i + 1];
                    else if (ops[i] == "-n") _Name = ops[i + 1];
                    else if (ops[i] == "-m") _MailAddr = ops[i + 1];
                    else if (ops[i] == "-g") _PriviLedge = std::stoi(ops[i + 1]);
                    else throw Exception("Invalid operation");
                }
                user.AddUser(_Cur_UserName, _UserName, _PassWord, _Name, _MailAddr, _PriviLedge);
            } else if (ops[1] == "login") {
                _UserName.clear();
                _PassWord.clear();
                for (int i = 2; i < ops.size(); i += 2) {
                    if (ops[i] == "-u") _UserName = ops[i + 1];
                    else if (ops[i] == "-p") _PassWord = ops[i + 1];
                    else throw Exception("Invalid operation");
                }
                user.LoginUser(_UserName, _PassWord);
            } else if (ops[1] == "logout") {
                _UserName.clear();
                for (int i = 2; i < ops.size(); i += 2) {
                    if (ops[i] == "-u") _UserName = ops[i + 1];
                    else throw Exception("Invalid operation");
                }
                user.LogoutUser(_UserName);
            } else if (ops[1] == "query_profile") {
                _Cur_UserName.clear();
                _UserName.clear();
                for (int i = 2; i < ops.size(); i += 2) {
                    if (ops[i] == "-c") _Cur_UserName = ops[i + 1];
                    else if (ops[i] == "-u") _UserName = ops[i + 1];
                    else throw Exception("Invalid operation");
                }
                user.QueryProfileUser(_Cur_UserName, _UserName);
            } else if (ops[1] == "modify_profile") {
                _Cur_UserName.clear();
                _UserName.clear();
                _PassWord.clear();
                _Name.clear();
                _MailAddr.clear();
                _PriviLedge = 0;
                for (int i = 2; i < ops.size(); i += 2) {
                    if (ops[i] == "-c") _Cur_UserName = ops[i + 1];
                    else if (ops[i] == "-u") _UserName = ops[i + 1];
                    else if (ops[i] == "-p") _PassWord = ops[i + 1];
                    else if (ops[i] == "-n") _Name = ops[i + 1];
                    else if (ops[i] == "-m") _MailAddr = ops[i + 1];
                    else if (ops[i] == "-g") _PriviLedge = std::stoi(ops[i + 1]);
                    else throw Exception("Invalid operation");
                }
                user.ModifyProfileUser(_Cur_UserName, _UserName, _PassWord, _Name, _MailAddr, _PriviLedge);
            } else if (ops[1] == "add_train") {
                _TrainID.clear();
                _StationNum = _SeatNum = 0;
                memset(_Stations, 0, sizeof(_Stations));
                memset(_Prices, 0, sizeof(_Prices));
                _StartTime.clear();
                memset(_TravelTimes, 0, sizeof(_TravelTimes));
                memset(_StopoverTimes, 0, sizeof(_StopoverTimes));
                _StartDate.clear();
                _EndDate.clear();
                _Type = 0;
                for (int i = 2; i < ops.size(); i += 2) {
                    if (ops[i] == "-i") _TrainID = ops[i + 1];
                    else if (ops[i] == "-n") _StationNum = std::stoi(ops[i + 1]);
                    else if (ops[i] == "-m") _SeatNum = std::stoi(ops[i + 1]);
                    else if (ops[i] == "-s") DivideStation(_Stations, ops[i + 1]);
                    else if (ops[i] == "-p") DivideNumber(_Prices, ops[i + 1]);
                    else if (ops[i] == "-x") GetTime(_StartTime, ops[i + 1]);
                    else if (ops[i] == "-t") DivideNumber(_TravelTimes, ops[i + 1]);
                    else if (ops[i] == "-o") DivideNumber(_StopoverTimes, ops[i + 1]);
                    else if (ops[i] == "-d") GetSaleDate(_StartDate, _EndDate, ops[i + 1]);
                    else if (ops[i] == "-y") _Type = ops[i + 1][0];
                    else throw Exception("Invalid operation");
                }
                train.AddTrain(_TrainID, _StationNum, _SeatNum, _Stations, _Prices, _StartTime,
                               _TravelTimes, _StopoverTimes, _StartDate, _EndDate, _Type);
            } else if(ops[1] == "delete_train") {
                _TrainID.clear();
                for (int i = 2; i < ops.size(); i += 2) {
                    if (ops[i] == "-i") _TrainID = ops[i + 1];
                    else throw Exception("Invalid operation");
                }
                train.DeleteTrain(_TrainID);
            } else if (ops[1] == "release_train") {
                _TrainID.clear();
                for (int i = 2; i < ops.size(); i += 2) {
                    if (ops[i] == "-i") _TrainID = ops[i + 1];
                    else throw Exception("Invalid operation");
                }
                train.ReleaseTrain(_TrainID);
            } else if (ops[1] == "query_train") {
                _TrainID.clear();
                _Date.clear();
                for (int i = 2; i < ops.size(); i += 2) {
                    if (ops[i] == "-i") _TrainID = ops[i + 1];
                    else if (ops[i] == "-d") GetDate(_Date, ops[i + 1]);
                    else throw Exception("Invalid operation");
                }
                train.QueryTrain(_TrainID, _Date);
            } else if (ops[1] == "query_ticket") {
                _StartStation.clear();
                _EndStation.clear();
                _Date.clear();
                _Flag = false;
                for (int i = 2; i < ops.size(); i += 2) {
                    if (ops[i] == "-s") _StartStation = ops[i + 1];
                    else if (ops[i] == "-t") _EndStation = ops[i + 1];
                    else if (ops[i] == "-d") GetDate(_Date, ops[i + 1]);
                    else if (ops[i] == "-p") {
                        if (ops[i + 1] == "time") _Flag = false;
                        else if (ops[i + 1] == "cost") _Flag = true;
                        else throw Exception("Invalid operation");
                    }
                    else throw Exception("Invalid operation");
                }
                train.QueryTicket(_StartStation, _EndStation, _Date, _Flag);
            } else if (ops[1] == "query_transfer") {
                _StartStation.clear();
                _EndStation.clear();
                _Date.clear();
                _Flag = false;
                for (int i = 2; i < ops.size(); i += 2) {
                    if (ops[i] == "-s") _StartStation = ops[i + 1];
                    else if (ops[i] == "-t") _EndStation = ops[i + 1];
                    else if (ops[i] == "-d") GetDate(_Date, ops[i + 1]);
                    else if (ops[i] == "-p") {
                        if (ops[i + 1] == "time") _Flag = false;
                        else if (ops[i + 1] == "cost") _Flag = true;
                        else throw Exception("Invalid operation");
                    }
                    else throw Exception("Invalid operation");
                }
                train.QueryTransfer(_StartStation, _EndStation, _Date, _Flag);
            } else if (ops[1] == "buy_ticket") {
                _UserName.clear();
                _TrainID.clear();
                _Date.clear();
                _StartStation.clear();
                _EndStation.clear();
                _TicketNum = 0;
                _Flag = false;
                for (int i = 2; i < ops.size(); i += 2) {
                    if (ops[i] == "-u") _UserName = ops[i + 1];
                    else if (ops[i] == "-i") _TrainID = ops[i + 1];
                    else if (ops[i] == "-d") GetDate(_Date, ops[i + 1]);
                    else if (ops[i] == "-n") _TicketNum = std::stoi(ops[i + 1]);
                    else if (ops[i] == "-f") _StartStation = ops[i + 1];
                    else if (ops[i] == "-t") _EndStation = ops[i + 1];
                    else if (ops[i] == "-q") {
                        if (ops[i + 1] == "false") _Flag = false;
                        else if (ops[i + 1] == "true") _Flag = true;
                        else throw Exception("Invalid operation");
                    }
                    else throw Exception("Invalid operation");
                }
                BuyTicket(_UserName, _TrainID, _Date, _TicketNum,
                          _StartStation, _EndStation, _Flag);
            } else if (ops[1] == "query_order") {
                _UserName.clear();
                for (int i = 2; i < ops.size(); i += 2) {
                    if (ops[i] == "-u") _UserName = ops[i + 1];
                    else throw Exception("Invalid operation");
                }
                QueryOrder(_UserName);
            } else if (ops[1] == "refund_ticket") {
                _UserName.clear();
                _OrderNum = 1;
                for (int i = 2; i < ops.size(); i += 2) {
                    if (ops[i] == "-u") _UserName = ops[i + 1];
                    else if (ops[i] == "-n") _OrderNum = std::stoi(ops[i + 1]);
                    else throw Exception("Invalid operation");
                }
                RefundTicket(_UserName, _OrderNum);
            } else if (ops[1] == "clean") {
                Clean();
            } else if (ops[1] == "exit") {
                std::cout << "bye\n";
                break;
            } else throw Exception("Invalid operation");
        } catch (Exception &error) {
            std::cout << error.what() << '\n';
            continue;
        }
    }
}

#endif //TICKETSYSTEM_TICKETSYSTEM_CPP