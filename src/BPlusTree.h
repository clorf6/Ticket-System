//
// Created by 屋顶上的小丑 on 2023/4/17.
//

#ifndef TICKETSYSTEM_BPLUSTREE_H
#define TICKETSYSTEM_BPLUSTREE_H

#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include "Exception.h"
#include "_string.h"

const int kMaxBlockSize = 50;
const int kMinBlockSize = 15;

template<class U, class T>
class Element {
public:
    T value;
    U index;
    Element() {}

    Element(const T &Value, const std::string &Index = "") : value(Value), index(Index) {};

    bool operator==(const Element &x) const {
        return index == x.index && value == x.value;
    }

    bool operator<(const Element &x) const {
        return index == x.index ? value < x.value : index < x.index;
    }

    bool operator>(const Element &x) const {
        return index == x.index ? value > x.value : index > x.index;
    }

    bool operator<=(const Element &x) const {
        return index == x.index ? value <= x.value : index <= x.index;
    }

    bool operator>=(const Element &x) const {
        return index == x.index ? value >= x.value : index >= x.index;
    }

    bool operator!=(const Element &x) const {
        return value != x.value || index != x.index;
    }

    ~Element() = default;
};

template<class U, class T>
class Node {
public:
    bool is_leaf;
    int count;
    int fa, pos;
    Element<U, T> key[kMaxBlockSize + 5];
    Node<U, T>() {
        is_leaf = true;
        count = 0;
        fa = pos = -1;
    }
    int UpperBound(const Element<U, T> &now) const {
        int l = 0, r = count - 1;
        while (l <= r) {
            int mid = (l + r) >> 1;
            if (now >= key[mid]) l = mid + 1;
            else r = mid - 1;
        }
        return l;
    }
    int LowerBound(const Element<U, T> &now) const {
        int l = 0, r = count - 1;
        while (l <= r) {
            int mid = (l + r) >> 1;
            if (now > key[mid]) l = mid + 1;
            else r = mid - 1;
        }
        return l;
    }
    int UpperBound(const string& now) const {
        int l = 0, r = count - 1;
        while (l <= r) {
            int mid = (l + r) >> 1;
            if (now >= key[mid].index) l = mid + 1;
            else r = mid - 1;
        }
        return l;
    }
    int LowerBound(const string& now) const {
        int l = 0, r = count - 1;
        while (l <= r) {
            int mid = (l + r) >> 1;
            if (now > key[mid].index) l = mid + 1;
            else r = mid - 1;
        }
        return l;
    }
    bool Find(const Element<U, T> &now) const {
        return key[LowerBound(now)] == now;
    }
};

template<class U, class T>
class InterNode : public Node<U, T> {
public:
    int child[kMaxBlockSize + 6]{};
    bool child_leaf[kMaxBlockSize + 6]{};
    InterNode<U, T>(const int& Pos = -1) {
        this->is_leaf = false;
        this->count = 0;
        this->fa = -1;
        this->pos = Pos;
        for (int i = 0; i < kMaxBlockSize + 1; i++) {
            child[i] = -1;
            child_leaf[i] = true;
        }
    }
};

template<class U, class T>
class LeafNode : public Node<U, T> {
public:
    int pre;
    int suc;
    LeafNode<U, T>(const int& Pos = -1) {
        this->is_leaf = true;
        this->count = 0;
        this->fa = this->pre = this->suc = -1;
        this->pos = Pos;
    }
};

template<class U, class T>
class BPlusTree {
public:
    LeafNode<U, T> Leaf_root;
    InterNode<U, T> Inter_root;
    bool root_leaf;
    int root_pos;
    int Inter_count;
    int Leaf_count;
    std::fstream Inter_data;
    std::fstream Leaf_data;
    const int kInterNodeSize = sizeof(InterNode<U, T>);
    const int kLeafNodeSize = sizeof(LeafNode<U, T>);
    const int kSizeofElement = sizeof(Element<U, T>);
    const int HeaderSize = sizeof(int) + sizeof(bool);
    InterNode<U, T> Inter_now, Inter_nex, Inter_child, Inter_fa;
    LeafNode<U, T> Leaf_now, Leaf_nex, Leaf_child, Leaf_nex_nex;

