#ifndef TRAINTICKETSYSTEM_BPLUSTREE_H
#define TRAINTICKETSYSTEM_BPLUSTREE_H

#include <iostream>
#include <fstream>
#include "../Tools/Algorithm.h"
#include "../SJTU/vector.hpp"

namespace Tools {

template<typename Key, typename T, int M = 1000, int L = 1000, bool sameKey = false, bool order = true, typename SecondKey = int, typename Compare = std::less<Key>, typename CompareK = std::less<SecondKey>>
        /*sameKey表示是否允许相同键值，false表示不允许，true表示允许*/
        /*order表示相同键值时T是否有序；通常为true(有第二关键字,此时SecondKey才有意义)*/
class BPlusTree {
private:

    struct Node{
        Key index[M - 1];
        SecondKey index_second[M - 1];
        long long pointer[M] = {-1};
        int num = 0; //有效索引个数
        int type = 1; //0:下一层为Node；1:下一层为dataNode
        Node() = default;
    }; //索引块

    struct DataNode{
        Key index[L];
        SecondKey index_second[L];
        long long Loc[L] = {-1};
        int num = 0; //有效数据个数
        long long next = -1; //链表，下一个数据块的位置, 用于遍历
        DataNode() = default;
    }; //数据块

    Node root; //根结点
    long long rootLoc = -1; //根结点数据的位置
    long long beginning = -1; //最小的数据块所在的位置
    std::string fileName;
    std::string datafileName;
    std::fstream file;
    std::fstream datafile;
    long long total = 0;

    const int sizeT = sizeof(T);
    const int sizeNode = sizeof(Node);
    const int sizeData = sizeof(DataNode);

    //文件头空出位置大小：sizeNode + long long

public:

    BPlusTree(const std::string &FN, const std::string &dataFN){
        fileName = FN;
        datafileName = dataFN;
        file.open(FN, std::fstream::in);
        if (!file) {
            file.open(FN, std::fstream::out);
            datafile.open(dataFN, std::fstream::out);
        } else {
            file.seekg(0);
            file.read(reinterpret_cast<char *>(&total), sizeof(long long));
            file.read(reinterpret_cast<char *>(&beginning), sizeof(long long));
            file.read(reinterpret_cast<char *>(&rootLoc), sizeof(long long));
            file.seekg(rootLoc);
            file.read(reinterpret_cast<char *>(&root), sizeNode);
        }
        file.close();
        datafile.close();
        file.open(fileName);
        datafile.open(datafileName);
    }

    ~BPlusTree(){
        file.seekp(0, std::ios::end);
        rootLoc = file.tellp();
        file.write(reinterpret_cast<char *>(&root), sizeNode);
        file.seekp(0);
        file.write(reinterpret_cast<char *>(&total), sizeof(long long));
        file.write(reinterpret_cast<char *>(&beginning), sizeof(long long));
        file.write(reinterpret_cast<char *>(&rootLoc), sizeof(long long));
        file.close();
        datafile.close();
    }

    bool Insert(Key index, SecondKey indexSecond, T value) {  //sameKey
        total++;
        datafile.seekp(0, std::ios::end);
        long long newLoc = datafile.tellp();
        datafile.write(reinterpret_cast<char *>(&value), sizeT);
        Key newIndex;
        SecondKey newIndexSecond;
        long long newNodeLoc;
        if (total == 1) {
            DataNode dataNode;
            dataNode.index[0] = index;
            dataNode.index_second[0] = indexSecond;
            dataNode.Loc[0] = newLoc;
            dataNode.num = 1;
            file.seekp(0, std::ios::end);
            long long next = file.tellp();
            if (next < 24) next = 24;
            file.seekp(next);
            file.write(reinterpret_cast<char *>(&dataNode), sizeData);
            root.pointer[0] = next;
            root.num = 0;
            beginning = next;
            return true;
        }
        bool exist = false;
        if (!insert(root, rootLoc, index, indexSecond, newLoc, newIndex, newIndexSecond, newNodeLoc, exist)) {
            Node newRoot;
            newRoot.type = 0;
            newRoot.num = 1;
            newRoot.index[0] = newIndex;
            newRoot.index_second[0] = newIndexSecond;
            if (rootLoc == -1) {
                file.seekp(0, std::ios::end);
                rootLoc = file.tellp();
            } else {
                file.seekp(rootLoc);
            }
            file.write(reinterpret_cast<char *>(&root), sizeNode);
            newRoot.pointer[0] = rootLoc;
            newRoot.pointer[1] = newNodeLoc;
            file.seekp(0, std::ios::end);
            rootLoc = file.tellp();
            file.write(reinterpret_cast<char *>(&newRoot), sizeNode);
            root = newRoot;
        }
        if (exist)
            return false;
        else
            return true;
    }

    void Insert(Key index, T value){  // !sameKey
        total++;
        datafile.seekp(0, std::ios::end);
        long long newLoc = datafile.tellp();
        datafile.write(reinterpret_cast<char *>(&value), sizeT);
        if (total == 1) {
            DataNode dataNode;
            dataNode.index[0] = index;
            dataNode.Loc[0] = newLoc;
            dataNode.num = 1;
            file.seekp(0, std::ios::end);
            long long next = file.tellp();
            if (next < 24) next = 24;
            file.seekp(next);
            file.write(reinterpret_cast<char *>(&dataNode), sizeData);
            root.pointer[0] = next;
            beginning = next;
            return;
        }
        Key newIndex;
        long long newNodeLoc;
        if (!insert(root, rootLoc, index, newLoc, newIndex, newNodeLoc)) {  //分裂根结点
            Node newRoot;
            newRoot.type = 0;
            newRoot.num = 1;
            newRoot.index[0] = newIndex;
            newRoot.pointer[0] = rootLoc;
            newRoot.pointer[1] = newNodeLoc;
            file.seekp(0, std::ios::end);
            rootLoc = file.tellp();
            file.write(reinterpret_cast<char *>(&newRoot), sizeNode);
            root = newRoot;
        }
    }

    bool Remove(Key index, SecondKey indexSecond){
        bool back = false, push = false;
        Key newIndex;
        SecondKey newIndexSecond;
        bool flag = remove(root, rootLoc, index, indexSecond, back, push, newIndex, newIndexSecond);
        if (!flag) return false;
        if (root.num == 0 && !root.type) {
            rootLoc = root.pointer[0];
            file.seekg(rootLoc);
            file.read(reinterpret_cast<char *>(&root), sizeNode);
        }
        total--;
        return true;
    }

