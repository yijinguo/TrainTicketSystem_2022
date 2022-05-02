#ifndef TRAINTICKETSYSTEM_TRAINSYSTEM_H
#define TRAINTICKETSYSTEM_TRAINSYSTEM_H

#include "OrderSystem.h"

namespace backEnd{

class TrainSystem {
private:
    /*一个信息结构体，记录某车辆的全部信息*/
    struct Train{
        Tools::String<22> trainID; //车辆，唯一标识符，简称为tid；<=20
        int stationNum = 0; //<=100
        int seatNum = 0; //<=1e5
        Tools::Time startTime;  //hh:mm
        Tools::Time startDate;  //mm-dd
        Tools::Time endDate;  //mm-dd
        struct stationInfo{
            Tools::String<20> stationName;  //本站站名
            int prefix_price = 0;  //始发站到本站的票价
            int prefix_time = 0;  //始发站到本站经历的时间
            int stopover = 0;  //本站停靠时间
        } stations[100];
        char type; //大写字母
        bool releaseStatus = false;
    };

    /*一个索引结构体，索引为tid与日期双索引（车次），日期优先*/
    struct Seat_key{
        size_t tid; //车辆标识数
        size_t date; //时间标识数
        bool operator==(const Seat_key &b) const {
            return (date == b.date && tid == b.tid);
        }
        bool operator<(const Seat_key &b) const {
            return (date < b.date) || (date == b.date && tid < b.tid);
        }
    };

    /*一个信息结构体，记录某车次的剩余座位个数*/
    struct Seat{
        Seat_key trainNum;
        int seatNum[100];
    };

    /*一个索引结构体，记录站点sid和途径车辆的tid, 站点优先*/
    struct Station_key{
        size_t tid;  //车辆标识数
        size_t sid;  //站点标识数
        bool operator==(const Station_key &b) const {
            return (sid == b.sid && tid == b.tid);
        }
        bool operator<(const Station_key &b) const{
            return (sid < b.sid) || (sid == b.sid && tid < b.tid);
        }
    };

    /*一个信息结构体，记录某站点某车辆的信息*/
    struct Station{
        Station_key stationIndex;
        long long loc; //该车信息在文件中的位置(?随手乱写，实用性存疑)
        int pos; //该站点在该车行程中的下标
    };

    Tools::BPlusTree<size_t, Train> BPT_train;
    Tools::BPlusTree<Seat_key, Seat> BPT_seat;
    Tools::BPlusTree<Station_key, Station> BPT_station;
public:
    TrainSystem() = default;
    ~TrainSystem() = default;
    explicit TrainSystem(const std::string &trainFN, const std::string &seatFN, const std::string &stationFN)
            : BPT_train(trainFN), BPT_seat(seatFN), BPT_station(stationFN) {};
    bool add_train(const std::string &trainID, int stationNum, int seatNum, Tools::String<20> *stations, int *prices,
              Tools::Time &startTime, int *travelTimes, int *stopoverTimes, Tools::Time *saleDate, char type);
    bool release_train(const std::string &trainID);
    bool query_train(const std::string &trainID, const std::string &date);
    bool query_ticket(const std::string &stationName, const std::string &date, std::ostream &os);
    bool query_transfer(const std::string &stationName, const std::string &date);
    bool buy_ticket(const std::string &username, const std::string &trainID, const std::string &date, int ticketNum,
                    const std::string &station1, const std::string station2, bool wait = false);
    bool refund_ticket(Order &orderInfo);
    void clean();
    //回滚函数
    void delete_train(const size_t &tid);
    void close_train(const size_t &tid);
};

}
#endif //TRAINTICKETSYSTEM_TRAINSYSTEM_H
