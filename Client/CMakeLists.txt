cmake_minimum_required(VERSION 3.5)
project(Client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp src/Client.cpp src/Client.h)
add_executable(Client ${SOURCE_FILES})