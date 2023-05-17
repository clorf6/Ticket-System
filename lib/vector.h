//
// Created by 屋顶上的小丑 on 2023/4/27.
//

#ifndef TICKETSYSTEM_VECTOR_H
#define TICKETSYSTEM_VECTOR_H

#include "Exception.h"
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <iostream>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
    template<typename T>
    class vector {
    public:
        /**
         * TODO
         * a type for actions of the elements of a vector, and you should write
         *   a class named const_iterator with same interfaces.
         */
        /**
         * you can see RandomAccessIterator at CppReference for help.
         */
        T *data;
        int _size;
        int _capacity;

        class const_iterator;

        class iterator {
            // The following code is written for the C++ type_traits library.
            // Type traits is a C++ feature for describing certain properties of a type.
            // For instance, for an iterator, iterator::value_type is the type that the
            // iterator points to.
            // STL algorithms and containers may use these type_traits (e.g. the following
            // typedef) to work properly. In particular, without the following code,
            // @code{std::sort(iter, iter1);} would not compile.
            // See these websites for more information:
            // https://en.cppreference.com/w/cpp/header/type_traits
            // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
            // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;

        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            friend class vector;

            T *head;
            int delta;
        public:
            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            iterator(T *_head, const int &_delta) : head(_head), delta(_delta) {};

            iterator operator+(const int &n) const {
                //TODO
                return iterator(head, delta + n);
            }

            iterator operator-(const int &n) const {
                //TODO
                return iterator(head, delta - n);
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                //TODO
                if (head != rhs.head) {
                    throw Exception("invalid_iterator");
                }
                return delta - rhs.delta;
            }

            iterator &operator+=(const int &n) {
                //TODO
                delta += n;
                return (*this);
            }

            iterator &operator-=(const int &n) {
                //TODO
                delta -= n;
                return (*this);
            }

            /**
             * TODO iter++
             */
            iterator operator++(int) {
                iterator ret = (*this);
                delta++;
                return ret;
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                delta++;
                return (*this);
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator ret = (*this);
                delta--;
                return ret;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                delta--;
                return (*this);
            }

            /**
             * TODO *it
             */
            T &operator*() const {
                return *(head + delta);
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                return head == rhs.head && delta == rhs.delta;
            }

            bool operator==(const const_iterator &rhs) const {
                return head == rhs.head && delta == rhs.delta;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return head != rhs.head || delta != rhs.delta;
            }

            bool operator!=(const const_iterator &rhs) const {
                return head != rhs.head || delta != rhs.delta;
            }
        };

        /**
         * TODO
         * has same function as iterator, just for a const object.
         */
        class const_iterator {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;

        private:
            /*TODO*/
            friend class vector;

            T *head;
            int delta;
        public:
            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            const_iterator(T *_head, const int &_delta) : head(_head), delta(_delta) {};

            const_iterator operator+(const int &n) const {
                //TODO
                const_iterator ret;
                ret.head = head;
                ret.delta = delta + n;
                return ret;
            }

            const_iterator operator-(const int &n) const {
                //TODO
                const_iterator ret;
                ret.head = head;
                ret.delta = delta - n;
                return ret;
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const const_iterator &rhs) const {
                //TODO
                if (head != rhs.head) {
                    throw Exception("invalid_iterator");
                }
                return delta - rhs.delta;
            }

            const_iterator &operator+=(const int &n) {
                //TODO
                delta += n;
                return (*this);
            }

            const_iterator &operator-=(const int &n) {
                //TODO
                delta -= n;
                return (*this);
            }

            /**
             * TODO iter++
             */
            const_iterator operator++(int) {
                const_iterator ret = (*this);
                delta++;
                return ret;
            }

            /**
             * TODO ++iter
             */
            const_iterator &operator++() {
                delta++;
                return (*this);
            }

            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                const_iterator ret = (*this);
                delta--;
                return ret;
            }

            /**
             * TODO --iter
             */
            const_iterator &operator--() {
                delta--;
                return (*this);
            }

            /**
             * TODO *it
             */
            T operator*() const {
                return head[delta];
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                return head == rhs.head && delta == rhs.delta;
            }

            bool operator==(const const_iterator &rhs) const {
                return head == rhs.head && delta == rhs.delta;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return head != rhs.head || delta != rhs.delta;
            }

            bool operator!=(const const_iterator &rhs) const {
                return head != rhs.head || delta != rhs.delta;
            }
        };

        /**
         * TODO Constructs
         * At least two: default constructor, copy constructor
         */
        vector() {
            _size = 0;
            _capacity = 4;
            data = (T *) malloc(sizeof(T) * _capacity);
            memset(data, 0, sizeof(T) * _capacity);
        }

        vector(const vector &other) {
            _size = other._size;
            _capacity = other._capacity;
            data = (T *) malloc(sizeof(T) * _capacity);
            memset(data, 0, sizeof(T) * _capacity);
            for (int i = 0; i < _size; i++) {
                data[i] = other.data[i];
            }
        }

        /**
         * TODO Destructor
         */
        ~vector() {
            for (int i = 0; i < _size; i++) {
                data[i].~T();
            }
            _size = _capacity = 0;
            free(data);
            data = nullptr;
        }

        /**
         * double the capacity
         */
        void doubleSpace() {
            _capacity <<= 1;
            T *new_data = (T *) malloc(sizeof(T) * _capacity);
            memset(new_data, 0, sizeof(T) * _capacity);
            for (int i = 0; i < _size; i++) {
                new_data[i] = data[i];
                data[i].~T();
            }
            free(data);
            data = new_data;
        }

        void halfSpace() {
            _capacity >>= 1;
            T *new_data = (T *) malloc(sizeof(T) * _capacity);
            memset(new_data, 0, sizeof(T) * _capacity);
            for (int i = 0; i < _size; i++) {
                new_data[i] = data[i];
                data[i].~T();
            }
            free(data);
            data = new_data;
        }

        /**
         * TODO Assignment operator
         */
        vector &operator=(const vector &other) {
            if (this != &other) {
                for (int i = 0; i < _size; i++) {
                    data[i].~T();
                }
                _size = _capacity = 0;
                free(data);
                _size = other._size;
                _capacity = other._capacity;
                data = (T *) malloc(sizeof(T) * _capacity);
                memset(data, 0, sizeof(T) * _capacity);
                for (int i = 0; i < _size; i++) {
                    data[i] = other.data[i];
                }
            }
            return (*this);
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         */
        T &at(const size_t &pos) {
            if (pos < 0 || pos >= _size) {
                throw Exception("index_out_of_bound");
            }
            return data[pos];
        }

        const T &at(const size_t &pos) const {
            if (pos < 0 || pos >= _size) {
                throw Exception("index_out_of_bound");
            }
            return data[pos];
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         * !!! Pay attentions
         *   In STL this operator does not check the boundary but I want you to do.
         */
        T &operator[](const size_t &pos) {
            if (pos < 0 || pos >= _size) {
                throw Exception("index_out_of_bound");
            }
            return data[pos];
        }

        const T &operator[](const size_t &pos) const {
            if (pos < 0 || pos >= _size) {
                throw Exception("index_out_of_bound");
            }
            return data[pos];
        }

        /**
         * access the first element.
         * throw container_is_empty if size == 0
         */
        const T &front() const {
            if (!_size) {
                throw Exception("container_is_empty");
            }
            return data[0];
        }

        /**
         * access the last element.
         * throw container_is_empty if size == 0
         */
        const T &back() const {
            if (!_size) {
                throw Exception("container_is_empty");
            }
            return data[_size - 1];
        }

        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            return iterator(data, 0);
        }

        const_iterator cbegin() const {
            return const_iterator(data, 0);
        }

        /**
         * returns an iterator to the end.
         */
        iterator end() {
            return iterator(data, _size);
        }

        const_iterator cend() const {
            return const_iterator(data, _size);
        }

        /**
         * checks whether the container is empty
         */
        bool empty() const {
            if (!_size) {
                return true;
            }
            return false;
        }

        /**
         * returns the number of elements
         */
        size_t size() const {
            return _size;
        }

        /**
         * clears the contents
         */
        void clear() {
            for (int i = 0; i < _size; i++) {
                data[i].~T();
            }
            _size = 0;
        }

        /**
         * inserts value before pos
         * returns an iterator pointing to the inserted value.
         */
        iterator insert(iterator pos, const T &value) {
            if (pos.delta < 0 || pos.delta > _size) {
                throw Exception("index_out_of_bound");
            }
            _size++;
            if (_size == _capacity) {
                doubleSpace();
            }
            for (int i = _size; i > pos.delta; i--) {
                data[i] = data[i - 1];
            }
            data[pos.delta] = value;
            pos.head = data;
            return pos;
        }

        /**
         * inserts value at index ind.
         * after inserting, this->at(ind) == value
         * returns an iterator pointing to the inserted value.
         * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
         */
        iterator insert(const size_t &ind, const T &value) {
            if (ind < 0 || ind > _size) {
                throw Exception("index_out_of_bound");
            }
            _size++;
            if (_size == _capacity) {
                doubleSpace();
            }
            for (int i = _size; i > ind; i--) {
                data[i] = data[i - 1];
            }
            data[ind] = value;
            return iterator(data, ind);
        }

        /**
         * removes the element at pos.
         * return an iterator pointing to the following element.
         * If the iterator pos refers the last element, the end() iterator is returned.
         */
        iterator erase(iterator pos) {
            if (pos.delta < 0 || pos.delta >= _size) {
                throw Exception("index_out_of_bound");
            }
            for (int i = pos.delta; i < _size - 1; i++) {
                data[i] = data[i + 1];
            }
            data[--_size].~T();
            if (_size < (_capacity >> 1)) halfSpace();
            pos.head = data;
            return pos;
        }

        /**
         * removes the element with index ind.
         * return an iterator pointing to the following element.
         * throw index_out_of_bound if ind >= size
         */
        iterator erase(const size_t &ind) {
            if (ind < 0 || ind >= _size) {
                throw Exception("index_out_of_bound");
            }
            for (int i = ind; i < _size - 1; i++) {
                data[i] = data[i + 1];
            }
            data[--_size].~T();
            if (_size < (_capacity >> 1)) halfSpace();
            return iterator(data, ind);
        }

        /**
         * adds an element to the end.
         */
        void push_back(const T &value) {
            _size++;
            if (_size == _capacity) {
                doubleSpace();
            }
            data[_size - 1] = value;
        }

        /**
         * remove the last element from the end.
         * throw container_is_empty if size() == 0
         */
        void pop_back() {
            if (!_size) {
                throw Exception("container_is_empty");
            }
            data[--_size].~T();
            if (_size < (_capacity >> 1)) halfSpace();
        }

        void swap(vector<T> &other) {
            std::swap(_size, other._size);
            std::swap(_capacity, other._capacity);
            std::swap(data, other.data);
        }
    };


}

#endif //TICKETSYSTEM_VECTOR_H
