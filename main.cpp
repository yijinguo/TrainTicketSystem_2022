#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

#include "Execute.h"

int main(){
    backEnd::System system;
    std::string cmd;
    while (getline(cin, cmd)) {
        try {
            system.execute(cmd);
        } catch (...) {
            break;
        }
    }
    return 0;
}