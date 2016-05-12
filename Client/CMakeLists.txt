cmake_minimum_required(VERSION 3.4)
project(Client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

set(SOURCE_FILES src/main.cpp src/Client.cpp src/Client.h)
add_executable(Client ${SOURCE_FILES})
target_link_libraries(Client ${OPENSSL_LIBRARIES})