    bool Remove(Key index){
        bool back = false;
        bool flag = remove(root, rootLoc, index, back);
        if (!flag) return false;
        if (root.num == 0 && !root.type) {
            rootLoc = root.pointer[0];
            file.seekg(rootLoc);
            file.read(reinterpret_cast<char *>(&root), sizeNode);
        }
        return true;
    }

    void find(Key index, int &num, sjtu::vector<T> &value){
        if (!sameKey) {
            T ans;
            if (findKey(root, index, ans)) {
                num = 1;
                value.push_back(ans);
            } else {
                num = 0;
            }
        } else {
            num = 0;
            FindKey(root, index, num, value);
        }
    }

    bool find(Key index, SecondKey indexSecond, T &ans) {
        return findKey(root, index, indexSecond, ans);
    }

    void modify(Key index, SecondKey indexSecond, T value) {

    }

private:

    void splitData(DataNode &dataNode, long long next, int in, Key index, SecondKey indexSecond, long long data, Key &newIndex, SecondKey &newIndexSecond, long long &newNodeLoc) {
        int tmp = L / 2;
        DataNode newDataNode;
        if (in > tmp) { //位于后半段
            if (in == tmp + 1) {
                newIndex = index;
                newIndexSecond = indexSecond;
            } else {
                newIndex = dataNode.index[tmp + 1];
                newIndexSecond = dataNode.index_second[tmp + 1];
            }
            newDataNode.num = 0;
            int i = tmp + 1, k = 0;
            for (; i < dataNode.num; ) {
                if (i == in) {
                    newDataNode.index[k] = index;
                    newDataNode.index_second[k] = indexSecond;
                    newDataNode.Loc[k] = data;
                    k++, newDataNode.num++;
                }
                newDataNode.index[k] = dataNode.index[i];
                newDataNode.index_second[k] = dataNode.index_second[i];
                newDataNode.Loc[k] = dataNode.Loc[i];
                k++, i++, newDataNode.num++;
            }
            if (in == dataNode.num) {
                newDataNode.index[k] = index;
                newDataNode.index_second[k] = indexSecond;
                newDataNode.Loc[k] = data;
                newDataNode.num++;
            }
        } else { //位于前半段
            newIndex = dataNode.index[tmp];
            newIndexSecond = dataNode.index_second[tmp];
            newDataNode.num = 0;
            for (int i = tmp, k = 0; i < dataNode.num; ++i, ++k) {
                newDataNode.index[k] = dataNode.index[i];
                newDataNode.index_second[k] = dataNode.index_second[i];
                newDataNode.Loc[k] = dataNode.Loc[i];
                newDataNode.num++;
            }
            for (int i = tmp; i > in; --i) {
                dataNode.index[i] = dataNode.index[i - 1];
                dataNode.index_second[i] = dataNode.index_second[i - 1];
                dataNode.Loc[i] = dataNode.Loc[i - 1];
            }
            dataNode.index[in] = index;
            dataNode.index_second[in] = indexSecond;
            dataNode.Loc[in] = data;
        }
        dataNode.num = tmp + 1;
        newDataNode.next = dataNode.next;
        file.seekp(0, std::ios::end);
        newNodeLoc = dataNode.next = file.tellp();
        file.write(reinterpret_cast<char *>(&newDataNode), sizeData);
        file.seekp(next);
        file.write(reinterpret_cast<char *>(&dataNode), sizeData);
    }

    bool changeNode(Node &now, long long nowLoc, int nextIndex, Key &newIndex, SecondKey &newIndexSecond, long long &newNodeLoc) {
        if (now.num == M - 1) { //需要裂点
            int tmp = M / 2;
            Node newNode;
            if (nextIndex >= tmp) {  //新点要加入的位置在后半段
                if (nextIndex == tmp) {
                    int i = tmp, k = 0;
                    newNode.pointer[0] = newNodeLoc;
                    for ( ; i < now.num; ++i, ++k) {
                        newNode.index[k] = now.index[i];
                        newNode.index_second[k] = now.index_second[i];
                        newNode.pointer[k + 1] = now.pointer[i + 1];
                    }
                } else {
                    int i = tmp + 1, k = 0;
                    newNode.pointer[0] = now.pointer[tmp + 1];
                    for ( ; i < now.num; ) {
                        if (i == nextIndex) {
                            newNode.index[k] = newIndex;
                            newNode.index_second[k] = newIndexSecond;
                            newNode.pointer[k + 1] = newNodeLoc;
                            ++k;
                        }
                        newNode.index[k] = now.index[i];
                        newNode.index_second[k] = now.index_second[i];
                        newNode.pointer[k + 1] = now.pointer[i + 1];
                        ++k, ++i;
                    }
                    if (i == now.num) {
                        newNode.index[k] = newIndex;
                        newNode.index_second[k] = newIndexSecond;
                        newNode.pointer[k + 1] = newNodeLoc;
                    }
                    newIndex = now.index[tmp];
                    newIndexSecond = now.index_second[tmp];
                }
                now.num = tmp;
                newNode.num = M - tmp - 1;
                newNode.type = now.type;
                //完成新节点newNode的构造
            } else {  //新点要加入的位置在前半段
                Key tmpNewIndex = now.index[tmp - 1];
                SecondKey tmpNewIndexSecond = now.index_second[tmp - 1];

                int i = tmp, k = 0;
                for ( ; i < now.num; ++i, ++k){
                    newNode.index[k] = now.index[i];
                    newNode.index_second[k] = now.index_second[i];
                    newNode.pointer[k] = now.pointer[i];
                }
                newNode.pointer[k] = now.pointer[i];

                for (i = tmp - 1; i > nextIndex; --i) {
                    now.index[i] = now.index[i - 1];
                    now.index_second[i] = now.index_second[i - 1];
                    now.pointer[i + 1] = now.pointer[i];
                }
                now.index[nextIndex] = newIndex;
                now.index_second[nextIndex] = newIndexSecond;
                newIndex = tmpNewIndex, newIndexSecond = tmpNewIndexSecond;

                now.pointer[nextIndex + 1] = newNodeLoc;
                now.num = tmp;
                newNode.num = M - tmp - 1;
                newNode.type = now.type;
            }
            file.seekp(0, std::ios::end);
            newNodeLoc = file.tellp();
            file.write(reinterpret_cast<char *>(&newNode), sizeNode);
            if (nowLoc != -1) {
                file.seekp(nowLoc);
                file.write(reinterpret_cast<char *>(&now), sizeNode);
            }
            return false;
        } else { //不需要裂点
            for (int i = now.num; i >= nextIndex + 1; --i) {
                now.index[i] = now.index[i - 1];
                now.index_second[i] = now.index_second[i - 1];
                now.pointer[i + 1] = now.pointer[i];
            }
            now.index[nextIndex] = newIndex;
            now.index_second[nextIndex] = newIndexSecond;
            now.pointer[nextIndex + 1] = newNodeLoc;
            now.num++;
            if (nowLoc != -1) {
                file.seekp(nowLoc);
                file.write(reinterpret_cast<char *>(&now), sizeNode);
            }
            return true;
        }
    }