    BPlusTree(const std::string &file_name) {
        Inter_data.open(file_name + ".inter", std::ios::in | std::ios::out | std::ios::binary);
        Leaf_data.open(file_name + ".leaf", std::ios::in | std::ios::out | std::ios::binary);
        if (Inter_data.is_open() && Leaf_data.is_open()) {
            Inter_data.seekg(0);
            Inter_data.read(reinterpret_cast<char *>(&root_pos), sizeof(int));
            Inter_data.read(reinterpret_cast<char *>(&root_leaf), sizeof(bool));
            Inter_data.seekp(0, std::ios::end);
            Inter_count = ((int)Inter_data.tellp() - 4) / kInterNodeSize;
            Leaf_data.seekp(0, std::ios::end);
            Leaf_count = Leaf_data.tellp() / kLeafNodeSize;
            if (root_leaf) {
                ReadLeafNode(root_pos, Leaf_root);
            } else {
                ReadInterNode(root_pos, Inter_root);
            }
        }
        else {
            std::ofstream create;
            create.open(file_name + ".inter", std::ios::out);
            create.close();
            Inter_data.open(file_name + ".inter", std::ios::in | std::ios::out | std::ios::binary);
            create.open(file_name + ".leaf", std::ios::out);
            create.close();
            Leaf_data.open(file_name + ".leaf", std::ios::in | std::ios::out | std::ios::binary);
            root_leaf = true;
            root_pos = Leaf_root.pos = ++Leaf_count;
        }
    };

    ~BPlusTree() {
        Inter_data.seekp(0);
        Inter_data.write(reinterpret_cast<char *>(&root_pos), sizeof(int));
        Inter_data.write(reinterpret_cast<char *>(&root_leaf), sizeof(bool));
        Inter_data.close();
        Leaf_data.close();
    }

    void ReadInterNode(int pos, InterNode<U, T> &ret) {
        Inter_data.seekg(HeaderSize + (pos - 1) * kInterNodeSize);
        Inter_data.read(reinterpret_cast<char *>(&ret), kInterNodeSize);
    }

    void ReadLeafNode(int pos, LeafNode<U, T> &ret) {
        Leaf_data.seekg((pos - 1) * kLeafNodeSize);
        Leaf_data.read(reinterpret_cast<char *>(&ret), kLeafNodeSize);
    }

    void WriteInterNode(int pos, InterNode<U, T> &ret) {
        Inter_data.seekp(HeaderSize + (pos - 1) * kInterNodeSize);
        Inter_data.write(reinterpret_cast<char *>(&ret), kInterNodeSize);
    }

    void WriteLeafNode(int pos, LeafNode<U, T> &ret) {
        Leaf_data.seekp((pos - 1) * kLeafNodeSize);
        Leaf_data.write(reinterpret_cast<char *>(&ret), kLeafNodeSize);
    }

    int Find(const Element<U, T> &now) {
        if (!root_leaf) {
            Inter_now = Inter_root;
            while (1) {
                int pos = Inter_now.UpperBound(now);
                if ((!Inter_now.child_leaf[pos]) && (~Inter_now.child[pos])) ReadInterNode(Inter_now.child[pos], Inter_now);
                else if (~Inter_now.child[pos]) {
                    return Inter_now.child[pos];
                }
            }
        } else {
            return Leaf_root.UpperBound(now);
        }
    }

