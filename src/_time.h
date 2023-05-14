//
// Created by 屋顶上的小丑 on 2023/5/11.
//

#ifndef TICKETSYSTEM__TIME_H
#define TICKETSYSTEM__TIME_H

#include <iostream>
#include <cstdio>
#include <string>

const int day_num[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const int day_sum[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
std::string str;

class date {
public:
    int month, day;

    date() {};

    date(const int& Month = 1, const int& Day = 1) : month(Month), day(Day) {};

    date(const std::string& Date) {
        month = std::stoi(Date.substr(0, 2));
        day = std::stoi(Date.substr(3, 2));
    }

    date(const date& other) : month(other.month), day(other.day) {};

    bool operator < (const date& other) const {
        return month == other.month ? day < other.day : month < other.month;
    }

    bool operator <= (const date& other) const {
        return month == other.month ? day <= other.day : month < other.month;
    }

    bool operator > (const date& other) const {
        return month == other.month ? day > other.day : month > other.month;
    }

    bool operator >= (const date& other) const {
        return month == other.month ? day >= other.day : month > other.month;
    }

    bool operator == (const date& other) const {
        return month == other.month && day == other.day;
    }

    bool operator != (const date& other) const {
        return day != other.day || month != other.month;
    }

    date& operator = (const date& other) {
        day = other.day;
        month = other.month;
        return (*this);
    }

    date operator + (const int& x) const {
        date ret((*this));
        ret.day += x;
        while (ret.day > day_num[ret.month]) {
            ret.day -= day_num[ret.month];
            ++ret.month;
        }
        return ret;
    }

    date operator - (const int& x) const {
        date ret((*this));
        ret.day -= x;
        while (ret.day <= 0) {
            --ret.month;
            ret.day += day_num[ret.month];
        }
        return ret;
    }

    date& operator += (const int& x) {
        day += x;
        while (day > day_num[month]) {
            day -= day_num[month];
            ++month;
        }
        return (*this);
    }

    date& operator -= (const int& x) {
        day -= x;
        while (day <= 0) {
            --month;
            day += day_num[month];
        }
        return (*this);
    }

    date& operator ++() {
        ++day;
        if (day > day_num[month]) {
            day -= day_num[month];
            ++month;
        }
        return (*this);
    }

    date& operator --() {
        --day;
        if (day <= 0) {
            --month;
            day += day_num[month];
        }
        return (*this);
    }

    int operator -(const date& other) const {
        int sum1 = day_sum[month - 1] + day;
        int sum2 = day_sum[other.month - 1] + other.day;
        return sum1 - sum2;
    }

    operator std::string() const {
        str.clear();
        if (month < 10) str += '0';
        str += std::to_string(month);
        str += '-';
        if (day < 10) str += '0';
        str += std::to_string(day);
        return str;
    }

    friend std::ostream& operator << (std::ostream &, const date &);

};

std::ostream& operator << (std::ostream &out, const date &x) {
    if (x.month < 10) out << '0';
    out << x.month << '-';
    if (x.day < 10) out << '0';
    out << x.day;
    return out;
}

class time {
public:
    int hour, minute;
    date now;

    time() = default;

    time(const int& Hour = 0, const int& Minute = 0,
         const int& Month = 1, const int& Day = 1) :
         hour(Hour), minute(Minute), now(Month, Day) {};

    time(const time& other) : hour(other.hour), minute(other.minute),
    now(other.now) {};

    time& operator = (const time& other) {
        hour = other.hour;
        minute = other.minute;
        now = other.now;
        return (*this);
    }

    bool operator < (const time& other) const {
        if (now != other.now) return now < other.now;
        if (hour != other.hour) return hour < other.hour;
        return minute < other.minute;
    }

    bool operator <= (const time& other) const {
        if (now != other.now) return now < other.now;
        if (hour != other.hour) return hour < other.hour;
        return minute <= other.minute;
    }

    bool operator > (const time& other) const {
        if (now != other.now) return now > other.now;
        if (hour != other.hour) return hour > other.hour;
        return minute > other.minute;
    }

    bool operator >= (const time& other) const {
        if (now != other.now) return now > other.now;
        if (hour != other.hour) return hour > other.hour;
        return minute >= other.minute;
    }

    bool operator == (const time& other) const {
        return now == other.now && hour == other.hour && minute == other.minute;
    }

    bool operator != (const time& other) const {
        return minute != other.minute || hour != other.hour || now != other.now;
    }

    time operator + (const int& x) const {
        time ret((*this));
        ret.minute += x;
        if (ret.minute >= 60) {
            ret.hour += ret.minute / 60;
            ret.minute %= 60;
            if (ret.hour >= 24) {
                ret.now += ret.hour / 24;
                ret.hour %= 24;
            }
        }
        return ret;
    }

    time operator - (const int& x) const {
        time ret((*this));
        ret.minute -= x;
        if (ret.minute < 0) {
            int num = (-1 - ret.minute) / 60 + 1;
            ret.hour -= num;
            ret.minute += num * 60;
            if (ret.hour < 0) {
                num = (-1 - ret.hour) / 24 + 1;
                ret.now -= num;
                ret.hour += num * 24;
            }
        }
        return ret;
    }

    time& operator += (const int& x) {
        minute += x;
        if (minute >= 60) {
            hour += minute / 60;
            minute %= 60;
            if (hour >= 24) {
                now += hour / 24;
                hour %= 24;
            }
        }
        return (*this);
    }

    time& operator -= (const int& x) {
        minute -= x;
        if (minute < 0) {
            int num = (-1 - minute) / 60 + 1;
            hour -= num;
            minute += num * 60;
            if (hour < 0) {
                num = (-1 - hour) / 24 + 1;
                now -= num;
                hour += num * 24;
            }
        }
        return (*this);
    }

    time& operator ++() {
        ++minute;
        if (minute == 60) {
            ++hour;
            minute = 0;
            if (hour == 24) {
                ++now;
                hour = 0;
            }
        }
        return (*this);
    }

    time& operator --() {
        --minute;
        if (minute < 0) {
            --hour;
            minute += 60;
            if (hour < 0) {
                --now;
                hour += 24;
            }
        }
        return (*this);
    }

    int operator - (const time& other) const {
        int ret = (now - other.now) * 24 * 60;
        ret += (hour - other.hour) * 60 + (minute - other.minute);
        return ret;
    }

    friend std::ostream& operator << (std::ostream &, const time &);

};

std::ostream& operator << (std::ostream &out, const time &x) {
    out << x.now << ' ';
    if (x.hour < 10) out << '0';
    out << x.hour << ':';
    if (x.minute < 10) out << '0';
    out << x.minute;
    return out;
}

#endif //TICKETSYSTEM__TIME_H
