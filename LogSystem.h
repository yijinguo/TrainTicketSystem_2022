#ifndef TRAINTICKETSYSTEM_LOGSYSTEM_H
#define TRAINTICKETSYSTEM_LOGSYSTEM_H

#include <iostream>
#include "Tools/String.h"
#include "Tools/BPlusTree.h"
#include "SJTU/map.hpp"

namespace backEnd {

class User {
    /*维护每条用户信息*/
    /*username用户唯一，以字符串对应的size_t作为索引，简称为用户uid*/
private:
    Tools::String<22> username;  //账户名，唯一标识符；<=20
    Tools::String<32> password;  //密码；<=30
    Tools::String<25> name; //姓名，汉字2-5个
    Tools::String<32> mailAddr;  //邮箱，不考虑合法性；<=30
    int privilege = 0; //权限；0-10
public:
    User() = default;
    ~User() = default;
    User(const std::string &_username, const std::string &_password, const std::string &_name, const std::string &_mailAddr, int pri);
    void modify(const std::string &_password, const std::string &_name, const std::string &_mailAddr, int pri);
    bool passwd_same(const std::string &passwd); //辅助函数，用于判断密码是否正确
    void print(std::ostream & os); //辅助函数，用于输出用户信息
};


class LogSystem{
    /*对外接口*/
private:
    Tools::BPlusTree<size_t, User> userList;  //维护所有用户信息
    Tools::BPlusTree<int, User> modifyRecord;  //仅用于回滚；时间戳下的修改记录，User记录修改前的用户信息
public:
    sjtu::map<size_t, int> logList;  //记录登录用户(视为能够用于其他系统的全局变量，程序运行时才有意义)
    explicit LogSystem(const std::string &file_name):userList(file_name){} //利用文件构造
    //各项相关操作
    /* op_name:当前操作者的uid
     * username, password, name, mailAddr, pri一一对应于User类
     * 返回类型为bool表示操作是否成功，操作成功返回true*/
    bool add_user(const size_t &op_name,const std::string &username, const std::string &password, const std::string &name, const std::string mailAddr, int pri);
    bool login(const std::string &username, const std::string password);
    bool logout(const std::string &username);
    bool query_profile(const size_t &op_name, const std::string &username);
    bool modify_profile(int timestamp, const size_t &op_name, const std::string &username, const std::string &password, const std::string &name, const std::string mailAddr, int pri);
    void clean(); //清除数据
    //与其他系统交互
    bool isLog(const size_t &username);
    void clearLogList();  //将所有用户下线
    //回滚函数
    bool delete_user(const size_t &uid);
    bool back_modify_profile(int timestamp);
};

}

#endif //TRAINTICKETSYSTEM_LOGSYSTEM_H
