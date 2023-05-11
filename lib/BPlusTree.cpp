#ifndef TICKET_SYSTEM_BPLUSTREE_CPP
#define TICKET_SYSTEM_BPLUSTREE_CPP

#include "BPlusTree.h"

using Index = string<64>;
BPlusTree<Index, int> bpt("BPT");
int main() {
    try {
        int T;
        std::cin >> T;
        while (T--) {
            std::string opt;
            std::cin >> opt;
            if (opt == "insert") {
                std::string s;
                int data;
                std::cin >> s >> data;
                bpt.Insert(Element<Index, int>{s, data});
            } else if (opt == "find") {
                std::string s;
                std::cin >> s;
                bpt.Find(s);
                if (bpt.ans.empty()) std::cout << "null\n";
                else {
                    for (int i = 0; i < bpt.ans.size(); i++) {
                        std::cout << bpt.ans[i] << ' ';
                    }
                    std::cout << '\n';
                }
            } else if (opt == "delete") {
                std::string s;
                int data;
                std::cin >> s >> data;
                bpt.Erase(Element<Index, int>{s, data});
            } else if(opt == "debug") {
                bpt.debug();
            }
        }
    }
    catch (Exception &error) {
        std::cout << error.what() << '\n';
    }
    return 0;
}

#endif //TICKET_SYSTEM_BPLUSTREE_CPP