    void Find(const string& now) {
        if (!root_leaf) {
            Inter_now = Inter_root;
            if (!Inter_now.count) {
                std::cout << "null";
                return ;
            }
            int pos;
            while (1) {
                pos = Inter_now.LowerBound(now);
//                printf("---1 debug---\n");
//                printf("pos %d\n",pos);
//                printInter(Inter_now);
//                printf("---2 debug ---\n");
                if ((!Inter_now.child_leaf[pos]) && (~Inter_now.child[pos])) {
                    ReadInterNode(Inter_now.child[pos], Inter_now);
                } else if (~Inter_now.child[pos]) break;
            }
            ReadLeafNode(Inter_now.child[pos], Leaf_now);
            pos = Leaf_now.LowerBound(now);
//          printf("---1 debug---\n");
//          printf("pos %d\n",pos);
//          printLeaf(Leaf_now);
//          printf("---2 debug ---\n");
            bool flag = false;
            for (int i = pos; i < Leaf_now.count; i++) {
                if (Leaf_now.key[i].index > now) {
                    if (!flag) {
                        std::cout << "null";
                    }
                    return ;
                } else if (Leaf_now.key[i].index == now) {
                    flag = true;
                    std::cout << Leaf_now.key[i].value << ' ';
                }
            }
            while (~Leaf_now.suc) {
                ReadLeafNode(Leaf_now.suc, Leaf_now);
                for (int i = 0; i < Leaf_now.count; i++) {
                    if (Leaf_now.key[i].index > now) {
                        if (!flag) {
                            std::cout << "null";
                        }
                        return ;
                    } else if (Leaf_now.key[i].index == now) {
                        flag = true;
                        std::cout << Leaf_now.key[i].value << ' ';
                    }
                }
            }
            if (!flag) {
                std::cout << "null";
            }
            return ;
        } else {
            if (!Leaf_root.count) {
                std::cout << "null";
                return ;
            }
            int pos = Leaf_root.LowerBound(now);
            if (pos >= Leaf_root.count || Leaf_root.key[pos].index != now) {
                std::cout << "null";
                return ;
            }
            for (int i = pos; i < Leaf_root.count; i++) {
                if (Leaf_root.key[i].index != now) {
                    return ;
                }
                std::cout << Leaf_root.key[i].value << ' ';
            }
            return ;
        }
    }

    void SplitFa(int Fa, Element<U, T> &Key, int Pos, bool Leaf) {
        ReadInterNode(Fa, Inter_fa);
        int k = Inter_fa.UpperBound(Key);
        for (int i = Inter_fa.count; i > k; i--) Inter_fa.key[i] = Inter_fa.key[i - 1];
        Inter_fa.key[k] = Key;
        Inter_fa.count++;
        for (int i = Inter_fa.count; i > k + 1; i--) {
            Inter_fa.child[i] = Inter_fa.child[i - 1];
            Inter_fa.child_leaf[i] = Inter_fa.child_leaf[i - 1];
        }
        Inter_fa.child[k + 1] = Pos;
        Inter_fa.child_leaf[k + 1] = Leaf;
        WriteInterNode(Fa, Inter_fa);
        if (Fa == root_pos) {
            Inter_root = Inter_fa;
        }
    }

    void SplitLeafKey(LeafNode<U, T> &now, LeafNode<U, T> &nex) {
        int new_count = kMinBlockSize;
        memcpy(nex.key, now.key + new_count, kSizeofElement * (now.count - new_count));
        nex.count = now.count - new_count;
        now.count = new_count;
    }

    void SplitList(LeafNode<U, T> &now, LeafNode<U, T> &nex) {
        nex.pre = now.pos;
        nex.suc = now.suc;
        if (~now.suc) {
            ReadLeafNode(now.suc, Leaf_nex);
            Leaf_nex.pre = nex.pos;
            WriteLeafNode(now.suc, Leaf_nex);
        }
        now.suc = nex.pos;
    }

    void SplitLeaf(LeafNode<U, T> &now) {
        LeafNode<U, T> nex(++Leaf_count);
        SplitList(now, nex);
        SplitLeafKey(now, nex);
        nex.fa = now.fa;
        WriteLeafNode(now.pos, now);
        WriteLeafNode(nex.pos, nex);
        SplitFa(now.fa, nex.key[0], nex.pos, true);
    }

