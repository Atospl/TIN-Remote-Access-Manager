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




