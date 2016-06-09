//
// Created by atos on 04.06.16.
//

#ifndef SERVERAPP_REQUESTHANDLER_H
#define SERVERAPP_REQUESTHANDLER_H

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include "HTTPSRequestHandler.h"
#include <iostream>


class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
public:
    RequestHandlerFactory()
    {}

    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request)
    {
        std::cout << request.getURI() << std::endl;
        std::cout << request.getMethod() << std::endl;
        if (request.getURI() == "/" && request.getMethod() == "GET")
            return (Poco::Net::HTTPRequestHandler*) new RootGetHandler();
        else
            if(request.getURI() == "/" && request.getMethod() == "POST")
                return (Poco::Net::HTTPRequestHandler*) new RootPostHandler();
            else
                if(request.getURI() == "/reservations.html" && request.getMethod() == "GET")
                        return (Poco::Net::HTTPRequestHandler*) new ReservationGetHandler();
                    else if(request.getURI() == "/reservations.html" && request.getMethod() == "POST")
                        return (Poco::Net::HTTPRequestHandler*) new ReservationPostHandler();
    }
};


#endif //SERVERAPP_REQUESTHANDLER_H
