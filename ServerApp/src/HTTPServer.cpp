//
// Created by atos on 04.06.16.
//

#include "HTTPServer.h"

HTTPServer::HTTPServer() : Server() {
    port = FilesPaths::getInstance().getHttpsPort();
}

virtual static HTTPServer& HTTPServer::getServer()
{
    static HTTPServer server;
    return server;
}