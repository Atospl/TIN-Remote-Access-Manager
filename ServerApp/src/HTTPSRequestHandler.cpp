//
// Created by atos on 04.06.16.
//

#include "HTTPSRequestHandler.h"
#include "HTTPServerFacade.h"
#include "Exception.h"
#include <tuple>
#include <fstream>
#include <iostream>
#include <string>
#include "Server.h"
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPCookie.h>

using namespace Poco::Net;
using namespace std;

/** BaseRequestHandler */
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



/** ROOT GET */
void RootGetHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                                Poco::Net::HTTPServerResponse& response)
{
    initFile("/home/atos/Projects/TIN/ServerApp/src/html/index.html");
    response.send() << stringStream.str();
}



/** ROOT POST */
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
        response.redirect("/reservations.html");

    }
    else {
        response.setStatusAndReason(HTTPResponse::HTTP_UNAUTHORIZED, "Invalid login/password");
        response.send() << "<html><head></head><body><h1>Invalid login/password</h1></body></html>";
    }
}




/** RESERVATION GET */
void ReservationGetHandler::handleRequest(Poco::Net::HTTPServerRequest &request,
                                          Poco::Net::HTTPServerResponse &response) {
//    if(request.getCookies())
    // check if session id is valid
    Poco::Net::NameValueCollection cookies;
    request.getCookies(cookies);
    /** check if session id is in the server */
    Poco::Net::NameValueCollection::ConstIterator it = cookies.find("sessionId");
    //@TODO check what happens when you dont have any cookie
    string clientSessionId = it->second;
    if(!checkSessionId(clientSessionId)) {
        response.setStatusAndReason(HTTPResponse::HTTP_UNAUTHORIZED, "Invalid login/password");
        response.send() << "<html><head></head><body><h1>Invalid login/password</h1></body></html>";
    }

    initFile("/home/atos/Projects/TIN/ServerApp/src/html/reservations.html");
    response.send() << stringStream.str();
}

bool ReservationGetHandler::checkSessionId(std::string sessionId) {
    /** @TODO BAD code over here... */
    std::vector<std::tuple<std::string, std::string, time_t>> tuples = HTTPServerFacade::getServer().getSessionIds();
    for(std::vector<std::tuple<std::string, std::string, time_t>>::iterator it  = tuples.begin(); it != tuples.end(); ++it)
    {
        /** get rid of sessions with  expired timeout */
        if(std::get<2>(*it) < time(NULL)) {
            tuples.erase(it);
            continue;
        }
        if(std::get<1>(*it) == sessionId)
            return true;
    }
    return false;
}



/** Reservation POST */
void ReservationPostHandler::handleRequest(Poco::Net::HTTPServerRequest &request,
                                           Poco::Net::HTTPServerResponse &response) {
    //Access Request
    if(request.getContentLength() == 0)
    {
        //IptablesController::grantLimitedAccess(inet_ntop(AF_INET, &ip4Address, buf, 16), minutes);
    }
}

