#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
    template<typename T>
    class vector {
    private:
        T *data;
        long long maxSize;
        long long currentLength;
    public:
        /**
         * TODO
         * a type for actions of the elements of a vector, and you should write
         *   a class named const_iterator with same interfaces.
         */
        /**
         * you can see RandomAccessIterator at CppReference for help.
         */
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
            value_type *data;
            int loc;

        public:
            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */

            iterator() = default;

            iterator(value_type *_data, int _loc) {
                data = _data;
                loc = _loc;
            }

            iterator(iterator other, const value_type value) {
            data = other.data;
                *data = value;
                loc = other.loc;
            }

            iterator operator+(const int &n) const {
                //TODO
                iterator result;
                result.data = data + n;
                return result;
            }

            iterator operator-(const int &n) const {
                //TODO
                iterator result;
                result.data = data - n;
                return result;
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                //TODO
                if ((data - loc) != (rhs.data - rhs.loc)) throw invalid_iterator();
                int distance = loc - rhs.loc;
                if (distance >= 0)
                    return distance;
                else
                    return (-1) * distance;
            }

            iterator &operator+=(const int &n) {
                //TODO
                data += n;
                loc += n;
                return *this;
            }

            iterator &operator-=(const int &n) {
                //TODO
                data -= n;
                loc -= n;
                return *this;
            }

            /**
             * TODO iter++
             */
            iterator operator++(int) {
                iterator tmp;
                tmp.data = data;
                tmp.loc = loc;
                data++;
                loc++;
                return tmp;
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                data++;
                loc++;
                return *this;
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator tmp;
                tmp.data = data;
                tmp.loc = loc;
                data--;
                loc--;
                return tmp;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                data--;
                loc--;
                return *this;
            }

            /**
             * TODO *it
             */
            T &operator*() const {
                return *data;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                if (data == rhs.data) return true;
                return false;
            }

            bool operator==(const const_iterator &rhs) const {
                if (data == rhs.data) return true;
                return false;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                if (data != rhs.data) return true;
                return false;
            }

            bool operator!=(const const_iterator &rhs) const {
                if (data != rhs.data) return true;
                return false;
            }
        };
        class const_iterator {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;

        private:
            /*TODO*/
            const value_type *data;
            int loc;

        public:

            const_iterator(value_type *_data, int _loc) {
                data = _data;
                loc = _loc;
            }

            iterator operator+(const int &n) const {
                iterator result;
                result.data = data + n;
                return result;
            }

            iterator operator-(const int &n) const {
                iterator result;
                result.data = data - n;
                return result;
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                if ((data - loc) != (rhs.data - rhs.loc)) throw invalid_iterator();
                int distance = loc - rhs.loc;
                if (distance >= 0)
                    return distance;
                else
                    return (-1) * distance;
            }

            const_iterator &operator+=(const int &n) {
                data += n;
                loc += n;
                return *this;
            }

            const_iterator &operator-=(const int &n) {
                data -= n;
                loc -= n;
                return *this;
            }

            const_iterator operator++(int) {
                iterator tmp;
                tmp.data = data;
                tmp.loc = loc;
                data++;
                loc++;
                return tmp;
            }

            const_iterator &operator++() {
                data++;
                loc++;
                return *this;
            }

            const_iterator operator--(int) {
                iterator tmp;
                tmp.data = data;
                tmp.loc = loc;
                data--;
                loc--;
                return tmp;
            }

            iterator &operator--() {
                data--;
                loc--;
                return *this;
            }

            const T &operator*() const {
                return *data;
            }

            bool operator==(const iterator &rhs) const {
                if (data == rhs.data) return true;
                return false;
            }

            bool operator==(const const_iterator &rhs) const {
                if (data == rhs.data) return true;
                return false;
            }

            bool operator!=(const iterator &rhs) const {
                if (data != rhs.data) return true;
                return false;
            }

            bool operator!=(const const_iterator &rhs) const {
                if (data != rhs.data) return true;
                return false;
            }

        };

        vector() {
            maxSize = 100000;
            currentLength = 0;
            data = (T *) malloc(sizeof(T) * (maxSize + 1));
        }

        vector(const vector &other) {
            maxSize = other.maxSize;
            currentLength = other.currentLength;
            data = (T *) malloc(sizeof(T) * (maxSize + 1));
            for (int i = 0; i < other.currentLength; i++) {
                new (data+i) T(other.data[i]);
            }
        }

        ~vector() {
            if (currentLength != 0) {
                for (int i = 0; i < currentLength; ++i) {
                    data[i].~T();
                }
            }
            free(data);
        }

        vector &operator=(const vector &other) {
            if (data == other.data) return *this;
            for (int i = 0; i < currentLength; ++i) {
                data[i].~T();
            }
            free(data);
            maxSize = other.maxSize;
            currentLength = other.currentLength;
            data = (T *) malloc(sizeof(T) * (maxSize + 1));
            for (long long i = 0; i < other.currentLength; i++) {
                new (data+i) T(other.data[i]);
            }
            return *this;
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         */

        T &at(const size_t &pos){
            if (pos >= currentLength || pos < 0) throw index_out_of_bound();
            return data[pos];
        }

        const T &at(const size_t &pos) const {
            if (pos >= currentLength || pos < 0) throw index_out_of_bound();
            return data[pos];
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         * !!! Pay attentions
         *   In STL this operator does not check the boundary but I want you to do.
         */
        T &operator[](const size_t &pos) {
            if (pos >= currentLength || pos < 0)
                throw index_out_of_bound();
            return data[pos];
        }

        const T &operator[](const size_t &pos) const {
            if (pos >= currentLength || pos < 0) throw index_out_of_bound();
            return data[pos];
        }

        /**
         * access the first element.
         * throw container_is_empty if size == 0
         */
        const T &front() const {
            return data[0];
        }

        /**
         * access the last element.
         * throw container_is_empty if size == 0
         */
        const T &back() const {
            return data[currentLength - 1];
        }

        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            iterator front(data, 1);
            return front;
        }

        const_iterator cbegin() const {
            const_iterator front(data, 1);
            return front;
        }

        /**
         * returns an iterator to the end.
         */
        iterator end() {
            iterator back(data + currentLength, currentLength + 1);
            return back;
        }

        const_iterator cend() const {
            const_iterator back(data + currentLength, currentLength + 1);
            return back;
        }

        /**
         * checks whether the container is empty
         */
        bool empty() const {
            if (currentLength == 0) return true;
            return false;
        }

        /**
         * returns the number of elements
         */
        size_t size() const {
            return currentLength;
        }

        /**
         * clears the contents
         */
        void clear() {
            for (int i = 0; i < currentLength; ++i)
                data[i].~T();
            currentLength = 0;
        }

        /**
         * inserts value before pos
         * returns an iterator pointing to the inserted value.
         */
        iterator insert(iterator pos, const T &value) {
            currentLength++;
            if (currentLength > maxSize) {
                maxSize *= 2;
                T *tmp = (T *) malloc(sizeof(T) * (currentLength - 1));
                for (long long i = 0; i < currentLength - 1; ++i) {
                    tmp[i] = data[i];
                }
                free(data);
                data = (T *) malloc(sizeof(T) * (maxSize + 1));
                for (long long i = 0; i < currentLength - 1; ++i) {
                    data[i] = tmp[i];
                }
                free(tmp);
            }
            T tmp = *pos;
            iterator result(pos, value);
            T original = tmp;
            pos++;
            while (pos != (end() - 1)) {
                tmp = *pos;
                *pos = original;
                original = tmp;
                pos++;
            }
            new (data + currentLength - 1) T(original);
            return result;
        }

        /**
         * inserts value at index ind.
         * after inserting, this->at(ind) == value
         * returns an iterator pointing to the inserted value.
         * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
         */
        iterator insert(const size_t &ind, const T &value) {
            if (ind < 0 || ind >= currentLength) throw index_out_of_bound();
            currentLength++;
            if (currentLength > maxSize) {
                maxSize *= 2;
                T *tmp = (T *) malloc(sizeof(T) * currentLength);
                for (long long i = 0; i < currentLength - 1; ++i) {
                    tmp[i] = data[i];
                }
                free(data);
                data = (T *) malloc(sizeof(T) * (maxSize + 1));
                for (long long i = 0; i < ind; ++i) {
                    data[i] = tmp[i];
                }
                data[ind] = value;
                for (long long i = ind + 1; i < currentLength; ++i) {
                    data[i] = tmp[i - 1];
                }
                free(tmp);
            } else {
                new (data + currentLength - 1) T(data[currentLength - 2]);
                for (long long i = currentLength - 2; i > ind; --i) {
                    data[i] = data[i - 1];
                }
                data[ind] = value;
            }
            iterator result(data + ind, ind);
            return result;
        }

        /**
         * removes the element at pos.
         * return an iterator pointing to the following element.
         * If the iterator pos refers the last element, the end() iterator is returned.
         */
        iterator erase(iterator pos) {
            if (pos == (end() - 1)) {
                iterator tmp = end();
                currentLength--;
                (*pos).~T();
                return tmp;
            }
            currentLength--;
            iterator result = pos + 1;
            while (pos != end()) {
                *pos = *(pos + 1);
                pos++;
            }
            (*(data + currentLength)).~T();
            return result;
        }

        /**
         * removes the element with index ind.
         * return an iterator pointing to the following element.
         * throw index_out_of_bound if ind >= size
         */
        iterator erase(const size_t &ind) {
            if (ind >= currentLength) throw index_out_of_bound();
            iterator result(data + ind + 1, ind);
            for (long long i = ind; i < currentLength - 1; ++i) {
                data[i] = data[i + 1];
            }
            currentLength--;
            (*(data + currentLength)).~T();
            return result;
        }

        /**
         * adds an element to the end.
         */
        void push_back(const T &value) {
            currentLength++;
            if (currentLength > maxSize) {
                maxSize *= 2;
                T *tmp = (T *) malloc(sizeof(T) * (currentLength - 1));
                for (long long i = 0; i < currentLength - 1; ++i) {
                    tmp[i] = data[i];
                }
                free(data);
                data = (T *) malloc(sizeof(T) * (maxSize + 1));
                for (long long i = 0; i < currentLength - 1; ++i) {
                    data[i] = tmp[i];
                }
                free(tmp);
            }
            new (data + currentLength - 1) T(value);
        }

        /**
         * remove the last element from the end.
         * throw container_is_empty if size() == 0
         */
        void pop_back() {
            if (currentLength == 0) throw container_is_empty();
            (*(data + currentLength - 1)).~T();
            currentLength--;
        }
    };


}

#endif

