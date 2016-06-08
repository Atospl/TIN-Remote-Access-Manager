//
// Created by atos on 04.06.16.
//

#include "HTTPSRequestHandler.h"
#include "HTTPServerFacade.h"
#include "Exception.h"
#include <tuple>
#include "FileController.h"
#include <fstream>
#include <iostream>
#include <string>
#include "Server.h"
#include <Poco/Net/HTTPResponse.h>
#include "IptablesController.h"
#include <Poco/Net/HTMLForm.h>
#include <regex>
#include <string>
#include <ctime>
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
        HTTPCookie loginCookie("login", login);
        cookie.setMaxAge(HTTPServerFacade::sessionIdTimeout);
        response.addCookie(cookie);
        response.addCookie(loginCookie);
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
    string ip = request.clientAddress().toString();
    //Access Request
    if(request.getContentLength() == 0)
    {
        IptablesController::grantLimitedAccess(ip, HTTPServerFacade::accessTime);
    }
    HTMLForm form(request, request.stream());
    //parse date
    time_t rawtime;
    struct tm* dateStruct;

    string machineId = form.get("machineId");
    string date = form.get("date");
    regex pattern("(\d{2})\/(\d{2})\/(\d{4})");
    smatch result;
    if(!regex_search(date, result, pattern)) {
        response.send() << "<html><head></head><body><h1>Invalid request</h1></body></html>";
        return;
    }
    //parse to time_t
    time(&rawtime);
    dateStruct = localtime(&rawtime);
    dateStruct->tm_year = stoi(result[2]);
    dateStruct->tm_mon = stoi(result[1]) - 1;
    dateStruct->tm_mday = stoi(result[0]);

    rawtime = mktime(dateStruct);

    //get user login
    Poco::Net::NameValueCollection cookies;
    request.getCookies(cookies);
    Poco::Net::NameValueCollection::ConstIterator it = cookies.find("login");
    std::string login;
    if (it != cookies.end())
        login = it->second;

    bool reservationResult = FileController::getInstance().addReservation(login, stoi(machineId), rawtime);
    if(reservationResult)
    {
        response.send() << "<html><head></head><body><h1>Ok!</h1></body></html>";
        return;
    }
    else
    {
        response.send() << "<html><head></head><body><h1>Couldn't make reservation</h1></body></html>";
        return;
    }
}

