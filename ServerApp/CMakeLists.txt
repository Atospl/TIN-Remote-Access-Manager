cmake_minimum_required(VERSION 3.2)
project(ServerApp)
file(MAKE_DIRECTORY Debug)
set(EXECUTABLE_OUTPUT_PATH Debug)

find_package(OpenSSL REQUIRED)
find_library(POCO_FOUNDATION PocoFoundation)
find_library(POCO_NET PocoNet)
include_directories(${OPENSSL_INCLUDE_DIR} )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lssl -lcrypto -lPocoFoundation -lPocoNet  -lPocoNetSSL -std=c++11 -pthread")

set(SOURCE_FILES src/main.cpp src/Server.cpp src/Server.h src/Session.cpp src/Session.h src/FileController.cpp src/FileController.h ../Shared/Message.cpp ../Shared/Message.h src/IptablesController.cpp src/IptablesController.h src/FilesPaths.cpp src/FilesPaths.h src/HTTPServerFacade.cpp src/HTTPServerFacade.h src/Exception.cpp src/Exception.h src/HTTPSRequestHandler.cpp src/HTTPSRequestHandler.h src/RequestHandlerFactory.cpp src/RequestHandlerFactory.h)
add_executable(ServerApp ${SOURCE_FILES} src/HTTPServerFacade.cpp src/HTTPServerFacade.h src/Exception.cpp src/Exception.h src/HTTPSRequestHandler.cpp src/HTTPSRequestHandler.h src/RequestHandlerFactory.cpp src/RequestHandlerFactory.h)
target_link_libraries(ServerApp ${OPENSSL_LIBRARIES} ${POCO_FOUNDATION} ${POCO_NET})
