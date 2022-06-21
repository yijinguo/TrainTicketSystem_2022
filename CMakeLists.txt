cmake_minimum_required(VERSION 3.16)
project(TrainTicketSystem)

set(CMAKE_CXX_STANDARD 14)

add_executable(code 1412.cpp
        parse.h
        Execute.h
        LogSystem.h
        TrainSystem.h
        SJTU/map.hpp
        SJTU/vector.hpp
        SJTU/priority_queue.hpp
        SJTU/linked_hashmap.hpp
        SJTU/exceptions.hpp
        SJTU/utility.hpp
        Tools/String.h
        Tools/BPlusTree.h)