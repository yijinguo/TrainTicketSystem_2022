#ifndef TRAINTICKETSYSTEM_ORDERSYSTEM_H
#define TRAINTICKETSYSTEM_ORDERSYSTEM_H

#include "Tools/Time.h"
#include "LogSystem.h"

namespace backEnd {

enum OrderStatus{ UNKNOWN, SUCCESS, PENDING, REFUNDED};

struct Order_key{
    size_t date; //日期
    size_t tid;  //车辆
    bool operator<(const Order_key &b) const {
        return (date < b.date) || (date == b.date && tid < b.tid);
    }
    bool operator==(const Order_key &b) const {
        return (date == b.date && tid == b.tid);
    }
};

struct Order{
    OrderStatus _status = UNKNOWN;
    Tools::String<22> username;  //购票人
    Tools::Time date;  //购买的车票日期
    Tools::String<22> trainID;  //车辆ID
    Tools::String<20> stationBegin;  //起始站
    Tools::Time beginTime;
    Tools::String<20> stationEnd;  //到达站
    Tools::Time endTime;
    int price = 0;  //票价
    int ticketNum = 0;  //车票张数
    Order() = default;
};

struct Refund{  //仅用于回滚：维护一次退票后候补队列的买进记录
    Order refundOrder; //退票订单的信息
    int buy = 0;  //本次退票引发的候补队列的变动数量(>=0)
    Order orderBuyList[100];  //具体变动的订单信息
};

class WaitingQueue {
private:
    Tools::BPlusTree<Order_key, Order, true> trainWaitingQueue;  //某车次的候补队列, 一对多
public:
    WaitingQueue() = default;
    explicit WaitingQueue(const std::string &filename) : trainWaitingQueue(filename){}
    ~WaitingQueue() = default;
    bool insert(size_t uid, size_t sid, Order &orderInfo);  //加入候补队列
    bool findAndSale(Order &orderInfo);  //有人退票后补票
    bool refund(Order &orderInfo);  //候补队列中的退票行为

    void clean();

};

class OrderSystem {
private:
    WaitingQueue waiting_queue;  //维护所有的等待队列
    Tools::BPlusTree<size_t, Order, true, true> allOrder;  //维护所有用户的订单信息, 一对多，不断向前插入
    Tools::BPlusTree<int, Refund> refundRecord;  //仅用于回滚：时间戳下的退款记录
public:
    OrderSystem() = default;
    explicit OrderSystem(const std::string &queueFN, const std::string &orderFN)
            : waiting_queue(queueFN), allOrder(orderFN) {}
    ~OrderSystem() = default;
    bool buy_ticket(const std::string &username, const std::string &trainID, const std::string &date, int ticketNum,
                    const std::string &station1, const std::string station2, bool wait = false);
    bool query_order(const std::string &username);
    bool refund_ticket(int timestamp, const std::string &username, int orderNum);
    void clean();
    //回滚函数
    void back_refund_ticket(int timestamp);
};

}


#endif //TRAINTICKETSYSTEM_ORDERSYSTEM_H
