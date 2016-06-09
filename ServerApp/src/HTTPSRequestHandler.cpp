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
    initFile("./index.html");
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

    initFile("./reservations.html");
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
    //get user login
    Poco::Net::NameValueCollection cookies;
    request.getCookies(cookies);
    Poco::Net::NameValueCollection::ConstIterator it = cookies.find("login");
    std::string login;
    if (it != cookies.end())
        login = it->second;

    string ip = request.clientAddress().toString();
    ip = ip.substr(0, ip.find(":", 0));
    //Access Request
    if(request.getContentLength() == 0)
    {
        unsigned int minutes = Server::getServer().checkAvailableTime(login);
        if(minutes > 0) {
            IptablesController::grantLimitedAccess(ip, minutes);
            response.send() << "<html><head></head><body><h1>Access granted</h1></body></html>";
            return;
        }
        else {
            response.send() << "<html><head></head><body><h1>Access denied</h1></body></html>";
            return;
        }

    }
    HTMLForm form(request, request.stream());
    //parse date
    time_t rawtime;
    struct tm* dateStruct;

    string machineId = form.get("machineId");
    string date = form.get("date");
    string hour = form.get("time");

    regex datePattern("(\\d{2})\\/(\\d{2})\\/(\\d{4})");
    smatch dateResult;
    regex hourPattern("(\\d{2}):(\\d{2})");
    smatch hourResult;
    if(!regex_search(date, dateResult, datePattern) || !regex_search(hour, hourResult, hourPattern)) {

        response.send() << "<html><head></head><body><h1>Invalid request</h1></body></html>";
        return;
    }
    if(stoi(dateResult[1]) > 31 || stoi(dateResult[1]) < 0 || stoi(dateResult[2]) > 12 || stoi(dateResult[2]) < 1 ||
            stoi(hourResult[1]) > 24 || stoi(hourResult[1]) < 0 || stoi(hourResult[2]) > 60 || stoi(hourResult[2]) < 0)
    {
        response.send() << "<html><head></head><body><h1>Invalid reservation</h1></body></html>";
        return;
    }

    //parse to time_t
    time(&rawtime);
    dateStruct = localtime(&rawtime);
    dateStruct->tm_year = stoi(dateResult[3]) - 1900;
    dateStruct->tm_mon = stoi(dateResult[2]) - 1;
    dateStruct->tm_mday = stoi(dateResult[1]);
    dateStruct->tm_hour = stoi(hourResult[1]);
    dateStruct->tm_min = stoi(hourResult[2]);
    dateStruct->tm_sec = 0;
    dateStruct->tm_isdst = -1;
    rawtime = mktime(dateStruct);

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

