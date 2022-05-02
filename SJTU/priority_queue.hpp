#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

/**
 * a container like std::priority_queue which is a heap internal.
 */
    template<typename T, class Compare = std::less<T>>
    class priority_queue {
    private:
        struct Node{
            T value;
            int npl;
            Node *left;
            Node *right;
            Node(T _value, int _npl = 0, Node *_left = nullptr, Node *_right = nullptr):value(_value), npl(_npl), left(_left), right(_right){};
        };
        Node *root;
        int currentSize;

    public:

        priority_queue() {
            currentSize = 0;
            root = nullptr;
        }
        priority_queue(const priority_queue &other) {
            currentSize = other.currentSize;
            if (other.empty()) {
                root = nullptr;
                return;
            }
            root = new Node(other.root->value, other.root->npl);
            copy(root, other.root);
        }

        ~priority_queue() {
            currentSize = 0;
            if (root != nullptr) clear(root);
        }

        priority_queue &operator=(const priority_queue &other) {
            if (this == &other) return *this;
            if (!empty()) clear(root);
            currentSize = other.currentSize;
            root = new Node(other.root->value, other.root->npl);
            copy(root, other.root);
            return *this;
        }
        /**
         * get the top of the queue.
         * @return a reference of the top element.
         * throw container_is_empty if empty() returns true;
         */
        const T & top() const {
            if (empty()) throw container_is_empty();
            return root->value;
        }

        void push(const T &e) {
            currentSize++;
            if (empty()) {
                root = new Node(e, 0);
                return;
            }
            Node *index = new Node(e, 0);
            Merge(root, index);
        }

        void pop() {
            if (empty()) throw container_is_empty();
            currentSize--;
            Node *tmp = root;
            if (root->right == nullptr) {
                root = root->left;
            } else if (root->left == nullptr) {
                root = root->right;
            } else {
                Merge(root->left, root->right);
                root = root->left;
            }
            delete tmp;
        }
        /**
         * return the number of the elements.
         */
        size_t size() const {
            return currentSize;
        }
        /**
         * check if the container has at least an element.
         * @return true if it is empty, false if it has at least an element.
         */
        bool empty() const {
            return (currentSize == 0);
        }
        /**
         * merge two priority_queues with at least O(logn) complexity.
         * clear the other priority_queue.
         */
        void merge(priority_queue &other) {
            if (other.empty()) return;
            currentSize += other.currentSize;
            Merge(root, other.root);
            other.currentSize = 0;
            other.root = nullptr;
        }

        /**
         * add by myself
         */
        void copy(Node* &a, Node *b){
            if (b->left != nullptr) {
                a->left = new Node(b->left->value, b->left->npl);
                copy(a->left, b->left);
            }
            if (b->right != nullptr) {
                a->right = new Node(b->right->value, b->right->npl);
                copy(a->right, b->right);
            }
        }

        void clear(Node* &x) {
            if (x == nullptr) return;
            if (x->left != nullptr) clear(x->left);
            if (x->right != nullptr) clear(x->right);
            delete x;
            x = nullptr;
        }

        void Merge(Node* &a, Node* &b, Compare _compare = Compare()) {
            if (b == nullptr) return;
            if (a == nullptr) {
                a = b;
                b = nullptr;
                return;
            }
            if (_compare(a->value, b->value)) { //a<b
                //swap(a, b)
                Node *tmp = a;
                a = b;
                b = tmp;
            }
            if (a->right == nullptr) {
                a->right = b;
            } else {
                Merge(a->right, b);
            }
            if (a->left == nullptr) {
                a->left = a->right;
                a->right = nullptr;
                return;
            }
            if (a->right != nullptr && a->right->npl > a->left->npl) {
                Node *tmp = a->left;
                a->left = a->right;
                a->right = tmp;
            }
            a->npl = a->right->npl + 1;
        }

    };

}

#endif







