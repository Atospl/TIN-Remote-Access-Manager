//
// Created by chmielok on 5/3/16.
//

#include "Server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

Server::Server() : name((char*)"WebServer"), port(8080) {

}

char* Server::getName() {
    return name;
}

void Server::runServer() {
    running = true;
    cout << "Weszlo1" << endl;
    serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketDescriptor < 0) {
        cout << "Niech cie, sockecie";
        return;
    }
    int enable = 1;
    setsockopt(serverSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(8080);
    int status = bind(serverSocketDescriptor, (sockaddr*) &address, sizeof(address));
    cout << "Weszlo2" << endl;
    if (status < 0) {
        cout << "Niech cie, bindzie";
        return;
    }
    status = listen(serverSocketDescriptor, 5);
    cout << "Weszlo3" << endl;
    if (status < 0) {
        cout << "Niech cie, listenie";
        return;
    }
    socketDescriptor = accept(serverSocketDescriptor, (sockaddr*) 0, (socklen_t *) 0);
    for (int x = 0; x<10000; ++x);
    cout << "Weszlo4" << endl;
    if (socketDescriptor < 0) {
        cout << "Niech cie, accepcie" << endl;
        return;
    }
    char* buffer;
    buffer = new char[1024];
    cout << "Weszlo5" << endl;
    status = read(socketDescriptor, buffer, 1024);
    if (status < 0) {
        cout << "Niech cie, recvie";
        close(socketDescriptor);
        close(serverSocketDescriptor);
        return;
    }
    cout << "Wiadomosc: " << buffer << endl;
    delete[] buffer;
    close(socketDescriptor);
    close(serverSocketDescriptor);

}