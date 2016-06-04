//
// Created by atos on 04.06.16.
//

#include "HTTPSRequestHandler.h"
#include "Exception.h"
#include <fstream>
#include <iostream>

void BaseRequestHandler::initFile(std::string path)
{
    file.open(path);
    if(file.is_open())
    {
        stringStream << file.rdbuf();
        file.close();
    }
    else
        throw FileHandlerException(FileHandlerException::ErrorCode::FILE_OPEN);

}

void RootGetHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                                Poco::Net::HTTPServerResponse& response)
{
    initFile("/home/atos/Projects/TIN/ServerApp/src/html/index.html");
    response.send() << stringStream.str();
}

void RootPostHandler::handleRequest(Poco::Net::HTTPServerRequest &request,
                                    Poco::Net::HTTPServerResponse &response)
{
    initFile("/home/atos/Projects/TIN/ServerApp/src/html/reservations.html");
    //@TODO ADD LOGIN AND SETTING COOKIES!
//    response.redirect("/html/reservations.html");
    response.send() << stringStream.str();
}