    bool insert(Node &now, long long nowLoc, Key index, SecondKey indexSecond, long long data, Key &newIndex, SecondKey &newIndexSecond, long long &newNodeLoc, bool &exist) { //允许相同键值时
        int leftIndex = Tools::lower_bound(now.index, index, now.num);
        int rightIndex = Tools::upper_bound(now.index, index, now.num);
        int nextIndex;
        long long next;
        if (leftIndex == rightIndex) {
            nextIndex = leftIndex;
            next = now.pointer[leftIndex];
        } else {
            for (nextIndex = leftIndex; nextIndex < rightIndex; ++nextIndex) {
                if (CompareK()(indexSecond, now.index_second[nextIndex])) {
                    break;
                } else if (!CompareK()(now.index_second[nextIndex], indexSecond)) {
                    exist = true;
                    return true;
                }
            }
            next = now.pointer[nextIndex];
        }
        file.seekg(next);
        if (!now.type) {  //下一个结点仍为索引结点
            Node nextNode;
            file.read(reinterpret_cast<char *>(&nextNode), sizeNode);
            if (insert(nextNode, next, index, indexSecond, data, newIndex, newIndexSecond, newNodeLoc, exist)) {
                return true;
            } else {
                return changeNode(now, nowLoc, nextIndex, newIndex, newIndexSecond, newNodeLoc);
            }
        } else {  //下一个结点为数据块（叶结点）
            DataNode dataNode;
            file.read(reinterpret_cast<char *>(&dataNode), sizeData);
            if (dataNode.num == L) { //需要裂点
                int left = Tools::lower_bound(dataNode.index, index, dataNode.num);
                int right = Tools::upper_bound(dataNode.index, index, dataNode.num);
                int in = left;
                if (left != right) {
                    for ( ; in < right; ++in) {
                        if (CompareK()(indexSecond, dataNode.index_second[in])) {
                            break;
                        } else if (!CompareK()(dataNode.index_second[in], indexSecond)) {
                            exist = true;
                            return true;
                        }
                    }
                }
                splitData(dataNode, next, in, index, indexSecond, data, newIndex, newIndexSecond, newNodeLoc);
                return changeNode(now, nowLoc, nextIndex, newIndex, newIndexSecond, newNodeLoc);
            } else {  //不需要裂点
                int left = Tools::lower_bound(dataNode.index, index, dataNode.num);
                int right = Tools::upper_bound(dataNode.index, index, dataNode.num);
                int insertIndex = left;
                if (left != right) {
                    for ( ; insertIndex < right; ++insertIndex) {
                        if (CompareK()(indexSecond, dataNode.index_second[insertIndex])) {
                            break;
                        } else if (!CompareK()(dataNode.index_second[insertIndex], indexSecond)) {
                            exist = true;
                            return true;
                        }
                    }
                }
                for (int i = dataNode.num; i > insertIndex; --i) {
                    dataNode.index[i] = dataNode.index[i - 1];
                    dataNode.index_second[i] = dataNode.index_second[i - 1];
                    dataNode.Loc[i] = dataNode.Loc[i - 1];
                }
                dataNode.index[insertIndex] = index;
                dataNode.index_second[insertIndex] = indexSecond;
                dataNode.Loc[insertIndex] = data;
                dataNode.num++;
                file.seekp(next);
                file.write(reinterpret_cast<char *>(&dataNode), sizeData);
                return true;
            }
        }
    }

