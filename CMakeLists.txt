cmake_minimum_required(VERSION 3.22)
project(csnetlab)

set(CMAKE_CXX_STANDARD 17)

find_package (Threads)
add_executable(lab1 lab1/main.cpp lab1/httpParse.cpp)
target_link_libraries (lab1 ${CMAKE_THREAD_LIBS_INIT})
