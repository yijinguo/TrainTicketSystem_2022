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
public:

    Tools::String<22> username;  //账户名，唯一标识符；<=20
    Tools::String<32> password;  //密码；<=30
    Tools::String<25> name; //姓名，汉字2-5个
    Tools::String<32> mailAddr;  //邮箱，不考虑合法性；<=30
    int privilege = 0; //权限；0-10

    User() = default;
    ~User() = default;
    User(const std::string &_username, const std::string &_password, const std::string &_name, const std::string &_mailAddr, int pri){
        username.initialise(_username);
        password.initialise(_password);
        name.initialise(_name);
        mailAddr.initialise(_mailAddr);
        privilege = pri;
    }
    //void modify(const std::string &_password, const std::string &_name, const std::string &_mailAddr, int pri)
    //bool passwd_same(const std::string &passwd); //辅助函数，用于判断密码是否正确
    void print() const { //辅助函数，用于输出用户信息
        cout << username << ' ' << name << ' ' << mailAddr << ' ' << privilege << '\n';
    }
};


class LogSystem{
    /*对外接口*/
private:
    Tools::BPlusTree<Tools::String<22>, User> userList("userList", "userListData");  //维护所有用户信息
    //Tools::BPlusTree<int, User> modifyRecord;  //仅用于回滚；时间戳下的修改记录，User记录修改前的用户信息
public:
    sjtu::map<Tools::String<22>, int> logList;  //记录登录用户(视为能够用于其他系统的全局变量，程序运行时才有意义)

    LogSystem() = default;
    ~LogSystem() = default;

    //各项相关操作
    /* op_name:当前操作者的uid
     * username, password, name, mailAddr, pri一一对应于User类
     * 返回类型为bool表示操作是否成功，操作成功返回true*/
    bool add_user(const Tools::String<22> &op_name,const std::string &username, const std::string &password, const std::string &name, const std::string mailAddr, int pri){
        User cur;
        if (!logList.find(op_name)) return false;
        if (!userList.find(op_name, 0, cur)) return false;
        if (cur.privilege <= pri) return false;
        User newUser(username, password, name, mailAddr, pri);
        if (!userList.Insert(newUser.username.hash(), 0, newUser))
            return false;
        else
            return true;
    }
    bool login(const std::string &username, const std::string &password) {
        Tools::String<22> uid_name(username);
        size_t uid = uid_name.hash();
        if (logList.find(uid) && logList[uid]) return false;
        User logUser;
        if (!userList.find(uid, 0, logUser)) return false;
        if (logUser.password != password) return false;
        logList[uid] = 1;
        return true;
    }

    bool logout(const std::string &username) {
        Tools::String<22> uid_name(username);
        size_t uid = uid_name.hash();
        if (!logList.find(uid) return false;
        if (logList[uid] == 0) return false;
        logList[uid] = 0;
        return true;
    }
    bool query_profile(const size_t &op_name, const std::string &username) {
        if (!logList.find(op_name)) return false;
        if (!logList[op_name]) return false;
        User cur, queryUser;
        if (!userList.find(op_name, 0, cur)) return false;
        Tools::String<22> queryUsername(username);
        size_t queryUid = queryUsername.hash();
        if (!userList.find(queryUid,0,queryUser)) return false;
        if (cur.privilege < queryUser.privilege) return false;
        queryUser.print();
        return true;
    }
    bool modify_profile(int timestamp, const size_t &op_name, const std::string &username, const std::string &password, const std::string &name, const std::string &mailAddr, int pri) {
        if (!logList.find(op_name)) return false;
        if (!logList[op_name]) return false;
        Tools::String<22> modifyUsername(username);
        size_t modifyUid = modifyUsername.hash();
        User cur, modifyUser;
        if (!userList.find(op_name,0,cur)) return false;
        if (cur.privilege <= pri) return false;
        if (!userList.find(modifyUid, 0, modifyUser)) return false;
        if (cur.privilege < modifyUser.privilege) return false;
        if (password != "\0") modifyUser.password = password;
        if (name != "\0") modifyUser.name = name;
        if (mailAddr != "\0") modifyUser.mailAddr = mailAddr;
        if (pri != -1) modifyUser.privilege = pri;
        userList.modify(modifyUid,0,modifyUser);
        modifyUser.print();
        return true;
    }
    void clean() {//清除数据
        userList.clear();
        logList.clear();
    }
    //与其他系统交互
    bool isLog(const size_t &username){
        return logList.find(username);
    }
    void clearLogList() {  //将所有用户下线
        logList.clear();
    }
    //回滚函数
    bool delete_user(const size_t &uid);
    bool back_modify_profile(int timestamp);
};

}

#endif //TRAINTICKETSYSTEM_LOGSYSTEM_H