    bool remove(Node &now, long long nowLoc, Key index, SecondKey indexSecond, bool &back, bool &push, Key &newIndex, SecondKey &newIndexSecond){
        //todo 考虑now为根且只有一个指针
        int leftIndex = Tools::lower_bound(now.index, index, now.num);
        int rightIndex = Tools::upper_bound(now.index, index, now.num);
        int nextIndex = leftIndex;
        if (leftIndex != rightIndex) {
            for ( ; nextIndex < rightIndex; ++nextIndex)
                if (CompareK()(indexSecond, now.index_second[nextIndex])) break;
        }
        long long next = now.pointer[nextIndex];
        file.seekg(next);
        if (!now.type) { //非叶结点
            Node nextNode;
            file.read(reinterpret_cast<char *>(&nextNode), sizeNode);
            if (!remove(nextNode, next, index, indexSecond, back, push, newIndex, newIndexSecond)) return false;
            if (push) {
               if (nextIndex != 0) {
                   now.index[nextIndex - 1] = newIndex;
                   now.index_second[nextIndex - 1] = newIndexSecond;
                   push = false;
               }
            }
            if (!back) {
                if (nowLoc != -1) {
                    file.seekp(nowLoc);
                    file.write(reinterpret_cast<char *>(&now), sizeNode);
                }
                return true;
            } else { //此时nextNode内的索引数少于最低值
                long long left = 0, right = 0;
                Node leftNode, rightNode;
                if (nextIndex != 0) {
                    left = now.pointer[nextIndex - 1];
                    file.seekg(left);
                    file.read(reinterpret_cast<char *>(&leftNode), sizeNode);
                    if (leftNode.num > (M - 1) / 2) {
                        for (int i = nextNode.num; i > 0; --i) {
                            nextNode.index[i] = nextNode.index[i - 1];
                            nextNode.index_second[i] = nextNode.index_second[i - 1];
                            nextNode.pointer[i + 1] = nextNode.pointer[i];
                        }
                        nextNode.pointer[1] = nextNode.pointer[0];
                        nextNode.pointer[0] = leftNode.pointer[leftNode.num];
                        nextNode.index[0] = now.index[nextIndex - 1];
                        nextNode.index_second[0] = now.index_second[nextIndex - 1];
                        now.index[nextIndex - 1] = leftNode.index[leftNode.num - 1];
                        now.index_second[nextIndex - 1] = leftNode.index_second[leftNode.num - 1];
                        leftNode.num--,  nextNode.num++;
                        file.seekp(left);
                        file.write(reinterpret_cast<char *>(&leftNode), sizeNode);
                        file.seekp(next);
                        file.write(reinterpret_cast<char *>(&nextNode), sizeNode);
                        if (nowLoc != -1) {
                            file.seekp(nowLoc);
                            file.write(reinterpret_cast<char *>(&now), sizeNode);
                        }
                        back = false;
                        return true;
                    }
                }
                if (nextIndex != now.num) {
                    right = now.pointer[nextIndex + 1];
                    file.seekg(right);
                    file.read(reinterpret_cast<char *>(&rightNode), sizeNode);
                    if (rightNode.num > (M - 1) / 2) {
                        nextNode.num++;
                        nextNode.index[nextNode.num - 1] = now.index[nextIndex];
                        nextNode.index_second[nextNode.num - 1] = now.index_second[nextIndex];
                        nextNode.pointer[nextNode.num] = rightNode.pointer[0];
                        now.index[nextIndex] = rightNode.index[0];
                        now.index_second[nextIndex] = rightNode.index_second[0];
                        file.seekp(next);
                        file.write(reinterpret_cast<char *>(&nextNode), sizeNode);
                        rightNode.num--;
                        for (int i = 0; i < rightNode.num; ++i) {
                            rightNode.index[i] = rightNode.index[i + 1];
                            rightNode.index_second[i] = rightNode.index_second[i + 1];
                            rightNode.pointer[i] = rightNode.pointer[i + 1];
                        }
                        rightNode.pointer[rightNode.num] = rightNode.pointer[rightNode.num + 1];
                        file.seekp(right);
                        file.write(reinterpret_cast<char *>(&rightNode), sizeNode);
                        if (nowLoc != -1) {
                            file.seekp(nowLoc);
                            file.write(reinterpret_cast<char *>(&now), sizeNode);
                        }
                        back = false;
                        return true;
                    }
                }
                if (nextIndex != 0) {  //合左侧
                    leftNode.index[leftNode.num] = now.index[nextIndex - 1];
                    leftNode.index_second[leftNode.num] = now.index_second[nextIndex - 1];
                    leftNode.num++;
                    for (int i = leftNode.num, k = 0; k < nextNode.num; i++, k++) {
                        leftNode.index[i] = nextNode.index[k];
                        leftNode.index_second[i] = nextNode.index_second[k];
                        leftNode.pointer[i] = nextNode.pointer[k];
                        leftNode.num++;
                    }
                    leftNode.pointer[leftNode.num] = nextNode.pointer[nextNode.num];
                    file.seekp(left);
                    file.write(reinterpret_cast<char *>(&leftNode), sizeNode);
                    now.num--;
                    for (int i = nextIndex - 1; i < now.num; ++i) {
                        now.index[i] = now.index[i + 1];
                        now.index_second[i] = now.index_second[i + 1];
                        now.pointer[i + 1] = now.pointer[i + 2];
                    }
                } else {  //合右侧
                    nextNode.index[nextNode.num] = now.index[nextIndex];
                    nextNode.index_second[nextNode.num] = now.index_second[nextIndex];
                    nextNode.num++;
                    for (int i = nextNode.num, k = 0; k < rightNode.num; ++i, ++k) {
                        nextNode.index[i] = rightNode.index[k];
                        nextNode.index_second[i] = rightNode.index_second[k];
                        nextNode.pointer[i] = rightNode.pointer[k];
                        nextNode.num++;
                    }
                    nextNode.pointer[nextNode.num] = rightNode.pointer[rightNode.num];
                    file.seekp(next);
                    file.write(reinterpret_cast<char *>(&nextNode), sizeNode);
                    now.num--;
                    for (int i = nextIndex; i < now.num; ++i) {
                        now.index[i] = now.index[i + 1];
                        now.index_second[i] = now.index_second[i + 1];
                        now.pointer[i + 1] = now.pointer[i + 2];
                    }
                }
                if (now.num < (M - 1) / 2) {
                    back = true;
                } else {
                    if (nowLoc != -1) {
                        file.seekp(nowLoc);
                        file.write(reinterpret_cast<char *>(&now), sizeNode);
                    }
                    back = false;
                }
                return true;
            }
        } else {  //叶结点
            DataNode dataNode;
            file.read(reinterpret_cast<char *>(&dataNode), sizeData);
            int in = Tools::find_index(dataNode.index, index, dataNode.num);
            if (in == -1) return false;
            int rightIn = Tools::upper_bound(dataNode.index, index, dataNode.num);
            for (; in < rightIn; ++in) {
                if (!CompareK()(dataNode.index_second[in], indexSecond) && !CompareK()(indexSecond, dataNode.index_second[in]))
                    break;
            }
            if (in == rightIn) return false;
            dataNode.num--;
            for (int i = in; i < dataNode.num; ++i) {
                dataNode.index[i] = dataNode.index[i + 1];
                dataNode.index_second[i] = dataNode.index_second[i + 1];
                dataNode.Loc[i] = dataNode.Loc[i + 1];
            }
            if (nowLoc == rootLoc && now.num == 0) {
                file.seekp(next);
                file.write(reinterpret_cast<char *>(&dataNode), sizeData);
                return true;
            }
            if (in == 0 && nextIndex == 0) push = true;
            if (dataNode.num >= L / 2) {
                if (in == 0) {
                    if (nextIndex == 0) {
                        newIndex = dataNode.index[0];
                        newIndexSecond = dataNode.index_second[0];
                    } else {
                        now.index[nextIndex - 1] = dataNode.index[0];
                        now.index_second[nextIndex - 1] = dataNode.index_second[0];
                    }
                }
                file.seekp(next);
                file.write(reinterpret_cast<char *>(&dataNode), sizeData);
                back = false;
                return true;
            }
            //需要寻求领养或合并
            long long left = 0, right = 0;
            DataNode leftNode, rightNode;
            if (nextIndex != 0) {  //向左查看
                left = now.pointer[nextIndex - 1];
                file.seekg(left);
                file.read(reinterpret_cast<char *>(&leftNode), sizeData);
                if (leftNode.num > L / 2) {  //向左借末尾
                    for (int i = dataNode.num; i > 0; --i) {
                        dataNode.index[i] = dataNode.index[i - 1];
                        dataNode.index_second[i] = dataNode.index_second[i - 1];
                        dataNode.Loc[i] = dataNode.Loc[i - 1];
                    }
                    dataNode.num++, leftNode.num--;
                    now.index[nextIndex - 1] = dataNode.index[0] = leftNode.index[leftNode.num];
                    now.index_second[nextIndex - 1] = dataNode.index_second[0] = leftNode.index_second[leftNode.num];
                    dataNode.Loc[0] = leftNode.Loc[leftNode.num];
                    file.seekp(left);
                    file.write(reinterpret_cast<char *>(&leftNode), sizeData);
                    file.seekp(next);
                    file.write(reinterpret_cast<char *>(&dataNode), sizeData);
                    if (nowLoc != -1) {
                        file.seekp(nowLoc);
                        file.write(reinterpret_cast<char *>(&now), sizeNode);
                    }
                    back = false;
                    return true;
                }
            }
            if (nextIndex != now.num) { //向右查看
                right = now.pointer[nextIndex + 1];
                file.seekg(right);
                file.read(reinterpret_cast<char *>(&rightNode), sizeData);
                if (rightNode.num > L / 2) {
                    dataNode.index[dataNode.num] = rightNode.index[0];
                    dataNode.index_second[dataNode.num] = rightNode.index_second[0];
                    dataNode.Loc[dataNode.num] = rightNode.Loc[0];
                    now.index[nextIndex] = rightNode.index[1];
                    now.index_second[nextIndex] = rightNode.index_second[1];
                    dataNode.num++, rightNode.num--;
                    for (int i = 0; i < rightNode.num; ++i) {
                        rightNode.index[i] = rightNode.index[i + 1];
                        rightNode.index_second[i] = rightNode.index_second[i + 1];
                        rightNode.Loc[i] = rightNode.Loc[i + 1];
                    }
                    file.seekp(right);
                    file.write(reinterpret_cast<char *>(&rightNode), sizeData);
                    file.seekp(next);
                    file.write(reinterpret_cast<char *>(&dataNode), sizeData);
                    back = false;
                    if (in == 0) {
                        if (nextIndex == 0) {
                            newIndex = dataNode.index[0];
                            newIndexSecond = dataNode.index_second[0];
                        } else {
                            now.index[nextIndex - 1] = dataNode.index[0];
                            now.index_second[nextIndex - 1] = dataNode.index_second[0];
                        }
                    }
                    if (nowLoc != -1) {
                        file.seekp(nowLoc);
                        file.write(reinterpret_cast<char *>(&now), sizeNode);
                    }
                    return true;
                }
            }
            if (nextIndex != now.num) {  //合右侧
                for (int i = dataNode.num, k = 0; k < rightNode.num; ) {
                    dataNode.index[i] = rightNode.index[k];
                    dataNode.index_second[i] = rightNode.index_second[k];
                    dataNode.Loc[i] = rightNode.Loc[k];
                    i++, k++, dataNode.num++;
                }
                dataNode.next = rightNode.next;
                file.seekp(next);
                file.write(reinterpret_cast<char *>(&dataNode), sizeData);
                now.num--;
                for (int i = nextIndex; i < now.num; ++i) {
                    now.index[i] = now.index[i + 1];
                    now.index_second[i] = now.index_second[i + 1];
                    now.pointer[i + 1] = now.pointer[i + 2];
                }
                if (in == 0) {
                    if (nextIndex == 0) {
                        newIndex = dataNode.index[0];
                        newIndexSecond = dataNode.index_second[0];
                    } else {
                        now.index[nextIndex - 1] = dataNode.index[0];
                        now.index_second[nextIndex - 1] = dataNode.index_second[0];
                    }
                }
            } else if (nextIndex != 0) {  //now.num；合左侧
                for (int i = leftNode.num, k = 0; k < dataNode.num; ) {
                    leftNode.index[i] = dataNode.index[k];
                    leftNode.index_second[i] = dataNode.index_second[k];
                    leftNode.Loc[i] = dataNode.Loc[k];
                    i++, k++, leftNode.num++;
                }
                leftNode.next = dataNode.next;
                file.seekp(left);
                file.write(reinterpret_cast<char *>(&leftNode), sizeData);
                now.num--;
            }
            if (now.num < M / 2) {
                back = true;
            } else {
                if (nowLoc != -1) {
                    file.seekp(nowLoc);
                    file.write(reinterpret_cast<char *>(&now), sizeNode);
                }
                back = false;
            }
            return true;
        }
    }

