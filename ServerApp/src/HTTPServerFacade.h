//
// Created by atos on 04.06.16.
//

#ifndef SERVERAPP_HTTPSERVER_H
#define SERVERAPP_HTTPSERVER_H

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTTPCookie.h>
#include <Poco/Net/ServerSocket.h>

#include "Server.h"

class HTTPServerFacade
{
public:
    /** Returns HTTPS Server instance */
    static HTTPServerFacade& getServer();
    /** Server destructor */
    ~HTTPServerFacade();
    /** Start the server */
    void runServer();
    /** Stop the server */
    void stopServer();
private:
    /** Singleton's private constructor. Imports certificates, keys and port number from file */
    HTTPServerFacade();

    const unsigned int maxQueued = 100;
    const unsigned int maxThreads = 50;

    /** Port used by http server */
    Poco::UInt16 port;
    /** HTTP Server parameters */
    Poco::Net::HTTPServerParams* pParams;
    /** Proper HTTP Server */
    Poco::Net::HTTPServer* server;
    /** Poco's server socket */
    Poco::Net::ServerSocket* sock;

    /** True if server has been started, false if not */
    bool isRunning;
};


#endif //SERVERAPP_HTTPSERVER_H
