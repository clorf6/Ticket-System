//
// Created by 屋顶上的小丑 on 2023/4/14.
//

#ifndef TICKETSYSTEM_EXCEPTION_H
#define TICKETSYSTEM_EXCEPTION_H

#include <iostream>
#include <string>

class Exception {
private:
    const std::string message;
public:
    Exception(const std::string &str) : message(str) {}

    const std::string what() { return message; }
};

#endif //TICKETSYSTEM_EXCEPTION_H
