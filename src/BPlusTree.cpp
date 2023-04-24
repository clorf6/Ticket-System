#ifndef TICKET_SYSTEM_BPLUSTREE_CPP
#define TICKET_SYSTEM_BPLUSTREE_CPP

#include "BPlusTree.h"

BPlusTree<string, int> list("BPT");
int main() {
//    try {
    int T;
    std::cin >> T;
    while (T--) {
        std::string opt;
        std::cin >> opt;
        if (opt == "insert") {
            std::string s;
            int data;
            std::cin >> s >> data;
            list.Insert(Element<string, int>{data, s});
        } else if (opt == "find") {
            std::string s;
            std::cin >> s;
            list.Find(s);
            std::cout << '\n';
        } else if (opt == "delete") {
            std::string s;
            int data;
            std::cin >> s >> data;
            list.Erase(Element<string, int>{data, s});
        } else if (opt == "debug") list.debug();
        //list.debug();
    }
//    }
//    catch (Exception &error) {
//        std::cout << error.what() << '\n';
//    }
    return 0;
}

#endif //TICKET_SYSTEM_BPLUSTREE_CPP