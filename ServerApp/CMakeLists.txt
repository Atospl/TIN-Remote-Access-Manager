cmake_minimum_required(VERSION 3.5)
project(TIN_Remote_Access_Manager)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lssl -lcrypto -std=c++11 -pthread")

set(SOURCE_FILES src/main.cpp src/Server.cpp src/Server.h src/Session.cpp src/Session.h src/FileController.cpp src/FileController.h ../Shared/Message.cpp ../Shared/Message.h)
add_executable(TIN_Remote_Access_Manager ${SOURCE_FILES})
target_link_libraries(TIN_Remote_Access_Manager ${OPENSSL_LIBRARIES})
