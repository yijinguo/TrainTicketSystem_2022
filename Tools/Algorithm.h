#ifndef TRAINTICKETSYSTEM_ALGORITHM_H
#define TRAINTICKETSYSTEM_ALGORITHM_H

#include <iostream>

namespace Tools{

    template<class T, typename Compare = std::less<T>>
    int lower_bound(T array[], T index, int num){ //返回第一次出现大于等于那个要查找的数的地址
        int i = 0;
        for (; i < num; ++i) {
            if (Compare()(array[i], index))
                continue;
            else
                break;
        }
        return i;
    }

    template<class T, typename Compare = std::less<T>>
    int upper_bound(T array[], T index, int num) { //返回第一次出现大于那个要查找的数的地址
        //0-base；如果index大于数组中所有数，返回num
        int i = 0;
        for (; i < num; ++i) {
            if (Compare()(index, array[i])) break;
        }
        return i;
    }

    template<class T, typename Compare = std::less<T>>
    int find_index(T array[], T index, int num) {
        //如果不存在返回-1;返回找到的第一个对应数
        bool flag = false;
        int i = 0;
        for (; i < num; ++i) {
            if (Compare()(index, array[i])) {
                break;
            } else if (Compare()(array[i], index)) {
                continue;
            } else {
                flag = true;
                break;
            }
        }
        if (!flag)
            return -1;
        else
            return i;
    }

}


#endif //TRAINTICKETSYSTEM_ALGORITHM_H
