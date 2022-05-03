#ifndef TRAINTICKETSYSTEM_BPLUSTREE_H
#define TRAINTICKETSYSTEM_BPLUSTREE_H

#include <iostream>
#include <fstream>

namespace Tools {

    //B+树
    //flag = true ：only key
    //M:数据规模

    template<typename Key, typename Value, bool sameKey = false, bool ORDER = false, int M = 100, typename Compare = std::less<Key>>
            /*sameKey表示是否允许相同键值，false表示不允许，true表示允许*/
            /*order表示当Key相同时的插入方向，false表示向后插入，true表示向前插入*/
    class BPlusTree {
    private:
        std::string fileName;
        int size = M;
    public:
        BPlusTree() = default;
        explicit BPlusTree(const std::string &file_name):fileName(file_name){}
    };

}
#endif //TRAINTICKETSYSTEM_BPLUSTREE_H
