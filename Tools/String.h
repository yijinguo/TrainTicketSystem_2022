#ifndef TRAINTICKETSYSTEM_STRING_H
#define TRAINTICKETSYSTEM_STRING_H

#include <iostream>
#include <string>
#include <cstring>

namespace Tools {

template<size_t LENGTH = 100>
class String {
private:
    char str[LENGTH]{0};
public:
    String() = default;
    String(const char *s);
    String(const std::string &s);
    String(const String &s);
    ~String() = default;
    bool operator>(const String &s) const;
    bool operator>=(const String &s) const;
    bool operator<(const String &s) const;
    bool operator<=(const String &s) const;
    bool operator==(const String &s) const;
    bool operator!=(const String &s) const;
    friend std::ostream &operator<<(std::ostream &os, const String &s);
    friend std::istream &operator>>(std::istream &is, String &s);
    char& operator[] (int index);
    const char& operator[] (int index) const;
    void clear();
    bool empty();
    size_t hash(); //求哈希值
};

}


#endif //TRAINTICKETSYSTEM_STRING_H
