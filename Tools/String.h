#ifndef TRAINTICKETSYSTEM_STRING_H
#define TRAINTICKETSYSTEM_STRING_H

#include <iostream>
#include <string>
#include <cstring>

namespace Tools {

template<size_t LENGTH = 100>
class String {
private:
    char str[LENGTH]{};
public:
    String() = default;
    explicit String(const std::string &s){
        strcpy(str, s.c_str());
    }
    String(const String &s){
        strcpy(str, s.str);
    }

    ~String() = default;

    void initialise(const std::string &s){
        strcpy(str, s.c_str());
    }

    String &operator = (const std::string &s) {
        strcpy(str, s.c_str());
    }

    //bool operator>(const String &s) const;
    //bool operator>=(const String &s) const;
    //bool operator<(const String &s) const;
    //bool operator<=(const String &s) const;
    //bool operator==(const String &s) const;
    //bool operator!=(const String &s) const;

    bool operator == (const std::string &s) const {
        return strcmp(str, s.c_str());
    }

    bool operator != (const std::string &s) const {
        return !strcmp(str, s.c_str());
    }

    friend std::ostream &operator<<(std::ostream &os, const String &s){
        os << *s.str;
        return os;
    }
    friend std::istream &operator>>(std::istream &is, String &s){
        is >> s.str;
        return is;
    }
    char& operator[] (int index){
        return str[index];
    }
    const char &operator[] (int index) const {
        return str[index];
    }
    void clear() const {
        str[0] = '\0';
    }
    bool empty() const {
        return str[0] == '\0';
    }

};

}


#endif //TRAINTICKETSYSTEM_STRING_H