    bool FindKey(Node now, Key index, int &num, sjtu::vector<T> &value){  //sameKey
        int leftIndex = Tools::lower_bound(now.index, index, now.num);
        int rightIndex = Tools::upper_bound(now.index, index, now.num);
        if (leftIndex == rightIndex) {
            long long next = now.pointer[leftIndex];
            file.seekg(next);
            if (!now.type) {
                Node nextNode;
                file.read(reinterpret_cast<char *>(&nextNode), sizeNode);
                return FindKey(nextNode, index, num, value);
            } else {
                DataNode dataNode;
                file.read(reinterpret_cast<char *>(&dataNode), sizeData);
                int ansLeft = Tools::find_index(dataNode.index, index, dataNode.num);
                if (ansLeft == -1) return false;
                for (int i = ansLeft; i < dataNode.num; ++i) {
                    if (Compare()(index, dataNode.index[i])) break;
                    T ans;
                    long long ansLoc = dataNode.Loc[i];
                    datafile.seekg(ansLoc);
                    datafile.read(reinterpret_cast<char *>(&ans), sizeT);
                    num++;
                    value.push_back(ans);
                }
                return true;
            }
        } else {
            if (!now.type) {
                Node nextNode;
                for (int i = leftIndex; i <= rightIndex; ++i) {
                    long long next = now.pointer[i];
                    file.seekg(next);
                    file.read(reinterpret_cast<char *>(&nextNode), sizeNode);
                    FindKey(nextNode, index, num, value);
                }
                return true;
            } else {
                DataNode dataNode;
                for (int i = leftIndex; i <= rightIndex; ++i) {
                    long long next = now.pointer[i];
                    file.seekg(next);
                    file.read(reinterpret_cast<char *>(&dataNode), sizeData);
                    int begin = find_index(dataNode.index, index, dataNode.num);
                    if (begin == -1) continue;
                    for (int j = begin; j < dataNode.num; ++j) {
                        if (Compare()(index, dataNode.index[j])) break;
                        T ans;
                        long long ansLoc = dataNode.Loc[j];
                        datafile.seekg(ansLoc);
                        datafile.read(reinterpret_cast<char *>(&ans), sizeT);
                        num++;
                        value.push_back(ans);
                    }
                }
                return true;
            }
        }
    }

