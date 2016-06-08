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
#include <vector>
#include <tuple>
#include <ctime>

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

    /** generates random session id */
    string genSessionId();
    /** adds new session id to the vector and sets its timeout */
    void addSessionId(std::string login, std::string sessionId);
    /** get vector with beautiful tuple */
    std::vector<std::tuple<std::string, std::string, time_t>>& getSessionIds() { return userSessionIds; }

    const static unsigned int sessionIdTimeout = 3600;
    const static unsigned int accessTime = 60;

private:
    /** Singleton's private constructor. Imports certificates, keys and port number from file */
    HTTPServerFacade();

    const unsigned int maxQueued = 100;
    const unsigned int maxThreads = 50;
    const unsigned int cookieLen = 16;

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
    /** vector containing users with their session ids with their timeouts */
    std::vector<std::tuple<std::string, std::string, time_t>> userSessionIds;
};


#endif //SERVERAPP_HTTPSERVER_H
