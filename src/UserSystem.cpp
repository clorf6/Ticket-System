//
// Created by 屋顶上的小丑 on 2023/5/6.
//

#ifndef TICKETSYSTEM_USERSYSTEM_CPP
#define TICKETSYSTEM_USERSYSTEM_CPP

#include "UserSystem.h"

User now_user;

UserSystem::UserSystem(): user_pos("user_pos"), user_data("user_data") {}

void UserSystem::AddUser(const username &Cur_username, const username &Username, const password &Password,
                    const name &Name, const mailAddr &MailAddr, const int &Privilege) {
    if (user_data.empty()) {
        User new_user(Username, Password, Name, MailAddr, 10);
        user_data.Write(1, new_user);
        user_pos.Insert(Element<username, int>{Username, 1});
        std::cout << "0\n";
        return ;
    }
    if (!is_login.find(Cur_username)) throw Exception("User is not logged in or does not exist");
    if (is_login[Cur_username] <= Privilege) throw Exception("No permission to create this user");
    user_pos.Find(Username);
    if (!user_pos.ans.empty()) throw Exception("The username already exists");
    User new_user(Username, Password, Name, MailAddr, Privilege);
    int user_count = user_data.size();
    user_data.Write(++user_count, new_user);
    user_pos.Insert(Element<username, int>{Username, user_count});
    std::cout << "0\n";
}


void UserSystem::LoginUser(const username &Username, const password &Password) {
    if (is_login.find(Username)) throw Exception("User has logged in");
    user_pos.Find(Username);
    if(user_pos.ans.empty()) throw Exception("User does not exist");
    user_data.Read(user_pos.ans[0], now_user);
    if (now_user.PassWord != Password) throw Exception("Password error");
    is_login[Username] = now_user.Privilege;
    std::cout << "0\n";
}

void UserSystem::LogoutUser(const username &Username) {
    if (!is_login.find(Username)) throw Exception("User is not logged in or does not exist");
    is_login.erase(Username);
    std::cout << "0\n";
}

void UserSystem::QueryProfileUser(const username &Cur_username, const username &Username) {
    if (!is_login.find(Cur_username)) throw Exception("User is not logged in or does not exist");
    user_pos.Find(Username);
    if (user_pos.ans.empty()) throw Exception("User does not exist");
    user_data.Read(user_pos.ans[0], now_user);
    if (Cur_username != Username && is_login[Cur_username] <= now_user.Privilege) {
        throw Exception("No permission to query this user's profile");
    }
    std::cout << now_user.UserName << ' ' << now_user.Name << ' ' << now_user.MailAddr << ' ' << now_user.Privilege << '\n';
}

void UserSystem::ModifyProfileUser(const username &Cur_username, const username &Username, const password &Password,
                                   const name &Name, const mailAddr &MailAddr, const int &Privilege) {
    if (!is_login.find(Cur_username)) throw Exception("User is not logged in or does not exist");
    user_pos.Find(Username);
    if (user_pos.ans.empty()) throw Exception("User does not exist");
    user_data.Read(user_pos.ans[0], now_user);
    if (Cur_username != Username && is_login[Cur_username] <= now_user.Privilege) {
        throw Exception("No permission to modify this user's profile");
    }
    if (is_login[Cur_username] <= Privilege) throw Exception("No permission to modify this user's permission");
    if (!Password.empty()) now_user.PassWord = Password;
    if (!Name.empty()) now_user.Name = Name;
    if (!MailAddr.empty()) now_user.MailAddr = MailAddr;
    if (Privilege) {
        now_user.Privilege = Privilege;
        if (is_login.find(Username)) is_login[Username] = Privilege;
    }
    user_data.Write(user_pos.ans[0], now_user);
    std::cout << now_user.UserName << ' ' << now_user.Name << ' ' << now_user.MailAddr << ' ' << now_user.Privilege << '\n';
}

#endif //TICKETSYSTEM_USERSYSTEM_CPP
