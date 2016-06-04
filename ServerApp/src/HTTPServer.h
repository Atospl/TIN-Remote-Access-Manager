//
// Created by atos on 04.06.16.
//

#ifndef SERVERAPP_HTTPSERVER_H
#define SERVERAPP_HTTPSERVER_H

#include "Server.h"

class HTTPServer : public Server
{
public:
    /** Returns HTTPS Server instance */
    virtual static HTTPServer& getServer();

private:
    /** Singleton's private constructor. Imports certificates, keys and port number from file */
    HTTPServer();
    /** Wait for clients connection, create new session afterwards */
    void listenForClients();
};


#endif //SERVERAPP_HTTPSERVER_H
