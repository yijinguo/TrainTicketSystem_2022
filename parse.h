#ifndef TRAINTICKETSYSTEM_PARSE_H
#define TRAINTICKETSYSTEM_PARSE_H

#include <iostream>
#include <cstring>
namespace backEnd {

class Command {
private:
    struct Node {
        struct data {
            char type;  //单字符表示的信息类型
            std::string cmd;  //对应单词
            data(){
                type = '\0';
                cmd = "";
            }
            data(char _type, const std::string &_cmd):type(_type){
                cmd = _cmd;
            }
        } info;
        Node *front = nullptr;
        Node *next = nullptr;
        Node() = default;
        Node(char _type, const std::string &_cmd):info(_type, _cmd){}
    };  //一个结点表示一个有效信息
    int timestamp = 0; //命令行的时间戳
    std::string cmdType;  //命令行第一个单词，表示指令类型
    Node *head = nullptr;
    Node *rear = nullptr;
    //链表存储除首单词外的命令
public:
    Command() = default;
    Command(std::string &command);  //利用整行命令构造一个Command变量
    ~Command();
    void parseCmd(std::string &command); //本类的核心函数：解析输入的命令行command，将解析结果存入cmd_p中
    //辅助函数
    void take_first(std::string &cmd);  //获取字符串cmd的第一个单词，并将cmd第一个单词剔除
    bool read_empty(std::string cmd);  //可能需要的函数：判断字符串cmd是否被读完（仅剩空格）
    void clear();  //可能需要的函数：清空数据
};

}
#endif //TRAINTICKETSYSTEM_PARSE_H