    void SplitChild(InterNode<U, T> &now, InterNode<U, T> &nex) {
        for (int i = 0; i <= nex.count; i++) {
            nex.child[i] = now.child[now.count + 1 + i];
            nex.child_leaf[i] = now.child_leaf[now.count + 1 + i];
            if (~nex.child[i]) {
                if (nex.child_leaf[i]) {
                    ReadLeafNode(nex.child[i], Leaf_child);
                    Leaf_child.fa = nex.pos;
                    WriteLeafNode(nex.child[i], Leaf_child);
                } else {
                    ReadInterNode(nex.child[i], Inter_child);
                    Inter_child.fa = nex.pos;
                    WriteInterNode(nex.child[i], Inter_child);
                }
            }
        }
    }

    void SplitInterKey(InterNode<U, T> &now, InterNode<U, T> &nex, Element<U, T> &x) {
        int new_count = kMinBlockSize;
        x = now.key[new_count];
        memcpy(nex.key, now.key + new_count + 1, kSizeofElement * (now.count - new_count - 1));
        nex.count = now.count - new_count - 1;
        now.count = new_count;
    }

    void SplitInter(InterNode<U, T> &now) {
        InterNode<U, T> nex(++Inter_count);
        Element<U, T> x;
        SplitInterKey(now, nex, x);
        SplitChild(now, nex);
        nex.fa = now.fa;
        WriteInterNode(now.pos, now);
        WriteInterNode(nex.pos, nex);
        SplitFa(now.fa, x, nex.pos, false);
    }

    void NewRoot(Node<U, T> &now, Node<U, T> &nex, Element<U, T> &x) {
        InterNode<U, T> new_root(++Inter_count);
        new_root.count = 1;
        new_root.key[0] = x;
        new_root.child[0] = now.pos;
        new_root.child[1] = nex.pos;
        new_root.child_leaf[0] = now.is_leaf;
        new_root.child_leaf[1] = nex.is_leaf;
        nex.fa = now.fa = new_root.pos;
        WriteInterNode(new_root.pos, new_root);
        root_pos = new_root.pos;
    }

    void SplitRoot() {
        if (!root_leaf) {
            InterNode<U, T> nex(++Inter_count);
            Element<U, T> x;
            SplitInterKey(Inter_root, nex, x);
            SplitChild(Inter_root, nex);
            NewRoot(Inter_root, nex, x);
            WriteInterNode(Inter_root.pos, Inter_root);
            WriteInterNode(nex.pos, nex);
            ReadInterNode(root_pos, Inter_root);
        } else {
            LeafNode<U, T> nex(++Leaf_count);
            SplitLeafKey(Leaf_root, nex);
            SplitList(Leaf_root, nex);
            NewRoot(Leaf_root, nex, nex.key[0]);
            WriteLeafNode(Leaf_root.pos, Leaf_root);
            WriteLeafNode(nex.pos, nex);
            ReadInterNode(root_pos, Inter_root);
            root_leaf = false;
        }
    }

    void InsertKey(LeafNode<U, T> &now, const Element<U, T> &x) {
        int pos = now.UpperBound(x);
        //if (pos && now.key[pos - 1] == x) throw Exception("Element already exists.");
        for (int i = now.count; i > pos; i--) now.key[i] = now.key[i - 1];
        now.key[pos] = x;
        now.count++;
        WriteLeafNode(now.pos, now);
    }

    void Insert(const Element<U, T> &x) {
        if (root_leaf) {
            InsertKey(Leaf_root, x);
            if (Leaf_root.count >= kMaxBlockSize) {
                SplitRoot();
            }
            return ;
        }
        int Leaf_pos = Find(x);
        ReadLeafNode(Leaf_pos, Leaf_now);
        InsertKey(Leaf_now, x);
        if (Leaf_now.count >= kMaxBlockSize) {
            SplitLeaf(Leaf_now);
            ReadInterNode(Leaf_now.fa, Inter_now);
            while (Inter_now.count >= kMaxBlockSize) {
                if (Inter_now.pos == root_pos) {
                    SplitRoot();
                    break;
                }
                SplitInter(Inter_now);
                ReadInterNode(Inter_now.fa, Inter_now);
            }
        }
        return ;
    }

