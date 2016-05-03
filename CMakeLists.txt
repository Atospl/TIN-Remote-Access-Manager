cmake_minimum_required(VERSION 3.5)
project(TIN_Remote_Access_Manager)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(SOURCE_FILES main.cpp)
add_executable(TIN_Remote_Access_Manager ${SOURCE_FILES})