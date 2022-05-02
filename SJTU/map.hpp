/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP


#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

    struct my_true_type{};
    struct my_false_type{};
    template<typename ITERATOR>
    struct my_type_traits{
        using iterator_assignable = typename ITERATOR::my_value_type;
    };

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    > class map {
    public:

        typedef pair<const Key, T> value_type;

        struct AvlNode {

            value_type *element;
            AvlNode *parent = nullptr;
            AvlNode *left = nullptr;
            AvlNode *right = nullptr;
            int height = 1;

            AvlNode() = default;

            AvlNode(const value_type &d, int _height = 1, AvlNode *_parent = nullptr, AvlNode *lt = nullptr,
                    AvlNode *rt = nullptr) : parent(_parent), left(lt), right(rt), height(_height) {
                element = (value_type *) malloc(sizeof(value_type));
                new(element) value_type(d);
            }

            ~AvlNode(){
                if (element) {
                    (*element).~value_type();
                    free(element);
                    element = nullptr;
                }
            }

        };

        AvlNode *root;
        size_t total;

        class const_iterator;

        class iterator {
            friend class map;
        private:
            friend class const_iterator;

            AvlNode *root;
            AvlNode *it;
            bool end;

        public:

            using difference_type = std::ptrdiff_t;
            using my_value_type = my_true_type;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;

            iterator() {
                root = nullptr;
                it = nullptr;
                end = false;
            }

            iterator(const iterator &other) : root(other.root), it(other.it), end(other.end) {}

            iterator(AvlNode *r, AvlNode *t, bool _end = false) : root(r), it(t), end(_end) {}

            iterator operator++(int) {
                if (root == nullptr || end) throw invalid_iterator();//迭代器为空或者==end()
                iterator tmp = *this;
                AvlNode *old = it;
                if (it->right != nullptr) {
                    it = it->right;
                    while (it->left != nullptr)
                        it = it->left;
                } else {
                    while (it->parent != nullptr && it->parent->right == it) it = it->parent;
                    if (it->parent != nullptr) {
                        it = it->parent;
                    } else {
                        end = true;
                        it = old;
                    }
                }
                return tmp;
            }

            iterator &operator++() {
                if (root == nullptr || end) throw invalid_iterator();//迭代器为空或者==end()
                AvlNode *old = it;
                if (it->right != nullptr) {
                    it = it->right;
                    while (it->left != nullptr)
                        it = it->left;
                } else {
                    while (it->parent != nullptr && it->parent->right == it) it = it->parent;
                    if (it->parent != nullptr) {
                        it = it->parent;
                    } else {
                        end = true;
                        it = old;
                    }
                }
                return *this;
            }

            iterator operator--(int) {
                iterator tmp = *this;
                if (end) {
                    it = root;
                    if (it != nullptr )
                        while (it->right != nullptr) it = it->right;
                    end = false;
                    return tmp;
                }
                if (it == nullptr) throw invalid_iterator();
                if (it->left != nullptr) {
                    it = it->left;
                    while (it->right != nullptr)
                        it = it->right;
                } else {
                    while (it->parent != nullptr && it->parent->left == it) it = it->parent;
                    if (it->parent != nullptr)
                        it = it->parent;
                    else
                        throw invalid_iterator();
                }
                return tmp;
            }

            iterator &operator--() {
                if (end) {
                    it = root;
                    if (it != nullptr )
                        while (it->right != nullptr) it = it->right;
                    end = false;
                    return *this;
                }
                if (it == nullptr) throw invalid_iterator();
                if (it->left != nullptr) {
                    it = it->left;
                    while (it->right != nullptr)
                        it = it->right;
                } else {
                    while (it->parent != nullptr && it->parent->left == it) it = it->parent;
                    if (it->parent != nullptr)
                        it = it->parent;
                    else
                        throw invalid_iterator();
                }
                return *this;
            }

            value_type &operator*() const {
                return *it->element;
            }

            bool operator==(const iterator &rhs) const {
                if (root == nullptr && rhs.root == nullptr) return true;
                if (root != rhs.root) return false;
                if (end && rhs.end) return true;
                if (it != rhs.it) return false;
                if (end != rhs.end) return false;
                return true;
            }

            bool operator==(const const_iterator &rhs) const {
                if (root == nullptr && rhs.root == nullptr) return true;
                if (root != rhs.root) return false;
                if (end && rhs.end) return true;
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

            value_type *operator->() const noexcept{
                if (end) throw invalid_iterator();
                return it->element;
            }
        };

        class const_iterator {
            friend class map;

        private:

            friend class iterator;

            AvlNode *root = nullptr;
            AvlNode *it = nullptr;
            bool end = false;

        public:

            using my_value_type = my_false_type;

            const_iterator() {
                root = nullptr;
                it = nullptr;
                end = false;
            }

            const_iterator(const const_iterator &other) {
                root = other.root;
                it = other.it;
                end = other.end;
            }

            const_iterator(const iterator &other) {
                root = other.root;
                it = other.it;
                end = other.end;
            }

            //add by myself
            const_iterator(AvlNode *_root, AvlNode *_it, bool _end = false) : root(_root), it(_it), end(_end) {}

            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.
            const_iterator operator++(int) {
                if (end) throw invalid_iterator();
                const_iterator tmp = *this;
                AvlNode *old = it;
                if (it->right != nullptr) {
                    it = it->right;
                    while (it->left != nullptr)
                        it = it->left;
                } else {
                    while (it->parent != nullptr && it->parent->right == it) it = it->parent;
                    if (it->parent != nullptr) {
                        it = it->parent;
                    } else {
                        it = old;
                        end = true;
                    }
                }
                return tmp;
            }

            const_iterator &operator++() {
                if (end) throw invalid_iterator();
                AvlNode *old = it;
                if (it->right != nullptr) {
                    it = it->right;
                    while (it->left != nullptr)
                        it = it->left;
                } else {
                    while (it->parent != nullptr && it->parent->right == it) it = it->parent;
                    if (it->parent != nullptr) {
                        it = it->parent;
                    } else {
                        end = true;
                        it = old;
                    }
                }
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator tmp = *this;
                if (end) {
                    it = root;
                    if (it != nullptr)
                        while (it->right != nullptr) it = it->right;
                    end = false;
                    return tmp;
                }
                if (it == nullptr) throw invalid_iterator();
                if (it->left != nullptr) {
                    it = it->left;
                    while (it->right != nullptr)
                        it = it->right;
                } else {
                    while (it->parent != nullptr && it->parent->left == it) it = it->parent;
                    if (it->parent != nullptr)
                        it = it->parent;
                    else
                        throw invalid_iterator();
                }
                return tmp;
            }

            const_iterator &operator--() {
                if (end) {
                    it = root;
                    if (it != nullptr)
                        while (it->right != nullptr) it = it->right;
                    end = false;
                    return *this;
                }
                if (it == nullptr) throw invalid_iterator();
                if (it->left != nullptr) {
                    it = it->left;
                    while (it->right != nullptr)
                        it = it->right;
                } else {
                    while (it->parent != nullptr && it->parent->left == it) it = it->parent;
                    if (it->parent != nullptr)
                        it = it->parent;
                    else
                        throw invalid_iterator();
                }
                return *this;
            }

            value_type operator*() const {
                return *it->element;
            }

            bool operator==(const iterator &rhs) const {
                if (root == nullptr && rhs.root == nullptr) return true;
                if (root != rhs.root) return false;
                if (end && rhs.end) return true;
                if (it != rhs.it) return false;
                if (end != rhs.end) return false;
                return true;
            }

            bool operator==(const const_iterator &rhs) const {
                if (root == nullptr && rhs.root == nullptr) return true;
                if (root != rhs.root) return false;
                if (end && rhs.end) return true;
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

            value_type *operator->() const noexcept {
                if (end) throw invalid_iterator();
                return it->element;
            }
        };

        map() {
            root = nullptr;
            total = 0;
        }

        map(const map &other) {
            copy(root, other.root);
            total = other.total;
        }

        map &operator=(const map &other) {
            if (root == other.root) return *this;
            clear();
            copy(root, other.root);
            total = other.total;
            return *this;
        }

        ~map() {
            total = 0;
            clear(root);
        }

        T &at(const Key &key) {
            AvlNode *re;
            if (search(key, re))
                return re->element->second;
            else
                throw index_out_of_bound();
        }

        const T &at(const Key &key) const {
            AvlNode *re;
            if (search(key, re))
                return re->element->second;
            else
                throw index_out_of_bound();
        }

        T &operator[](const Key &key) {
            AvlNode *re;
            if (search(key, re)) {
                return re->element->second;
            } else {
                AvlNode *t = insert(key, nullptr, root);
                total++;
                return t->element->second;
            }
        }

        const T &operator[](const Key &key) const {
            AvlNode *re;
            if (search(key, re))
                return re->element->second;
            else
                throw index_out_of_bound();
        }

        iterator begin() {
            AvlNode *_it = root;
            if (root != nullptr)
                while (_it->left != nullptr) {_it = _it->left;}
            return iterator(root, _it);
        }

        const_iterator cbegin() const {
            AvlNode *_it = root;
            if (root != nullptr)
                while (_it->left != nullptr) {_it = _it->left;}
            return const_iterator(root, _it);
        }

        iterator end() {
            return iterator(root, nullptr, true);
        }

        const_iterator cend() const {
            return const_iterator(root, nullptr, true);
        }

        bool empty() const {
            return (total == 0);
        }

        size_t size() const { return total; }

        void clear() {
            total = 0;
            clear(root);
            root = nullptr;
        }

        pair<iterator, bool> insert(const value_type &value) {
            AvlNode *re;
            if (search(value.first, re)) {
                return pair<iterator, bool>(iterator(root, re), false);
            } else {
                total++;
                AvlNode *t = insert(value.first, nullptr, root);
                t->element->second = value.second;
                return pair<iterator, bool>(iterator(root, t), true);
            }
        }

        void erase(iterator pos) {
            if (pos.root != root || pos.end) throw invalid_iterator();
            total--;
            remove(pos->first, root);
        }

        size_t count(const Key &key) const {
            AvlNode *re;
            if (search(key, re))
                return 1;
            else
                return 0;
        }

        iterator find(const Key &key) {
            AvlNode *t;
            if (search(key, t))
                return iterator(root, t);
            else
                return iterator(root, nullptr, true);  //end()
        }

        const_iterator find(const Key &key) const {
            AvlNode *t;
            if (search(key, t))
                return const_iterator(root, t);
            else
                return const_iterator(root, nullptr, true);  //cend()
        }

    private:

        //add by myself

        void copy(AvlNode *&a, AvlNode *b, AvlNode *_parent = nullptr) {
            if (b == nullptr) {
                a = nullptr;
                return;
            }
            a = new AvlNode(*b->element, b->height, _parent);
            if (b->left != nullptr)
                copy(a->left, b->left, a);
            if (b->right != nullptr)
                copy(a->right, b->right, a);
        }

        void clear(AvlNode *t) {
            if (t == nullptr) return;
            if (t->left != nullptr)
                clear(t->left);
            AvlNode *tmp = t->right;
            delete t;
            t = nullptr;
            if (tmp != nullptr)
                clear(tmp);
        }

        bool search(const Key &key, AvlNode *&re) const {
            AvlNode *t = root;
            while (t != nullptr && Compare()(t->element->first, key) != Compare()(key, t->element->first)) {
                if (Compare()(key, t->element->first)) //<
                    t = t->left;
                else
                    t = t->right;
            }
            if (t == nullptr) {
                re = nullptr;
                return false;
            } else {
                re = t;
                return true;
            }
        }

        AvlNode* insert(const Key &key, AvlNode *p, AvlNode *&t) {
            AvlNode *result;
            if (t == nullptr) {
                T zero;
                value_type d(key, zero);
                t = new AvlNode(d, 1, p);
                return t;
            }
            if (Compare()(key, t->element->first)) {
                result = insert(key, t, t->left);
                if (Height(t->left) - Height(t->right) == 2) {
                    if (Compare()(key,t->left->element->first))
                        LL(t);
                    else
                        LR(t);
                }
            } else {
                result = insert(key, t, t->right);
                if (Height(t->right) - Height(t->left) == 2) {
                    if (Compare()(key, t->right->element->first))
                        RL(t);
                    else
                        RR(t);
                }
            }
            t->height = max(Height(t->left), Height(t->right)) + 1;
            return result;
        }

        bool remove(const Key &key, AvlNode *&t) {  //true表示不需要调整,false表示需要调整
            if (t == nullptr) return true;
            if (Compare()(key, t->element->first)) {
                if (remove(key, t->left)) return true;
                return adjust(t, 0);
            } else if (Compare()(t->element->first, key)){
                if (remove(key, t->right)) return true;
                return adjust(t, 1);
            } else {
                if (t->left == nullptr || t->right == nullptr) {
                    AvlNode *oldNode = t;
                    t = (t->left == nullptr) ? t->right : t->left;
                    if (t != nullptr && oldNode->parent != nullptr)
                        t->parent = oldNode->parent;
                    delete oldNode;
                    return false;
                } else {
                    AvlNode *tmp = t->right;
                    if (tmp->left == nullptr) {
                        AvlNode *old = t;
                        tmp->left = old->left;
                        old->left->parent = tmp;
                        tmp->parent = old->parent;
                        tmp->height = old->height;
                        t = tmp;
                        delete old;
                        return adjust(t, 1);
                    } else {
                        while (tmp->left != nullptr) tmp = tmp->left;
                        AvlNode *old = t, *oldP = tmp->parent;
                        if (tmp->right != nullptr) {
                            oldP->left = tmp->right;
                            tmp->right->parent = oldP;
                        } else {
                            tmp->parent->left = nullptr;
                        }
                        tmp->left = old->left;
                        tmp->right = old->right;
                        old->left->parent = old->right->parent = tmp;
                        tmp->parent = old->parent;
                        tmp->height = old->height;
                        t = tmp;
                        delete old;
                        while (oldP != t) {
                            if (adjust(oldP, 0))
                                return true;
                            else
                                oldP = oldP->parent;
                        }
                        return adjust(oldP, 1);
                    }
                }
            }
        }

        int Height(AvlNode *t) { return t == nullptr ? 0 : t->height; }

        int max(int a, int b) { return (a > b) ? a : b; }

        void LL(AvlNode *&t) {
            AvlNode *t2 = t;
            AvlNode *t1 = t->left;
            t1->parent = t2->parent;
            if (t2->parent != nullptr) {
                if (t2->parent->left == t2) {
                    t2->parent->left = t1;
                } else {
                    t2->parent->right = t1;
                }
            }
            t2->left = t1->right;
            if (t1->right != nullptr) t1->right->parent = t2;
            t1->right = t2;
            t2->parent = t1;
            t2->height = max(Height(t2->left), Height(t2->right)) + 1;
            t1->height = max(Height(t1->left), Height(t2)) + 1;
            if (root == t) root = t1;
            t = t1;
        }

        void LR(AvlNode *&t) {
            RR(t->left);
            LL(t);
        }

        void RR(AvlNode *&t) {
            AvlNode *t2 = t;
            AvlNode *t1 = t->right;
            t1->parent = t2->parent;
            if (t2->parent != nullptr) {
                if (t2->parent->left == t2) {
                    t2->parent->left = t1;
                } else {
                    t2->parent->right = t1;
                }
            }
            t2->right = t1->left;
            if (t1->left != nullptr) t1->left->parent = t2;
            t1->left = t2;
            t2->parent = t1;
            t2->height = max(Height(t2->left), Height(t2->right)) + 1;
            t1->height = max(Height(t1->left), Height(t2)) + 1;
            if (root == t) root = t1;
            t = t1;
        }

        void RL(AvlNode *&t) {
            LL(t->right);
            RR(t);
        }

        bool adjust(AvlNode *&t, int subTree) {  //调整函数
            //suvTree = 0:left   = 1:right
            if (subTree) {  //在右子树上删除
                if (Height(t->left) - Height(t->right) == 1) return true;
                if (Height(t->right) - Height(t->left) == 1) return true;
                if (Height(t->left) == Height(t->right)) {
                    --t->height;
                    return false;
                }
                if (Height(t->left->right) > Height(t->left->left)) {
                    LR(t);
                    return false;
                }
                LL(t);
                if (Height(t->right) == Height(t->left))
                    return false;
                else
                    return true;
            } else {  //在左子树删除
                if (Height(t->right) - Height(t->left) == 1) return true;
                if (Height(t->left) == Height(t->right)) {
                    --t->height;
                    return false;
                }
                if (Height(t->right->left) > Height(t->right->right)) {
                    RL(t);
                    return false;
                }
                RR(t);
                if (Height(t->right) == Height(t->left))
                    return false;
                else
                    return true;
            }
        }
    };

}

#endif