    void MergeFa(InterNode<U, T> &Fa, int k) {
        for (int i = k; i < Fa.count; i++) Fa.key[i] = Fa.key[i + 1];
        for (int i = k + 1; i <= Fa.count; i++) {
            Fa.child[i] = Fa.child[i + 1];
            Fa.child_leaf[i] = Fa.child_leaf[i + 1];
        }
        Fa.count--;
        WriteInterNode(Fa.pos, Fa);
        if (Fa.pos == root_pos) {
            Inter_root = Fa;
        }
    }

    void MergeLeafKey(LeafNode<U, T> &now, LeafNode<U, T> &nex) {
        memcpy(now.key + now.count, nex.key, kSizeofElement * nex.count);
        now.count += nex.count;
    }

    void MergeList(LeafNode<U, T> &now, LeafNode<U, T> &nex) {
        if (~nex.suc) {
            ReadLeafNode(nex.suc, Leaf_nex_nex);
            Leaf_nex_nex.pre = now.pos;
            WriteLeafNode(nex.suc, Leaf_nex_nex);
        }
        now.suc = nex.suc;
    }

    void MergeLeaf(LeafNode<U, T> &now, LeafNode<U, T> &nex, InterNode<U, T> &fa, int k) {
        MergeList(now, nex);
        MergeLeafKey(now, nex);
        WriteLeafNode(now.pos, now);
        MergeFa(fa, k);
    }

    void MergeChild(InterNode<U, T> &now, InterNode<U, T> &nex, int& count) {
        for (int i = 0; i <= nex.count; i++) {
            now.child[count + i] = nex.child[i];
            now.child_leaf[count + i] = nex.child_leaf[i];
            if (~nex.child[i]) {
                if (nex.child_leaf[i]) {
                    ReadLeafNode(nex.child[i], Leaf_child);
                    Leaf_child.fa = now.pos;
                    WriteLeafNode(nex.child[i], Leaf_child);
                } else {
                    ReadInterNode(nex.child[i], Inter_child);
                    Inter_child.fa = now.pos;
                    WriteInterNode(nex.child[i], Inter_child);
                }
            }
        }
    }

    void MergeInterKey(InterNode<U, T> &now, InterNode<U, T> &nex, Element<U, T> &x) {
        memcpy(now.key + now.count + 1, nex.key, kSizeofElement * nex.count);
        now.key[now.count] = x;
        now.count += nex.count + 1;
    }

    void MergeInter(InterNode<U, T> &now, InterNode<U, T> &nex, InterNode<U, T> &fa, int k, const Element<U, T> &x, const Element<U, T> &x_suc, bool &flag) {
        int Count = now.count + 1;
        MergeInterKey(now, nex, fa.key[k]);
        MergeChild(now, nex, Count);
        if (now.key[Count - 1] == x) {
            flag = true;
            now.key[Count - 1] = x_suc;
        }
        WriteInterNode(now.pos, now);
        MergeFa(fa, k);
    }

    void MergeRoot(const Element<U, T> &x, const Element<U, T> &x_suc, bool &flag) {
        if (!root_leaf) {
            if (!Inter_root.child_leaf[0]) {
                ReadInterNode(Inter_root.child[0], Inter_now);
                ReadInterNode(Inter_root.child[1], Inter_nex);
                int Count = Inter_now.count + 1;
                MergeInterKey(Inter_now, Inter_nex, Inter_root.key[0]);
                MergeChild(Inter_now, Inter_nex, Count);
                if (Inter_now.key[Count - 1] == x) {
                    flag = true;
                    Inter_now.key[Count - 1] = x_suc;
                }
                Inter_root = Inter_now;
                root_pos = Inter_now.pos;
                WriteInterNode(root_pos, Inter_root);
            } else {
                ReadLeafNode(Inter_root.child[0], Leaf_now);
                ReadLeafNode(Inter_root.child[1], Leaf_nex);
                MergeLeafKey(Leaf_now, Leaf_nex);
                MergeList(Leaf_now, Leaf_nex);
                Leaf_root = Leaf_now;
                root_pos = Leaf_now.pos;
                root_leaf = true;
                WriteLeafNode(root_pos, Leaf_root);
            }
        }
    }

