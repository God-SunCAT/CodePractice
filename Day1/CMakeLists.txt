cmake_minimum_required(VERSION 4.1)
project(Day1)

set(CMAKE_CXX_STANDARD 20)

add_executable(Day1 BufferPool_deserted.cpp
        test.cpp
        BufferPool.h
        BufferPool.cpp)
