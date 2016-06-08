//
// Created by atos on 04.06.16.
//
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>

#include "HTTPServerFacade.h"
#include "RequestHandlerFactory.h"
#include "Exception.h"

using namespace Poco::Net;

HTTPServerFacade::HTTPServerFacade() {
    port = FilesPaths::getInstance().getServerPort();

    //set up server parameters
    pParams = new HTTPServerParams();
    pParams->setMaxQueued(maxQueued);
    pParams->setMaxThreads(maxThreads);

    //set up server socket
    sock = new ServerSocket(port);

    //set up server
    server = new HTTPServer(new RequestHandlerFactory(), *sock, pParams);

    isRunning = false;
}

HTTPServerFacade &HTTPServerFacade::getServer() {
    static HTTPServerFacade serverFacade;
    return serverFacade;
}

HTTPServerFacade::~HTTPServerFacade() {
    pParams->release();
    delete sock;
    delete server;
}

void HTTPServerFacade::runServer() {
    server->start();
    isRunning = true;
}

void HTTPServerFacade::stopServer() {
    if(isRunning)
        server->stop();
}

string HTTPServerFacade::genSessionId() {
    /** Simple algorithm for random string. Not very safe -
     * you should use boost::random to achieve it */
    // @TODO sometimes it fucks up. Add some sleep or sth
    auto randchar = []() -> char
    {
        const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(cookieLen,0);
    std::generate_n( str.begin(), cookieLen, randchar );
    return str;
//    srand(time(NULL) + rand());
//    stringstream ss;
//    for(int i = 0;i < 16;i++)
//    {
//        int j = rand() % 127;
//        while(j < 32)
//            j = rand() % 127;
//        ss << char(j);
//    }
//    return ss.str();
}

void HTTPServerFacade::addSessionId(std::string login, std::string sessionId) {
    userSessionIds.push_back(std::make_tuple(login, sessionId, time(NULL) + sessionIdTimeout));
}