    bool EraseKey(LeafNode<U, T> &now, const Element<U, T> &x, Element<U, T> &suc, bool &flag) {
        int pos = now.LowerBound(x);
        if (pos >= now.count || now.key[pos] != x) {
            return false;
            //throw Exception("Element doesn't exist.");
        }
        for (int i = pos; i < now.count; i++) now.key[i] = now.key[i + 1];
        now.count--;
        if (now.count > pos) suc = now.key[pos];
        else {
            if (~now.suc) {
                ReadLeafNode(now.suc, Leaf_now);
                suc = Leaf_now.key[0];
            }
        }
        WriteLeafNode(now.pos, now);
        if (!pos) {
            if (~now.fa) {
                ReadInterNode(now.fa, Inter_fa);
                int k = Inter_fa.LowerBound(x);
                if (Inter_fa.key[k] == x) {
                    if (now.count) {
                        Inter_fa.key[k] = now.key[0];
                        flag = true;
                        WriteInterNode(now.fa, Inter_fa);
                        if (now.fa == root_pos) {
                            Inter_root = Inter_fa;
                        }
                    }
                }
            }
        }
        return true;
    }

    void FindPre(InterNode<U, T> &now, InterNode<U, T>& nex, InterNode<U, T>& fa, int& k, Element<U, T> &x) {
        if (~nex.fa) {
            ReadInterNode(nex.fa, fa);
            k = fa.UpperBound(x);
            if (k > 0 && (~fa.child[k - 1])) {
                ReadInterNode(fa.child[k - 1], now);
                k--;
            } else {
                now = nex;
                ReadInterNode(fa.child[1], nex);
            }
        }
    }

    void ChangeSuc(const Element<U, T> &x, Element<U, T> &x_suc) {
        Inter_now = Inter_root;
        while (1) {
            int pos = Inter_now.LowerBound(x);
            if (pos == Inter_now.count) {
                if ((!Inter_now.child_leaf[pos]) && (~Inter_now.child[pos])) {
                    ReadInterNode(Inter_now.child[pos], Inter_now);
                    continue;
                }
                else return ;
            }
            if (Inter_now.key[pos] == x) {
                Inter_now.key[pos] = x_suc;
                WriteInterNode(Inter_now.pos, Inter_now);
                if (Inter_now.pos == root_pos) Inter_root = Inter_now;
                return ;
            } else if (Inter_now.key[pos] > x) {
                if ((!Inter_now.child_leaf[pos]) && (~Inter_now.child[pos])) ReadInterNode(Inter_now.child[pos], Inter_now);
                else return ;
            }
        }
    }

