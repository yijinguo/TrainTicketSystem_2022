#ifndef TRAINTICKETSYSTEM_PARSE_H
#define TRAINTICKETSYSTEM_PARSE_H

#include <iostream>
#include <cstring>
#include "SJTU/vector.hpp"

namespace backEnd {

class Command {

public:

    int timestamp = 0; //命令行的时间戳
    std::string cmd;

    Command() = default;
    explicit Command(std::string &command){
        timestamp = 0;
        cmd = command;
        std::string time = takeFirstWord();
        for (int i = 1; i < time.length() - 1; ++i)
            timestamp = timestamp * 10 + time[i] - '0';
    }
    ~Command() = default;

    void Initialise(std::string &command){
        timestamp = 0;
        cmd = command;
        std::string time = takeFirstWord();
        for (int i = 1; i < time.length() - 1; ++i)
            timestamp = timestamp * 10 + time[i] - '0';
    }

    std::string takeFirstWord() {//获取字符串cmd的第一个单词，并将cmd第一个单词剔除
        int index = 0;
        int firstWordIndex;
        std::string firstWord;
        while (cmd[index] == ' ') { ++index; }
        firstWordIndex = index;
        while (cmd[index] != ' ' && cmd[index] != '\0') { ++index; }
        char word[index - firstWordIndex + 1];
        for (int i = 0; i < index - firstWordIndex; ++i) { word[i] = cmd[i + firstWordIndex]; }
        word[index - firstWordIndex] = '\0';
        std::string result = std::string(word);
        while (cmd[index] == ' ') { ++index; }
        int l = cmd.length();
        if (cmd[index] == '\0') {
            cmd = "";
            return result;
        }
        for (int i = index; i < l; ++i) cmd[i - index] = cmd[i];
        cmd[l - index] = '\0';
        return result;
    }

    bool read_empty(std::string cmd);  //可能需要的函数：判断字符串cmd是否被读完（仅剩空格）
    void clear();  //可能需要的函数：清空数据
};

}
#endif //TRAINTICKETSYSTEM_PARSE_H
