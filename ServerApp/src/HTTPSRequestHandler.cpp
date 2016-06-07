//
// Created by atos on 04.06.16.
//

#include "HTTPSRequestHandler.h"
#include "HTTPServerFacade.h"
#include "Exception.h"
#include <fstream>
#include <iostream>
#include <string>
#include "Server.h"
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPCookie.h>

using namespace Poco::Net;
using namespace std;

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
    /** Validate user */
    HTMLForm form(request, request.stream());
    string login = form.get("login");
    string password = form.get("password");
    if(Server::getServer().verifyUser(login.c_str(), password.c_str())) {
        /** send cookie and save session id */
        string sessionId = HTTPServerFacade::getServer().genSessionId();
        HTTPCookie cookie("sessionId", sessionId);
        cookie.setMaxAge(HTTPServerFacade::sessionIdTimeout);
        response.addCookie(cookie);
        /** save session id in the server */
        HTTPServerFacade::getServer().addSessionId(login, sessionId);

    }
    else {
        response.setStatusAndReason(HTTPResponse::HTTP_UNAUTHORIZED, "Invalid login/password");
        response.send() << "<html><head></head><body><h1>Invalid login/password</h1></body></html>";
    }




    initFile("/home/atos/Projects/TIN/ServerApp/src/html/reservations.html");
    //@TODO ADD LOGIN AND SETTING COOKIES!





//    response.redirect("/html/reservations.html");
    response.send() << stringStream.str();
}

