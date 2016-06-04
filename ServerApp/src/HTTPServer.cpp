//
// Created by atos on 04.06.16.
//
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>

#include "HTTPServer.h"
#include "HTTPSession.h"
#include "Exception.h"

HTTPServer::HTTPServer() : Server()
{
    port = FilesPaths::getInstance().getHttpsPort();
}

virtual static HTTPServer& HTTPServer::getServer()
{
    static HTTPServer server;
    return server;
}

void HTTPServer::listenForClients()
{
    int clientSocket;
    sockaddr_in address_info;
    socklen_t data_length = sizeof(sockaddr_in);

    Message buf;

    // czekaj na kolejne połączenia
    while (0 < (clientSocket = accept(serverSocket, (sockaddr*)&address_info, &data_length)) ) {
        HTTPSession * session;
        session = new HTTPSession(clientSocket, address_info.sin_addr.s_addr, sslctx);

        //odczytaj Message od klienta
        thread sessionThread(&HTTPSession::run, session); // utwórz wątek dla nowego połączenia
        sessionThread.join(); // uruchom wątek
    }
    cerr << errno << endl;
    throw ServerException(ServerException::ErrorCode::ACCEPT_FAILURE);
}