//
// Created by Paweł Chmielak on 5/3/16.
//

#include "Server.h"
#include "Session.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

using namespace std;

bool Server::running = false;
int Server::port = 8080;
int Server::maxWaitingConns = 100;

Server::~Server() {
    if (running)
        close(serverSocket);
}

Server &Server::getServer() {
    static Server server;
    return server;
}

void Server::runServer() {
    try {
        prepare();
        listenForClients();
    } catch (ServerException e) {
        cerr << "FAILURE. Code" << e.errorCode << endl;
    }
    close (serverSocket);
}

bool Server::setDefaultPort(int p) {
    if (!running)
        port = p;
    else
        return false; // nie można zmienić portu już działającego serwera
    return true;
}

bool Server::setMaxWaitingConns(int cap) {
    if (!running)
        maxWaitingConns = cap;
    else
        return false; // nie można zmienić dla już działającego serwera
    return true;
}

void Server::prepare() {
    int status, enable = 1;

    // faza tworzenia gniazda
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
        throw ServerException(ServerException::ErrorCode::SOCKET_FAILURE);
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    running = true;

    // faza przypięcia gniazda do programu z określonym portem
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    status = bind(serverSocket, (sockaddr*) &address, sizeof(address));
    if (status < 0)
        throw ServerException(ServerException::ErrorCode::BIND_FAILURE);

    // przerobienie gniazda na pasywne (serwer)
    status = listen(serverSocket, 5);
    if (status < 0)
        throw ServerException(ServerException::ErrorCode::LISTEN_FAILURE);
}

void Server::listenForClients() {
    int socket;
    sockaddr_in address_info;
    socklen_t data_length;

    // czekaj na kolejne połączenia
    while (0 < (socket = accept(serverSocket, (sockaddr*)&address_info, &data_length)) ) {
        Session * session;
        session = new Session(socket, address_info.sin_addr.s_addr);
        thread sessionThread(&Session::run, session); // utwórz wątek dla nowego połączenia
        sessionThread.join(); // uruchom wątek
    }

    throw ServerException(ServerException::ErrorCode::ACCEPT_FAILURE);

//    socketDescriptor = accept(serverSocket, (sockaddr*) 0, (socklen_t *) 0);
//    if (socketDescriptor < 0) {
//        cout << "Niech cie, accepcie" << endl;
//        return;
//    }
//    char* buffer;
//    buffer = new char[1024];
//    cout << "Weszlo5" << endl;
//    status = read(socketDescriptor, buffer, 1024);
//    if (status < 0) {
//        cout << "Niech cie, recvie";
//        close(socketDescriptor);
//        close(serverSocket);
//        return;
//    }
//    cout << "Wiadomosc: " << buffer << endl;
//    delete[] buffer;
}











