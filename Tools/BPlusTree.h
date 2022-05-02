#ifndef TRAINTICKETSYSTEM_BPLUSTREE_H
#define TRAINTICKETSYSTEM_BPLUSTREE_H


namespace Tools {

    //B+树
    //flag = true ：only key
    //M:数据规模

    template<typename Key, typename Value, int M = 100, typename Compare = std::less<Key>>
    class BPlusTree { };

}
#endif //TRAINTICKETSYSTEM_BPLUSTREE_H
