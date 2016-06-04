//
// Created by atos on 04.06.16.
//

#ifndef SERVERAPP_REQUESTHANDLER_H
#define SERVERAPP_REQUESTHANDLER_H

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include "HTTPSRequestHandler.h"

class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
public:
    RequestHandlerFactory()
    {}

    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request)
    {
        if (request.getURI() == "/")
            return (Poco::Net::HTTPRequestHandler*) new RootHandler();
//        else
//            return new DataHandler();
    }
};


#endif //SERVERAPP_REQUESTHANDLER_H
