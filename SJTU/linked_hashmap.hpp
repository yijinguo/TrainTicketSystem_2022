/**
 * implement a container like std::linked_hashmap
 */
#ifndef SJTU_LINKEDHASHMAP_HPP
#define SJTU_LINKEDHASHMAP_HPP

// only for std::equal_to<T> and std::hash<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {
    
template<
	class Key,
	class T,
	class Hash = std::hash<Key>, 
	class Equal = std::equal_to<Key>
> class linked_hashmap {
public:
	typedef pair<const Key, T> value_type;

    struct link {
        value_type *data;
        link *next;  //双链表
        link *front; //双链表
        link *nxt; //单链表

        link() : data(nullptr), nxt(nullptr), next(nullptr), front(nullptr) {}
        link(const value_type &d,  link *_front = nullptr,link *_next = nullptr, link *_nxt = nullptr) : next(_next), front(_front), nxt(_nxt) {
            data = (value_type *) malloc(sizeof(value_type));
            new(data) value_type(d);
        }
        ~link() {
            if (data) {
                (*data).~value_type();
                free(data);
                data = nullptr;
            }
        }
    };

    const int initialSize = 200;  //初始大小，待定
    const double loadFactor = 0.5;  //所能接受的最多元素个数占容量的比例,待定
    const int times = 2;  //每次更新时扩大的倍数

    int capacity;  //允许的总容量
    size_t total;  //实际数据量
    link **array;  //指针数组，保存head位置
    link *head;  //插入顺序双链表，头节点
    link *rear;  //插入顺序双链表，尾节点

	class const_iterator;

	class iterator {
        friend class linked_hashmap;
        friend class const_iterator;
	private:
		link *head;
        link *it;
        bool end;
	public:
		iterator() {
            head = it = nullptr;
			end = false;
		}
		iterator(const iterator &other) {
			head = other.head;
            it = other.it;
            end = other.end;
		}
        iterator(link *h, link *t, bool _end = false):head(h), it(t), end(_end){}

		iterator operator++(int) {
            if (!head || end) throw invalid_iterator();
            iterator tmp(*this);
            if (it->next)
                it = it->next;
            else
                end = true;
            return tmp;
        }

		iterator & operator++() {
            if (!head || end) throw invalid_iterator();
            if (it->next)
                it = it->next;
            else
                end = true;
            return *this;
        }

		iterator operator--(int) {
            if (!head) throw invalid_iterator();
            iterator tmp(*this);
            if (end) {
                end = false;
            } else if (it->front == head) {
                throw invalid_iterator();
            } else {
                it = it->front;
            }
            return tmp;
        }

		iterator & operator--() {
            if (!head) throw invalid_iterator();
            if (end) {
                end = false;
            } else if (it->front == head) {
                throw invalid_iterator();
            } else {
                it = it->front;
            }
            return *this;
        }

		value_type & operator*() const {
            //if (end) throw invalid_iterator();
            return *it->data;
        }
		bool operator==(const iterator &rhs) const {
            if (!head && !rhs.head) return true;
            if (it != rhs.it) return false;
            if (end != rhs.end) return false;
            return true;
        }
		bool operator==(const const_iterator &rhs) const {
            if (!head && !rhs.head) return true;
            if (it != rhs.it) return false;
            if (end != rhs.end) return false;
            return true;
        }

		bool operator!=(const iterator &rhs) const {
            if (*this == rhs) return false;
            return true;
        }
		bool operator!=(const const_iterator &rhs) const {
            if (*this == rhs) return false;
            return true;
        }

		value_type* operator->() const noexcept {
            if (end) throw invalid_iterator();
            return it->data;
        }
	};
	class const_iterator {
        friend class iterator;
    private:
        link *head;
        link *it;
        bool end;
    public:
        const_iterator() {
            head = it = nullptr;
            end = false;
        }
        const_iterator(const const_iterator &other) {
            head = other.head;
            it = other.it;
            end = other.end;
        }
        const_iterator(const iterator &other) {
            head = other.head;
            it = other.it;
            end = other.end;
        }
        const_iterator(link *h, link *t, bool _end = false) : head(h), it(t), end(_end) {}

        const_iterator operator++(int) {
            if (!head || end) throw invalid_iterator();
            const_iterator tmp(*this);
            if (it->next)
                it = it->next;
            else
                end = true;
            return tmp;
        }

        const_iterator & operator++() {
            if (!head || end) throw invalid_iterator();
            if (it->next)
                it = it->next;
            else
                end = true;
            return *this;
        }

        const_iterator operator--(int) {
            if (!head) throw invalid_iterator();
            const_iterator tmp(*this);
            if (end) {
                end = false;
            } else if (it->front == head) {
                throw invalid_iterator();
            } else {
                it = it->front;
            }
            return tmp;
        }

        const_iterator & operator--() {
            if (!head) throw invalid_iterator();
            if (end) {
                end = false;
            } else if (it->front == head) {
                throw invalid_iterator();
            } else {
                it = it->front;
            }
            return *this;
        }

        value_type operator*() const {
            //if (end) throw invalid_iterator();
            return *it->data;
        }

        //bool operator==(const iterator &rhs) const {
        //    if (!head && !rhs.head) return true;
        //    if (it != rhs.it) return false;
        //    if (end != rhs.end) return false;
        //    return true;
        //}
        bool operator==(const const_iterator &rhs) const {
            if (!head && !rhs.head) return true;
            if (it != rhs.it) return false;
            if (end != rhs.end) return false;
            return true;
        }

        //bool operator!=(const iterator &rhs) const {
        //    if (*this == rhs) return false;
        //    return true;
        //}
        bool operator!=(const const_iterator &rhs) const {
            if (*this == rhs) return false;
            return true;
        }

        value_type* operator->() const noexcept {
            if (end) throw invalid_iterator();
            return it->data;
        }
    };

	linked_hashmap() {
        capacity = initialSize;
        total = 0;
        array = new link *[capacity];
        for (int i = 0; i < capacity; ++i) array[i] = nullptr;
        head = rear = nullptr;
    }

	linked_hashmap(const linked_hashmap &other) {
        capacity = other.capacity;
        total = 0;
        array = new link *[capacity];
        for (int i = 0; i < capacity; ++i) array[i] = nullptr;
        head = rear = nullptr;
        if (!other.head) { return; }
        //读双链表，进行插入
        link *tmp = other.head->next;
        while (tmp) {
            insert(*tmp->data);
            tmp = tmp->next;
        }
    }

	linked_hashmap & operator=(const linked_hashmap &other) {
        if (head == other.head) return *this;
        clear();
        capacity = other.capacity;
        total = 0;
        delete [] array;
        array = new link *[capacity];
        for (int i = 0; i < capacity; ++i) array[i] = nullptr;
        if (!other.head) {
            head = rear = nullptr;
            return *this;
        }
        link *tmp = other.head->next;
        while (tmp) {
            insert(*tmp->data);
            tmp = tmp->next;
        }
        return *this;
    }

	~linked_hashmap() {
        clear();
        delete [] array;
    }

	T & at(const Key &key) {
        link *rt;
        if (search(key, rt))
            return rt->data->second;
        else
            throw index_out_of_bound();
    }
	const T & at(const Key &key) const {
        link *rt;
        if (search(key, rt))
            return rt->data->second;
        else
            throw index_out_of_bound();
    }

	T & operator[](const Key &key) {
        link *rt;
        if (search(key, rt)) {
            return rt->data->second;
        } else {
            T zero;
            value_type d(key, zero);
            return insert(d).first.it->data->second;
        }
    }
	const T & operator[](const Key &key) const {
        link *rt;
        if (search(key, rt))
            return rt->data->second;
        else
            throw index_out_of_bound();
    }

	iterator begin() {
        if (!head) return end();
        return iterator(head, head->next);
    }
	const_iterator cbegin() const {
        if (!head) return cend();
        return iterator(head, head->next);
    }

	iterator end() { return iterator(head, rear, true); }
	const_iterator cend() const { return const_iterator(head, rear, true); }

	bool empty() const { return (total == 0); }

    size_t size() const { return total; }

	void clear() {
        if (total == 0) return;
        for (int i = 0; i < capacity; ++i) {
            if (array[i]) {
                delete array[i];
                array[i] = nullptr;
            }
        }
        total = 0;
        link *t = rear;
        while (t) {
            link *tmp = t;
            t = t->front;
            delete tmp;
        }
        head = rear = nullptr;
    }
 
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
        int key = Hash()(value.first) % capacity;
        link *t, *tmp;
        if (array[key]) {
            t = array[key]->nxt;
            tmp = array[key];
            while (t) {
                if (Equal()(t->data->first, value.first))
                    return pair<iterator, bool>(iterator(head, t), false);
                t = t->nxt;
                tmp = tmp->nxt;
            }
        }
        //t = NULL, tmp为该行最后一个
        if (total < capacity * loadFactor) {
            total++;
            if (!array[key]) {
                if (!head) { head = rear = new link; }
                t = new link(value, rear);
                rear->next = t;
                rear = t;
                array[key] = new link;
                array[key]->nxt = t;
                return pair<iterator, bool>(iterator(head, t), true);
            }
            t = new link(value, rear);
            rear->next = t;
            rear = t;
            tmp->nxt = t;
            return pair<iterator, bool>(iterator(head, t), true);
        } else {
            for (int i = 0; i < capacity; ++i) {
                if (array[i]) {
                    delete array[i];
                    array[i] = nullptr;
                }
            }
            delete [] array;
            capacity *= times;
            array = new link *[capacity];
            for (int i = 0; i < capacity; ++i) { array[i] = nullptr; }
            link *it = head->next;
            while (it) {
                if (it->nxt) it->nxt = nullptr;
                Insert(it);
                it = it->next;
            }
            return insert(value);
        }
    }

	void erase(iterator pos) {
        if (pos.head != head || pos.end) throw invalid_iterator();
        total--;
        link *target = pos.it;
        int key = Hash()(target->data->first) % capacity;
        link *t = array[key]->nxt, *tmp = array[key];
        while (!Equal()(t->data->first, target->data->first)) {
            t = t->nxt;
            tmp = tmp->nxt;
        }
        tmp->nxt = t->nxt;
        if (tmp == array[key] && !tmp->nxt) {
            delete array[key];
            array[key] = nullptr;
        }
        target->front->next = target->next;
        if (target != rear)
            target->next->front = target->front;
        else
            rear = target->front;
        delete target;
        if (head == rear) {
            delete head;
            head = rear = nullptr;
        }
    }

	size_t count(const Key &key) const {
        link *t;
        if (search(key, t))
            return 1;
        else
            return 0;
    }

	iterator find(const Key &key) {
        if (total == 0) return end();
        link *rt;
        if (search(key, rt))
            return iterator(head, rt);
        else
            return end();
    }
	const_iterator find(const Key &key) const {
        if (total == 0) return cend();
        link *rt;
        if (search(key, rt))
            return const_iterator(head, rt);
        else
            return cend();
    }

private:

    bool search(const Key &key, link *&t) const {
        int value = Hash()(key) % capacity;
        if (!array[value]) {
            t = nullptr;
            return false;
        }
        link *re = array[value]->nxt;
        while (re) {
            if (Equal()(re->data->first, key)) {
                t = re;
                return true;
            }
            re = re->nxt;
        }
        return false;
    }

    void Insert(link *&it){  //将一个数插入哈希表而不插入链表，保证能够插入
        int key = Hash()(it->data->first) % capacity;
        if (!array[key]) {
            array[key] = new link;
            array[key]->nxt = it;
            return;
        }
        link *t = array[key]->nxt;
        while (t->nxt) { t = t->nxt; }
        t->nxt = it;
    }
};

}

#endif
