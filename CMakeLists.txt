cmake_minimum_required(VERSION 3.5)
project(TIN_Remote_Access_Manager)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")


set(SOURCE_FILES src/main.cpp src/Server.cpp src/Server.h src/Session.cpp src/Session.h)
add_executable(TIN_Remote_Access_Manager ${SOURCE_FILES})