    //

    bool insert(Node &now, long long nowLoc, Key index, long long data, Key &newIndex, long long &newNodeLoc) { //不允许相同键值时
        int nextIndex = Tools::upper_bound(now.index, index, now.num);
        long long next = now.pointer[nextIndex];
        file.seekg(next);
        if (!now.type) {  //下一个结点仍为索引结点
            Node nextNode;
            file.read(reinterpret_cast<char *>(&nextNode), sizeNode);
            if (insert(nextNode, next, index, data, newIndex, newNodeLoc)) {
                return true;
            } else {
                if (now.num == M - 1) { //需要裂点
                    int tmp = M / 2;
                    Node newNode;
                    if (nextIndex >= tmp) {  //新点要加入的位置在后半段
                        if (nextIndex > tmp)
                            newIndex = now.index[tmp];
                        else
                            newIndex = index;
                        int i = tmp, k = 0;
                        for ( ; i < now.num; ) {
                            if (i == nextIndex) {
                                newNode.pointer[k] = newNodeLoc;
                                newNode.index[k] = newIndex;
                                k++;
                            }
                            newNode.index[k] = now.index[i];
                            newNode.pointer[k] = now.pointer[i];
                            k++, i++;
                        }
                        newNode.pointer[k] = now.pointer[i];
                        now.num = tmp;
                        newNode.num = M - tmp - 1;
                        newNode.type = now.type;  //完成新节点newNode的构造
                    } else {  //新点要加入的位置在前半段
                        newIndex = now.index[tmp - 1];
                        for (int i = tmp - 1; i > nextIndex; --i) {
                            now.index[i] = now.index[i - 1];
                            now.pointer[i + 1] = now.pointer[i];
                        }
                        now.index[nextIndex] = newIndex;
                        now.pointer[nextIndex + 1] = newNodeLoc;
                        int i = tmp, k = 0;
                        for ( ; i < now.num; ++i, ++k){
                            newNode.index[k] = now.index[i];
                            newNode.pointer[k] = now.pointer[i];
                        }
                        newNode.pointer[k] = now.pointer[i];
                        now.num = tmp;
                        newNode.num = M - tmp - 1;
                        newNode.type = now.type;
                    }
                    file.seekp(0, std::ios::end);
                    newNodeLoc = file.tellp();
                    file.write(reinterpret_cast<char *>(&newNode), sizeNode);
                    if (nowLoc != -1) {
                        file.seekp(nowLoc);
                        file.write(reinterpret_cast<char *>(&now), sizeNode);
                    }
                    return false;
                } else { //不需要裂点
                    for (int i = now.num; i >= nextIndex + 1; --i) {
                        now.index[i] = now.index[i - 1];
                        now.pointer[i + 1] = now.pointer[i];
                    }
                    now.index[nextIndex] = newIndex;
                    now.pointer[nextIndex + 1] = newNodeLoc;
                    now.num++;
                    if (nowLoc != -1) {
                        file.seekp(nowLoc);
                        file.write(reinterpret_cast<char *>(&now), sizeNode);
                    }
                    return true;
                }
            }
        } else {  //下一个结点为数据块（叶结点）
            DataNode dataNode;
            file.read(reinterpret_cast<char *>(&dataNode), sizeData);
            if (dataNode.num == L) { //需要裂点
                int in = Tools::upper_bound(dataNode.index, index, dataNode.num);
                int tmp = L / 2;
                DataNode newDataNode;
                if (in > tmp) { //位于后半段
                    if (in == tmp + 1)
                        newIndex = index;
                    else
                        newIndex = dataNode.index[tmp + 1];
                    newDataNode.num = 0;
                    for (int i = tmp + 1, k = 0; i < dataNode.num; ) {
                        if (i == in) {
                            newDataNode.index[k] = index;
                            newDataNode.Loc[k] = data;
                            k++, newDataNode.num++;
                        }
                        newDataNode.index[k] = dataNode.index[i];
                        newDataNode.Loc[k] = dataNode.Loc[i];
                        k++, i++, newDataNode.num++;
                    }
                } else { //位于前半段
                    newIndex = dataNode.index[tmp];
                    newDataNode.num = 0;
                    for (int i = tmp, k = 0; i < dataNode.num; ++i, ++k) {
                        newDataNode.index[k] = dataNode.index[i];
                        newDataNode.Loc[k] = dataNode.Loc[i];
                        newDataNode.num++;
                    }
                    for (int i = tmp; i > in; --i) {
                        dataNode.index[i] = dataNode.index[i - 1];
                        dataNode.Loc[i] = dataNode.Loc[i - 1];
                    }
                    dataNode.index[in] = index;
                    dataNode.Loc[in] = data;
                }
                dataNode.num = tmp + 1;
                newDataNode.next = dataNode.next;
                file.seekp(0, std::ios::end);
                newNodeLoc = dataNode.next = file.tellp();
                file.write(reinterpret_cast<char *>(&newDataNode), sizeData);
                dataNode.next = newNodeLoc;
                file.seekp(next);
                file.write(reinterpret_cast<char *>(&dataNode), sizeData);
                return false;
            } else {  //不需要裂点
                int insertIndex = Tools::upper_bound(dataNode.index, index, dataNode.num);
                for (int i = dataNode.num; i > insertIndex; --i) {
                    dataNode.index[i] = dataNode.index[i - 1];
                    dataNode.Loc[i] = dataNode.Loc[i - 1];
                }
                dataNode.index[insertIndex] = index;
                dataNode.Loc[insertIndex] = data;
                dataNode.num++;
                file.seekp(next);
                file.write(reinterpret_cast<char *>(&dataNode), sizeData);
                return true;
            }
        }
    }