    void Erase(const Element<U, T> &x) {
        Element<U, T> x_suc;
        if (root_leaf) {
            EraseKey(Leaf_root, x, x_suc);
            return ;
        }
        int Leaf_pos = Find(x);
        ReadLeafNode(Leaf_pos, Leaf_nex);
        ReadInterNode(Leaf_nex.fa, Inter_fa);
        int k = Inter_fa.UpperBound(Leaf_nex.key[0]);
        bool flag = false;
        bool ret = EraseKey(Leaf_nex, x, x_suc, flag);
        if (!ret) return ;
        if (k > 0 && (~Inter_fa.child[k - 1])) {
            ReadLeafNode(Inter_fa.child[k - 1], Leaf_now);
            k--;
        } else {
            Leaf_now = Leaf_nex;
            ReadLeafNode(Inter_fa.child[1], Leaf_nex);
        }
//        printf("---1 debug---\n");
//        printLeaf(Leaf_now);
//        printf("---2 debug---\n");
//        printLeaf(Leaf_nex);
//        printf("---3 debug---\n");
        if ((!Leaf_now.count) || (!Leaf_nex.count) || Leaf_now.count + Leaf_nex.count <= kMinBlockSize) {
            if (Inter_fa.pos == root_pos && Inter_root.count == 1) {
                MergeRoot(x, x_suc, flag);
                return ;
            }
            Element<U, T> now_x = Inter_fa.key[0];
            MergeLeaf(Leaf_now, Leaf_nex, Inter_fa, k);
            if (Inter_fa.pos == root_pos) return ;
            Inter_nex = Inter_fa;
            FindPre(Inter_now, Inter_nex, Inter_fa, k, now_x);
            while ((!Inter_now.count) || (!Inter_nex.count) || Inter_now.count + Inter_nex.count <= kMinBlockSize) {
                if (Inter_fa.pos == root_pos && Inter_root.count == 1) {
                    MergeRoot(x, x_suc, flag);
                    break ;
                }
                now_x = Inter_fa.key[0];
//                printf("---1 debug---\n");
//                print(Inter_now);
//                printf("---2 debug---\n");
//                print(Inter_nex);
//                printf("---3 debug---\n");
                MergeInter(Inter_now, Inter_nex, Inter_fa, k, x, x_suc, flag);
                if (Inter_fa.pos == root_pos) break ;
                Inter_nex = Inter_fa;
                FindPre(Inter_now, Inter_nex, Inter_fa, k, now_x);
            }
        }
        if(flag) return ;
        ChangeSuc(x, x_suc);
        return ;
    }

    void printLeaf(int pos) {
        LeafNode<U, T> now;
        ReadLeafNode(pos, now);
        printLeaf(now);
    }

    void printLeaf(LeafNode<U, T> &now) {
        std::cout << "Leaf " << now.is_leaf << ' ' << now.count << ' ' << now.fa << ' ' << now.pos << '\n';
        for (int i = 0; i < now.count; i++) {
            std::cout << "i " << i << " key " << now.key[i].index << ' ' << now.key[i].value << '\n';
        }
        return ;
    }

    void print(InterNode<U, T> &now) {
        std::cout << "Inter " << now.is_leaf << ' ' << now.count << ' ' << now.fa << ' ' << now.pos << '\n';
        for (int i = 0; i < now.count; i++) {
            std::cout << "i " << i << " key " << now.key[i].index << ' ' << now.key[i].value << '\n';
        }
        for (int i = 0; i <= now.count; i++) {
            std::cout << "i " << i << " " << now.child[i] << " nowpos " << now.pos << '\n';
            if (now.child_leaf[i] && (~now.child[i])) {
                LeafNode<U, T> Leaf;
                ReadLeafNode(now.child[i], Leaf);
                printLeaf(Leaf);
            }
            else if (~now.child[i]) {
                InterNode<U, T> Inter;
                ReadInterNode(now.child[i], Inter);
                print(Inter);
            }
        }
        return ;
    }

    void print(int pos) {
        InterNode<U, T> now;
        ReadInterNode(pos, now);
        print(now);
    }

    void printInter(int pos) {
        InterNode<U, T> now;
        ReadInterNode(pos, now);
        printInter(now);
    }

    void printInter(InterNode<U, T> &now) {
        std::cout << "Inter " << now.is_leaf << ' ' << now.count << ' ' << now.fa << ' ' << now.pos << '\n';
        for (int i = 0; i < now.count; i++) {
            std::cout << "i " << i << " key " << now.key[i].index << ' ' << now.key[i].value << '\n';
        }
        for (int i = 0; i <= now.count; i++) {
            printf("i %d %d %d\n",i, now.child[i], now.child_leaf[i]);
        }
    }

    void debug() {
        std::cout << "-------\n";
        std::cout << "Inter_count " << Inter_count << " Leaf_count " << Leaf_count << '\n';
        if (root_leaf) printLeaf(Leaf_root);
        else print(Inter_root);
        std::cout << "-------\n";
    }
};

#endif //TICKETSYSTEM_BPLUSTREE_H
