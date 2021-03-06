#include "Tools/BPlusTree.h"
#include <stdio.h>


struct Key{
    char Index[65] = {'\0'};
    Key() = default;
    explicit Key(const std::string &s) {
        strcpy(Index, s.c_str());
    }
    bool operator<(const Key &b) const {
        if (strcmp(Index, b.Index) < 0)
            return true;
        else
            return false;
    }
};

struct Value {
    char Index[65] = {'\0'};
    int t = 0;
    Value() = default;
    Value(const std::string &s, int value): t(value){
        strcpy(Index, s.c_str());
    }
};

int main(){
    //freopen("5.in", "r", stdin);
    //freopen("gyj.out", "w", stdout);
    Tools::BPlusTree<Key, Value, 10, 10, int> tree("file", "datafile");
    int n;
    scanf("%d", &n);
    std::string op;
    while (n--) {
        std::cin >> op;
        if (n == 1)
            int i = 0;
        if (op == "insert") {
            std::string in;
            int inValue;
            std::cin >> in >> inValue;
            Key index(in);
            Value value(in, inValue);
            tree.Insert(index, inValue, value);
        } else if (op == "delete") {
            std::string s;
            int inValue;
            std::cin >> s >> inValue;
            Key index(s);
            tree.Remove(index, inValue);
        } else if (op == "find") {
            std::string s;
            std::cin >> s;
            Key index(s);
            int num = 0;
            sjtu::vector<Value> t;
            tree.find(index,  t);
            if (t.empty()) {
                std::cout << "null\n";
            } else {
                auto it = t.begin();
                while (it != t.end()) {
                    std::cout << (*it).t << ' ';
                    it++;
                }
                std::cout << '\n';
            }
        } else if (op == "modify") {
            std::string s;
            int origin;
            int inValue;
            std::cin >> s >> origin >> inValue;
            Key index(s);
            Value value(s, inValue);
            tree.modify(index, origin, value);
        }
    }
    //remove("file");
    //remove("datafile");
}