    bool remove(Node &now, long long nowLoc, Key index, bool &back) {
        //需要寻求领养或者合并时返回false
        int nextIndex = Tools::upper_bound(now.index, index, now.num);
        long long next = now.pointer[nextIndex];
        file.seekg(next);
        if (!now.type) { //非叶结点
            Node nextNode;
            file.read(reinterpret_cast<char *>(&nextNode), sizeNode);
            if (!remove(nextNode, next, index, back)) return false;
            if (!back) {
                return true;
            } else { //此时nextNode内的索引数少于最低值
                long long left = 0, right = 0;
                Node leftNode, rightNode;
                if (nextIndex != 0) {
                    left = now.pointer[nextIndex - 1];
                    file.seekg(left);
                    file.read(reinterpret_cast<char *>(&leftNode), sizeNode);
                    if (leftNode.num > M / 2) {
                        long long change = leftNode.pointer[leftNode.num];
                        Key tmp = now.index[nextIndex - 1];
                        now.index[nextIndex - 1] = leftNode.index[leftNode.num - 1];
                        leftNode.num--;
                        file.seekp(left);
                        file.write(reinterpret_cast<char *>(&leftNode), sizeNode);
                        for (int i = nextNode.num; i > 0; ++i) {
                            nextNode.index[i] = nextNode.index[i - 1];
                            nextNode.pointer[i + 1] = nextNode.pointer[i];
                        }
                        nextNode.index[0] = tmp;
                        nextNode.pointer[1] = nextNode.pointer[0];
                        nextNode.pointer[0] = change;
                        nextNode.num++;
                        file.seekp(next);
                        file.write(reinterpret_cast<char *>(&nextNode), sizeNode);
                        if (nowLoc != -1) {
                            file.seekp(nowLoc);
                            file.write(reinterpret_cast<char *>(&now), sizeNode);
                        }
                        back = true;
                        return true;
                    }
                }
                if (nextIndex != now.num) {
                    right = now.pointer[nextIndex + 1];
                    file.seekg(right);
                    file.read(reinterpret_cast<char *>(&rightNode), sizeNode);
                    if (rightNode.num > M / 2) {
                        long long change = rightNode.pointer[0];
                        Key tmp = now.index[nextIndex];
                        now.index[nextIndex] = rightNode.index[0];
                        nextNode.num++;
                        nextNode.index[nextNode.num - 1] = tmp;
                        nextNode.pointer[nextNode.num] = change;
                        file.seekp(next);
                        file.write(reinterpret_cast<char *>(&nextNode), sizeNode);
                        rightNode.num--;
                        for (int i = 0; i < rightNode.num; ++i) {
                            rightNode.index[i] = rightNode.index[i + 1];
                            rightNode.pointer[i] = rightNode.pointer[i + 1];
                        }
                        rightNode.pointer[rightNode.num] = rightNode.pointer[rightNode.num + 1];
                        file.seekp(right);
                        file.write(reinterpret_cast<char *>(&rightNode), sizeNode);
                        if (nowLoc != -1) {
                            file.seekp(nowLoc);
                            file.write(reinterpret_cast<char *>(&now), sizeNode);
                        }
                        back = true;
                        return true;
                    }
                }
                if (nextIndex == now.num) {  //合左侧
                    leftNode.index[leftNode.num] = now.index[now.num - 1];
                    leftNode.num++;
                    for (int i = leftNode.num, k = 0; k < nextNode.num;) {
                        leftNode.index[i] = nextNode.index[k];
                        leftNode.pointer[i] = nextNode.pointer[k];
                        i++, k++, leftNode.num++;
                    }
                    leftNode.pointer[leftNode.num] = nextNode.pointer[nextNode.num];
                    file.seekp(left);
                    file.write(reinterpret_cast<char *>(&leftNode), sizeNode);
                    now.num--;
                } else {  //合右侧
                    nextNode.index[nextNode.num] = now.index[nextIndex];
                    nextNode.num++;
                    for (int i = nextNode.num, k = 0; k < rightNode.num;) {
                        nextNode.index[i] = rightNode.index[k];
                        nextNode.pointer[i] = rightNode.pointer[k];
                        i++, k++, nextNode.num++;
                    }
                    nextNode.pointer[nextNode.num] = rightNode.pointer[rightNode.num];
                    file.seekp(next);
                    file.write(reinterpret_cast<char *>(&nextNode), sizeNode);
                    now.num--;
                    for (int i = nextIndex; i < now.num; ++i) {
                        now.index[i] = now.index[i + 1];
                        now.pointer[i + 1] = now.pointer[i + 2];
                    }
                }
                if (now.num < M / 2) {
                    back = true;
                } else {
                    if (nowLoc != -1) {
                        file.seekp(nowLoc);
                        file.write(reinterpret_cast<char *>(&now), sizeNode);
                    }
                    back = false;
                }
                return true;
            }
        } else {  //叶结点
            DataNode dataNode;
            file.read(reinterpret_cast<char *>(&dataNode), sizeData);
            int in = Tools::find_index(dataNode.index, index, dataNode.num);
            if (in == -1) return false;
            dataNode.num--;
            for (int i = in; i < dataNode.num; ++i) {
                dataNode.index[i] = dataNode.index[i + 1];
                dataNode.Loc[i] = dataNode.Loc[i + 1];
            }
            if (dataNode.num > L / 2) {
                file.seekp(next);
                file.write(reinterpret_cast<char *>(&dataNode), sizeData);
                back = false;
                return true;
            }
            //需要寻求领养或合并
            long long left = 0, right = 0;
            DataNode leftNode, rightNode;
            if (in != 0) {
                left = now.pointer[in - 1];
                file.seekg(left);
                file.read(reinterpret_cast<char *>(&leftNode), sizeData);
                if (leftNode.num > L / 2) {
                    for (int i = dataNode.num; i > 0; --i) {
                        dataNode.index[i] = dataNode.index[i - 1];
                        dataNode.Loc[i] = dataNode.Loc[i - 1];
                    }
                    dataNode.num++, leftNode.num--;
                    now.index[nextIndex - 1] = dataNode.index[0] = leftNode.index[leftNode.num];
                    dataNode.Loc[0] = leftNode.Loc[leftNode.num];
                    file.seekp(left);
                    file.write(reinterpret_cast<char *>(&leftNode), sizeData);
                    file.seekp(next);
                    file.write(reinterpret_cast<char *>(&dataNode), sizeData);
                    if (nowLoc != -1) {
                        file.seekp(nowLoc);
                        file.write(reinterpret_cast<char *>(&now), sizeNode);
                    }
                    back = false;
                    return true;
                }
            }
            if (in != dataNode.num) {
                right = now.pointer[in + 1];
                file.seekg(right);
                file.read(reinterpret_cast<char *>(&rightNode), sizeData);
                if (rightNode.num > L / 2) {
                    now.index[nextIndex] = rightNode.index[1];
                    dataNode.index[dataNode.num] = rightNode.index[0];
                    dataNode.Loc[dataNode.num] = rightNode.Loc[0];
                    for (int i = 0; i < rightNode.num - 1; ++i) {
                        rightNode.index[i] = rightNode.index[i + 1];
                        rightNode.Loc[i] = rightNode.Loc[i + 1];
                    }
                    dataNode.num++, rightNode.num--;
                    file.seekp(right);
                    file.write(reinterpret_cast<char *>(&rightNode), sizeData);
                    file.seekp(next);
                    file.write(reinterpret_cast<char *>(&dataNode), sizeData);
                    if (nowLoc != -1) {
                        file.seekp(nowLoc);
                        file.write(reinterpret_cast<char *>(&now), sizeNode);
                    }
                    back = false;
                    return true;
                }
            }
            if (nextIndex != now.num) {  //合右侧
                for (int i = dataNode.num, k = 0; k < rightNode.num; ) {
                    dataNode.index[i] = rightNode.index[k];
                    dataNode.Loc[i] = rightNode.Loc[k];
                    i++, k++, dataNode.num++;
                }
                dataNode.next = rightNode.next;
                file.seekp(next);
                file.write(reinterpret_cast<char *>(&dataNode), sizeData);
                for (int i = nextIndex; i < now.num - 1; ++i) {
                    now.index[i] = now.index[i + 1];
                    now.pointer[i + 1] = now.pointer[i + 2];
                }
                now.num--;
            } else {
                for (int i = leftNode.num, k = 0; k < dataNode.num; ) {
                    leftNode.index[i] = dataNode.index[k];
                    leftNode.Loc[i] = dataNode.Loc[k];
                    i++, k++, leftNode.num++;
                }
                leftNode.next = dataNode.next;
                file.seekp(left);
                file.write(reinterpret_cast<char *>(&leftNode), sizeData);
                now.num--;
            }
            if (now.num < M / 2) {
                back = true;
            } else {
                if (nowLoc != -1) {
                    file.seekp(nowLoc);
                    file.write(reinterpret_cast<char *>(&now), sizeNode);
                }
                back = false;
            }
            return true;
        }
    }

