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
#include "Pool.h"
#include "Cache.h"
#include "FileSystem.h"

const int kMaxBlockSize = 70;
const int kMinBlockSize = 25;

template<class U, class T>
class Element {
public:
    T value;
    U index;
    Element() {}

    Element(T Value, U Index) : value(Value), index(Index) {};
    Element(const Element<U, T>& other) {
        value = other.value;
        index = other.index;
    }

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
    Node<U, T>(const Node<U, T> &other) :
            is_leaf(other.is_leaf), count(other.count),
            fa(other.fa), pos(other.pos) {
        for (int i = 0; i < count; i++) key[i] = other.key[i];
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
    int UpperBound(const U& now) const {
        int l = 0, r = count - 1;
        while (l <= r) {
            int mid = (l + r) >> 1;
            if (now >= key[mid].index) l = mid + 1;
            else r = mid - 1;
        }
        return l;
    }
    int LowerBound(const U& now) const {
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
    InterNode<U, T>(const InterNode<U, T> &other) :
            Node<U, T>(other) {
        for (int i = 0; i <= this->count; i++) {
            child[i] = other.child[i];
            child_leaf[i] = other.child_leaf[i];
        }
    };
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
    LeafNode<U, T>(const LeafNode<U, T> &other) :
            Node<U, T>(other), pre(other.pre), suc(other.suc) {};
};


template<class U, class T, class Hash = std::hash<int>>
class BPlusTree {
public:
    LeafNode<U, T> Leaf_root;
    InterNode<U, T> Inter_root;
    bool root_leaf;
    int root_pos;
    FileSystem<InterNode<U, T>> Inter_data;
    FileSystem<LeafNode<U, T>> Leaf_data;
    LRUCache<LeafNode<U, T>> Leaf_cache;
    LRUCache<InterNode<U, T>> Inter_cache;
    Pool Leaf_pool, Inter_pool;

    const int kSizeofElement = sizeof(Element<U, T>);
    const int kAppendixSize = sizeof(int) + sizeof(bool);
    InterNode<U, T> Inter_now, Inter_nex, Inter_child, Inter_fa;
    LeafNode<U, T> Leaf_now, Leaf_nex, Leaf_child, Leaf_nex_nex;

    BPlusTree(const std::string &file_name) :
    Inter_data(file_name + ".inter"), Leaf_data(file_name + ".leaf"), Leaf_cache(&Leaf_data), Inter_cache(&Inter_data),
    Leaf_pool(file_name + "_Leaf"), Inter_pool(file_name + "_Inter") {
        if (Inter_data.empty()) {
            root_leaf = true;
            root_pos = Leaf_root.pos = Leaf_pool.Alloc();
            Leaf_cache.put(root_pos, Leaf_root);
        } else {
            Inter_data.file.seekg(-kAppendixSize, std::ios::end);
            Inter_data.file.read(reinterpret_cast<char *>(&root_pos), sizeof(int));
            Inter_data.file.read(reinterpret_cast<char *>(&root_leaf), sizeof(bool));
            if (root_leaf) {
                Leaf_cache.get(root_pos, Leaf_root);
            } else {
                Inter_cache.get(root_pos, Inter_root);
            }
        }
    };

    ~BPlusTree() {
        Inter_cache.dump();
        Leaf_cache.dump();
        Inter_data.file.seekp(0, std::ios::end);
        Inter_data.file.write(reinterpret_cast<char *>(&root_pos), sizeof(int));
        Inter_data.file.write(reinterpret_cast<char *>(&root_leaf), sizeof(bool));
    }

    int Find(const Element<U, T> &now) {
        if (!root_leaf) {
            Inter_now = Inter_root;
            while (1) {
                int pos = Inter_now.UpperBound(now);
                if ((!Inter_now.child_leaf[pos]) && (~Inter_now.child[pos])) Inter_cache.get(Inter_now.child[pos], Inter_now);
                else if (~Inter_now.child[pos]) {
                    return Inter_now.child[pos];
                }
            }
        } else {
            return Leaf_root.UpperBound(now);
        }
    }

    void Find(const U& now) {
        if (!root_leaf) {
            Inter_now = Inter_root;
            if (!Inter_now.count) {
                std::cout << "null";
                return ;
            }
            int pos;
            while (1) {
                pos = Inter_now.LowerBound(now);
                if ((!Inter_now.child_leaf[pos]) && (~Inter_now.child[pos])) {
                    Inter_cache.get(Inter_now.child[pos], Inter_now);
                } else if (~Inter_now.child[pos]) break;
            }
            Leaf_cache.get(Inter_now.child[pos], Leaf_now);
            pos = Leaf_now.LowerBound(now);
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
                Leaf_cache.get(Leaf_now.suc, Leaf_now);
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
        Inter_cache.get(Fa, Inter_fa);
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
        Inter_cache.put(Fa, Inter_fa);
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
            Leaf_cache.get(now.suc, Leaf_nex);
            Leaf_nex.pre = nex.pos;
            Leaf_cache.put(now.suc, Leaf_nex);
        }
        now.suc = nex.pos;
    }

    void SplitLeaf(LeafNode<U, T> &now) {
        LeafNode<U, T> nex(Leaf_pool.Alloc());
        SplitList(now, nex);
        SplitLeafKey(now, nex);
        nex.fa = now.fa;
        Leaf_cache.put(now.pos, now);
        Leaf_cache.put(nex.pos, nex);
        SplitFa(now.fa, nex.key[0], nex.pos, true);
    }

    void SplitChild(InterNode<U, T> &now, InterNode<U, T> &nex) {
        for (int i = 0; i <= nex.count; i++) {
            nex.child[i] = now.child[now.count + 1 + i];
            nex.child_leaf[i] = now.child_leaf[now.count + 1 + i];
            if (~nex.child[i]) {
                if (nex.child_leaf[i]) {
                    Leaf_cache.get(nex.child[i], Leaf_child);
                    Leaf_child.fa = nex.pos;
                    Leaf_cache.put(nex.child[i], Leaf_child);
                } else {
                    Inter_cache.get(nex.child[i], Inter_child);
                    Inter_child.fa = nex.pos;
                    Inter_cache.put(nex.child[i], Inter_child);
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
        InterNode<U, T> nex(Inter_pool.Alloc());
        Element<U, T> x;
        SplitInterKey(now, nex, x);
        SplitChild(now, nex);
        nex.fa = now.fa;
        Inter_cache.put(now.pos, now);
        Inter_cache.put(nex.pos, nex);
        SplitFa(now.fa, x, nex.pos, false);
    }

    void NewRoot(Node<U, T> &now, Node<U, T> &nex, Element<U, T> &x) {
        InterNode<U, T> new_root(Inter_pool.Alloc());
        new_root.count = 1;
        new_root.key[0] = x;
        new_root.child[0] = now.pos;
        new_root.child[1] = nex.pos;
        new_root.child_leaf[0] = now.is_leaf;
        new_root.child_leaf[1] = nex.is_leaf;
        nex.fa = now.fa = new_root.pos;
        Inter_cache.put(new_root.pos, new_root);
        root_pos = new_root.pos;
    }

    void SplitRoot() {
        if (!root_leaf) {
            InterNode<U, T> nex(Inter_pool.Alloc());
            Element<U, T> x;
            SplitInterKey(Inter_root, nex, x);
            SplitChild(Inter_root, nex);
            NewRoot(Inter_root, nex, x);
            Inter_cache.put(Inter_root.pos, Inter_root);
            Inter_cache.put(nex.pos, nex);
            Inter_cache.get(root_pos, Inter_root);
        } else {
            LeafNode<U, T> nex(Leaf_pool.Alloc());
            SplitLeafKey(Leaf_root, nex);
            SplitList(Leaf_root, nex);
            NewRoot(Leaf_root, nex, nex.key[0]);
            Leaf_cache.put(Leaf_root.pos, Leaf_root);
            Leaf_cache.put(nex.pos, nex);
            Inter_cache.get(root_pos, Inter_root);
            root_leaf = false;
        }
    }

    void InsertKey(LeafNode<U, T> &now, const Element<U, T> &x) {
        int pos = now.UpperBound(x);
        if (pos && now.key[pos - 1] == x) throw Exception("Element already exists.");
        for (int i = now.count; i > pos; i--) now.key[i] = now.key[i - 1];
        now.key[pos] = x;
        now.count++;
        Leaf_cache.put(now.pos, now);
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
        Leaf_cache.get(Leaf_pos, Leaf_now);
        InsertKey(Leaf_now, x);
        if (Leaf_now.count >= kMaxBlockSize) {
            SplitLeaf(Leaf_now);
            Inter_cache.get(Leaf_now.fa, Inter_now);
            while (Inter_now.count >= kMaxBlockSize) {
                if (Inter_now.pos == root_pos) {
                    SplitRoot();
                    break;
                }
                SplitInter(Inter_now);
                Inter_cache.get(Inter_now.fa, Inter_now);
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
        Inter_cache.put(Fa.pos, Fa);
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
            Leaf_cache.get(nex.suc, Leaf_nex_nex);
            Leaf_nex_nex.pre = now.pos;
            Leaf_cache.put(nex.suc, Leaf_nex_nex);
        }
        now.suc = nex.suc;
    }

    void MergeLeaf(LeafNode<U, T> &now, LeafNode<U, T> &nex, InterNode<U, T> &fa, int k) {
        MergeList(now, nex);
        MergeLeafKey(now, nex);
        Leaf_cache.put(now.pos, now);
        Leaf_pool.Recycle(nex.pos);
        MergeFa(fa, k);
    }

    void MergeChild(InterNode<U, T> &now, InterNode<U, T> &nex, int& count) {
        for (int i = 0; i <= nex.count; i++) {
            now.child[count + i] = nex.child[i];
            now.child_leaf[count + i] = nex.child_leaf[i];
            if (~nex.child[i]) {
                if (nex.child_leaf[i]) {
                    Leaf_cache.get(nex.child[i], Leaf_child);
                    Leaf_child.fa = now.pos;
                    Leaf_cache.put(nex.child[i], Leaf_child);
                } else {
                    Inter_cache.get(nex.child[i], Inter_child);
                    Inter_child.fa = now.pos;
                    Inter_cache.put(nex.child[i], Inter_child);
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
        Inter_cache.put(now.pos, now);
        Inter_pool.Recycle(nex.pos);
        MergeFa(fa, k);
    }

    void MergeRoot(const Element<U, T> &x, const Element<U, T> &x_suc, bool &flag) {
        if (!root_leaf) {
            if (!Inter_root.child_leaf[0]) {
                Inter_cache.get(Inter_root.child[0], Inter_now);
                Inter_cache.get(Inter_root.child[1], Inter_nex);
                int Count = Inter_now.count + 1;
                MergeInterKey(Inter_now, Inter_nex, Inter_root.key[0]);
                MergeChild(Inter_now, Inter_nex, Count);
                if (Inter_now.key[Count - 1] == x) {
                    flag = true;
                    Inter_now.key[Count - 1] = x_suc;
                }
                Inter_root = Inter_now;
                root_pos = Inter_now.pos;
                Inter_cache.put(root_pos, Inter_root);
                Inter_pool.Recycle(Inter_nex.pos);
            } else {
                Leaf_cache.get(Inter_root.child[0], Leaf_now);
                Leaf_cache.get(Inter_root.child[1], Leaf_nex);
                MergeLeafKey(Leaf_now, Leaf_nex);
                MergeList(Leaf_now, Leaf_nex);
                Leaf_root = Leaf_now;
                root_pos = Leaf_now.pos;
                root_leaf = true;
                Leaf_cache.put(root_pos, Leaf_root);
                Leaf_pool.Recycle(Leaf_nex.pos);
            }
        }
    }

    bool EraseKey(LeafNode<U, T> &now, const Element<U, T> &x, Element<U, T> &suc, bool &flag) {
        int pos = now.LowerBound(x);
        if (pos >= now.count || now.key[pos] != x) {
            //throw Exception("Element doesn't exist.");
            return false;
        }
        for (int i = pos; i < now.count; i++) now.key[i] = now.key[i + 1];
        now.count--;
        if (now.count > pos) suc = now.key[pos];
        else {
            if (~now.suc) {
                Leaf_cache.get(now.suc, Leaf_now);
                suc = Leaf_now.key[0];
            }
        }
        Leaf_cache.put(now.pos, now);
        if (!pos) {
            if (~now.fa) {
                Inter_cache.get(now.fa, Inter_fa);
                int k = Inter_fa.LowerBound(x);
                if (Inter_fa.key[k] == x) {
                    if (now.count) {
                        Inter_fa.key[k] = now.key[0];
                        flag = true;
                        Inter_cache.put(now.fa, Inter_fa);
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
            Inter_cache.get(nex.fa, fa);
            k = fa.UpperBound(x);
            if (k > 0 && (~fa.child[k - 1])) {
                Inter_cache.get(fa.child[k - 1], now);
                k--;
            } else {
                now = nex;
                Inter_cache.get(fa.child[1], nex);
            }
        }
    }

    void ChangeSuc(const Element<U, T> &x, Element<U, T> &x_suc) {
        Inter_now = Inter_root;
        while (1) {
            int pos = Inter_now.LowerBound(x);
            if (pos == Inter_now.count) {
                if ((!Inter_now.child_leaf[pos]) && (~Inter_now.child[pos])) {
                    Inter_cache.get(Inter_now.child[pos], Inter_now);
                    continue;
                }
                else return ;
            }
            if (Inter_now.key[pos] == x) {
                Inter_now.key[pos] = x_suc;
                Inter_cache.put(Inter_now.pos, Inter_now);
                if (Inter_now.pos == root_pos) Inter_root = Inter_now;
                return ;
            } else if (Inter_now.key[pos] > x) {
                if ((!Inter_now.child_leaf[pos]) && (~Inter_now.child[pos])) Inter_cache.get(Inter_now.child[pos], Inter_now);
                else return ;
            }
        }
    }

    void BorrowLeafPre(LeafNode<U, T> &now, LeafNode<U, T> &nex, InterNode<U, T>& fa, int k) {
        now.key[now.count++] = nex.key[0];
        for (int i = 0; i < nex.count; i++) nex.key[i] = nex.key[i + 1];
        nex.count--;
        fa.key[k] = nex.key[0];
        Leaf_cache.put(now.pos, now);
        Leaf_cache.put(nex.pos, nex);
        Inter_cache.put(fa.pos, fa);
        if (fa.pos == root_pos) Inter_root = fa;
    }

    void BorrowLeafSuc(LeafNode<U, T> &now, LeafNode<U, T> &nex, InterNode<U, T>& fa, int k) {
        for (int i = nex.count; i >= 1; i--) nex.key[i] = nex.key[i - 1];
        nex.count++;
        nex.key[0] = now.key[--now.count];
        fa.key[k] = nex.key[0];
        Leaf_cache.put(now.pos, now);
        Leaf_cache.put(nex.pos, nex);
        Inter_cache.put(fa.pos, fa);
        if (fa.pos == root_pos) Inter_root = fa;
    }

    void BorrowInterPre(InterNode<U, T> &now, InterNode<U, T> &nex, InterNode<U, T>& fa, int k) {
        now.key[now.count++] = nex.key[0];
        now.child[now.count] = nex.child[0];
        now.child_leaf[now.count] = nex.child_leaf[0];
        if (now.child_leaf[now.count]) {
            Leaf_cache.get(now.child[now.count], Leaf_child);
            Leaf_child.fa = now.pos;
            Leaf_cache.put(now.child[now.count], Leaf_child);
        } else {
            Inter_cache.get(now.child[now.count], Inter_child);
            Inter_child.fa = now.pos;
            Inter_cache.put(now.child[now.count], Inter_child);
        }
        for (int i = 0; i < nex.count; i++) nex.key[i] = nex.key[i + 1];
        for (int i = 0; i <= nex.count; i++) {
            nex.child[i] = nex.child[i + 1];
            nex.child_leaf[i] = nex.child_leaf[i + 1];
        }
        nex.count--;
        std::swap(fa.key[k], now.key[now.count - 1]);
        Inter_cache.put(now.pos, now);
        Inter_cache.put(nex.pos, nex);
        Inter_cache.put(fa.pos, fa);
        if (fa.pos == root_pos) Inter_root = fa;
    }

    void BorrowInterSuc(InterNode<U, T> &now, InterNode<U, T> &nex, InterNode<U, T>& fa, int k) {
        for (int i = nex.count; i >= 1; i--) nex.key[i] = nex.key[i - 1];
        for (int i = nex.count + 1; i >= 1; i--) {
            nex.child[i] = nex.child[i - 1];
            nex.child_leaf[i] = nex.child_leaf[i - 1];
        }
        nex.count++;
        nex.child[0] = now.child[now.count];
        nex.child_leaf[0] = now.child_leaf[now.count];
        if (now.child_leaf[now.count]) {
            Leaf_cache.get(now.child[now.count], Leaf_child);
            Leaf_child.fa = nex.pos;
            Leaf_cache.put(now.child[now.count], Leaf_child);
        } else {
            Inter_cache.get(now.child[now.count], Inter_child);
            Inter_child.fa = nex.pos;
            Inter_cache.put(now.child[now.count], Inter_child);
        }
        nex.key[0] = now.key[--now.count];
        std::swap(fa.key[k], nex.key[0]);
        Inter_cache.put(now.pos, now);
        Inter_cache.put(nex.pos, nex);
        Inter_cache.put(fa.pos, fa);
        if (fa.pos == root_pos) Inter_root = fa;
    }

    void Erase(const Element<U, T> &x) {
        Element<U, T> x_suc;
        bool flag = false;
        if (root_leaf) {
            EraseKey(Leaf_root, x, x_suc, flag);
            return ;
        }
        int Leaf_pos = Find(x);
        Leaf_cache.get(Leaf_pos, Leaf_nex);
        Inter_cache.get(Leaf_nex.fa, Inter_fa);
        int k = Inter_fa.UpperBound(Leaf_nex.key[0]);
        bool ret = EraseKey(Leaf_nex, x, x_suc, flag);
        if (!ret) return ;
        if (k > 0 && (~Inter_fa.child[k - 1])) {
            Leaf_cache.get(Inter_fa.child[k - 1], Leaf_now);
            k--;
        } else {
            Leaf_now = Leaf_nex;
            Leaf_cache.get(Inter_fa.child[1], Leaf_nex);
        }
        if (Leaf_now.count <= kMinBlockSize && Leaf_nex.count > kMinBlockSize) {
            BorrowLeafPre(Leaf_now, Leaf_nex, Inter_fa, k);
        } else if (Leaf_now.count > kMinBlockSize && Leaf_nex.count <= kMinBlockSize) {
            BorrowLeafSuc(Leaf_now, Leaf_nex, Inter_fa, k);
        } else if (Leaf_now.count <= kMinBlockSize && Leaf_nex.count <= kMinBlockSize) {
            if (Inter_fa.pos == root_pos && Inter_root.count == 1) {
                MergeRoot(x, x_suc, flag);
                return ;
            }
            Element<U, T> now_x = Inter_fa.key[0];
            MergeLeaf(Leaf_now, Leaf_nex, Inter_fa, k);
            if (Inter_fa.pos == root_pos) return ;
            Inter_nex = Inter_fa;
            FindPre(Inter_now, Inter_nex, Inter_fa, k, now_x);
            while (1) {
                now_x = Inter_fa.key[0];
                if (Inter_now.count <= kMinBlockSize && Inter_nex.count > kMinBlockSize) {
                    BorrowInterPre(Inter_now, Inter_nex, Inter_fa, k);
                    break;
                } else if (Inter_now.count > kMinBlockSize && Inter_nex.count <= kMinBlockSize) {
                    BorrowInterSuc(Inter_now, Inter_nex, Inter_fa, k);
                    break;
                } else if (Inter_now.count <= kMinBlockSize && Inter_nex.count <= kMinBlockSize) {
                    if (Inter_fa.pos == root_pos && Inter_root.count == 1) {
                        MergeRoot(x, x_suc, flag);
                        break ;
                    }
                    MergeInter(Inter_now, Inter_nex, Inter_fa, k, x, x_suc, flag);
                } else break;
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
        Leaf_cache.get(pos, now);
        printLeaf(now);
    }

    static void printLeaf(LeafNode<U, T> &now) {
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
                Leaf_cache.get(now.child[i], Leaf);
                printLeaf(Leaf);
            }
            else if (~now.child[i]) {
                InterNode<U, T> Inter;
                Inter_cache.get(now.child[i], Inter);
                print(Inter);
            }
        }
        return ;
    }

    void print(int pos) {
        InterNode<U, T> now;
        Inter_cache.get(pos, now);
        print(now);
    }

    void printInter(int pos) {
        InterNode<U, T> now;
        Inter_cache.get(pos, now);
        printInter(now);
    }

    static void printInter(InterNode<U, T> &now) {
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
        std::cout << "Inter_count " << Inter_pool.Node_count << " Leaf_count " << Leaf_pool.Node_count << '\n';
        if (root_leaf) printLeaf(Leaf_root);
        else print(Inter_root);
        std::cout << "-------\n";
    }

};



#endif //TICKETSYSTEM_BPLUSTREE_H