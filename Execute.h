#ifndef TRAINTICKETSYSTEM_EXECUTE_H
#define TRAINTICKETSYSTEM_EXECUTE_H

#include "parse.h"
#include "TrainSystem.h"
#define MAXSIZE = 1e3  //该数值为一行命令除时间戳外占用的字符数最大值

namespace backEnd {

class System {
private:

    Command command;
    LogSystem logSystem;
    TrainSystem trainSystem;
    OrderSystem orderSystem;
    struct cmdLine{
        bool existence = true;  //该命令行是否为有效命令行(否：false)
        char line[1000];
    };
    Tools::BPlusTree<int, cmdLine> timestampSystem;

    System() = default;
    ~System() = default;
    void add_user();  //对应回滚函数：LogSystem::delete_user
    void login();  //对应回滚函数：logout
    void logout();  //对应回滚函数：login
    void query_profile();
    void modify_profile();  //对应回滚函数：back_modify_profile（有维护该操作的数据结构）
    void add_train();  //对应回滚函数：TrainSystem::delete_train
    void release_train();  //对应回滚函数：TrainSystem::close_train
    void query_train();
    void query_ticket();
    void query_transfer();
    void buy_ticket();  //对应回滚函数：TrainSystem::refund_ticket + OrderSystem::refund_ticket
    void query_order();
    void refund_ticket();  //对应回滚函数：TrainSystem::buy_ticket + OrderSystem::buy_ticket + 候补队列还原 （有维护该操作的数据结构）
    void rollback();  //对应回滚函数：?
    void clean();
    void exit();

    void clear(); //辅助函数，每次execute后清理command

public:

    void execute(std::string &cmd); //核心执行函数，执行各种操作

};

}

#endif //TRAINTICKETSYSTEM_EXECUTE_H
