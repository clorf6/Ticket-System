//
// Created by 屋顶上的小丑 on 2023/5/4.
//

#ifndef TICKETSYSTEM_USERSYSTEM_H
#define TICKETSYSTEM_USERSYSTEM_H

#include <string>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <fstream>
#include "BPlusTree.h"
#include "Exception.h"
#include "LinkedHashMap.h"
#include "_string.h"

using username = string<21>;
using password = string<31>;
using name = string<21>;
using mailAddr = string<31>;
using trainID = string<21>;

struct User {
    username UserName;
    password PassWord;
    name Name;
    mailAddr MailAddr;
    int Privilege;

    User(const std::string &_UserName = "",
         const std::string &_PassWord = "",
         const std::string &_Name = "",
         const std::string &_MailAddr = "",
         const int &_Privilege = 0) :
         UserName(_UserName), PassWord(_PassWord),
         Name(_Name), MailAddr(_MailAddr), Privilege(_Privilege) {};
};

extern User now_user;
const size_t kSizeofUser = sizeof(User);

class UserSystem {
private:

    BPlusTree<username, int> user_pos;
    LinkedHashMap<username, int, HashString> is_login;
public:
    UserSystem();
};

#endif //TICKETSYSTEM_USERSYSTEM_H