    bool findKey(Node now, Key index, T &ans) {  // !sameKey
        int nextIndex = Tools::upper_bound(now.index, index, now.num);
        long long next = now.pointer[nextIndex];
        file.seekg(next);
        if (!now.type) {
            Node nextNode;
            file.read(reinterpret_cast<char *>(&nextNode), sizeNode);
            if (findKey(nextNode, index, ans))
                return true;
            else
                return false;
        } else {
            DataNode dataNode;
            file.read(reinterpret_cast<char *>(&dataNode), sizeData);
            int target = Tools::find_index(dataNode.index, index, dataNode.num);
            if (target == -1) {
                return false;
            } else {
                long long Loc = dataNode.Loc[target];
                datafile.seekg(Loc);
                datafile.read(reinterpret_cast<char *>(&ans), sizeT);
                return true;
            }
        }
    }

    bool findKey(Node now, Key index, SecondKey indexSecond, T &ans) {
        int leftIndex = Tools::lower_bound(now.index, index, now.num);
        int rightIndex = Tools::upper_bound(now.index, index, now.num);
        if (leftIndex == rightIndex) {
            long long next = now.pointer[leftIndex];
            file.seekg(next);
            if (!now.type) {
                Node nextNode;
                file.read(reinterpret_cast<char *>(&nextNode), sizeNode);
                return findKey(nextNode, index, indexSecond, ans);
            } else {
                DataNode dataNode;
                file.read(reinterpret_cast<char *>(&dataNode), sizeData);
                int ansLeft = Tools::find_index(dataNode.index, index, dataNode.num);
                if (ansLeft == -1) return false;
                for (int i = ansLeft; i < dataNode.num; ++i) {
                    if (Compare()(index, dataNode.index[i])) return false;
                    if (!CompareK()(indexSecond, dataNode.index_second[i]) && !CompareK()(dataNode.index_second[i], indexSecond)) {
                        long long ansLoc = dataNode.Loc[i];
                        datafile.seekg(ansLoc);
                        datafile.read(reinterpret_cast<char *>(&ans), sizeT);
                        return true;
                    }
                }
                return false;
            }
        } else {
            if (!now.type) {
                Node nextNode;
                for (int i = leftIndex + 1; i <= rightIndex; ++i) {
                    long long next = now.pointer[i];
                    file.seekg(next);
                    file.read(reinterpret_cast<char *>(&nextNode), sizeNode);
                    return findKey(nextNode, index, indexSecond, ans);
                }
            } else {
                DataNode dataNode;
                for (int i = leftIndex + 1; i <= rightIndex; ++i) {
                    long long next = now.pointer[i];
                    file.seekg(next);
                    file.read(reinterpret_cast<char *>(&dataNode), sizeData);
                    int begin = find_index(dataNode.index, index, dataNode.num);
                    for (int j = begin; j < dataNode.num; ++j) {
                        if (Compare()(index, dataNode.index[i])) return false;
                        if (!CompareK()(indexSecond, dataNode.index_second[i]) && !CompareK()(dataNode.index_second[i], indexSecond)) {
                            long long ansLoc = dataNode.Loc[j];
                            datafile.seekg(ansLoc);
                            datafile.read(reinterpret_cast<char *>(&ans), sizeT);
                            return true;
                        }
                    }
                }
                return false;
            }
        }
    }

};

}
#endif //TRAINTICKETSYSTEM_BPLUSTREE_H
