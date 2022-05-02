#ifndef TRAINTICKETSYSTEM_TIME_H
#define TRAINTICKETSYSTEM_TIME_H

#include <iostream>

namespace Tools{

class Time{
    /*时间有两种形式：mm-dd,hh:mm*/
    enum TimeType{ DATE, MOMENT};
private:
    int month = 6;  //6-8
    int day = 1;  //1-31
    int hour = 0;  //0-24
    int minute = 0;  //0-59
public:
    TimeType type = MOMENT;
    Time() = default;
    ~Time() = default;
    Time(const char *str);
    Time(const std::string &str);
    size_t hash();
    void print(std::ostream &os);
};

}

#endif //TRAINTICKETSYSTEM_TIME_H
