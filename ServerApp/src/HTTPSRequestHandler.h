//
// Created by atos on 04.06.16.
//

#ifndef SERVERAPP_HTTPREQUESTHHANDLER_H
#define SERVERAPP_HTTPREQUESTHHANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <fstream>
#include <sstream>
#include <string>

/** Abstract base class for request handlers */
class BaseRequestHandler : public Poco::Net::HTTPRequestHandler {
public:
//    virtual void handleRequest(Poco::Net::HTTPServerRequest& request,
//                       Poco::Net::HTTPServerResponse& response);
    /** Initialize file stream, read file to str */
    virtual void initFile(std::string path);
protected:
    std::ifstream file;
    std::stringstream stringStream;
};

class RootHandler : public BaseRequestHandler
{
public:
    virtual void handleRequest(Poco::Net::HTTPServerRequest& request,
                               Poco::Net::HTTPServerResponse& response);
};

#endif //SERVERAPP_HTTPREQUESTHHANDLER_H
