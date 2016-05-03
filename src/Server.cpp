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
#include <cstring>

using namespace std;

Server::Server() : name((char*)"Brutal anal penetrator"), port(80) {

}

char* Server::getName() {
    return name;
}

void Server::runServer() {
    serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketDescriptor < 0) {
        cout << "Pierdol sie, sockecie";
        return;
    }
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(8080);
    int status = bind(serverSocketDescriptor, (sockaddr*) &address, sizeof(address));
    if (status < 0) {
        cout << "Pierdol sie, bindzie";
        return;
    }
    status = listen(serverSocketDescriptor, 5);
    if (status < 0) {
        cout << "Pierdol sie, listenie";
        return;
    }
    sockaddr_in returnAddress;
    socklen_t addressLength;
    socketDescriptor = accept(serverSocketDescriptor, (sockaddr*) 0, (socklen_t *) 0);
    if (socketDescriptor < 0) {
        cout << "Pierdol sie, accepcie";
        return;
    }
    char buffer[1024] = "Hello client!\n";
    write(socketDescriptor, buffer, strlen(buffer));
//    status = recv(socketDescriptor, buffer, 1024, 0);
//    if (status < 0) {
//        cout << "Pierdol sie, recvie";
//        close(socketDescriptor);
//        close(serverSocketDescriptor);
//        return;
//    }
//    cout << buffer;

    close(socketDescriptor);
    close(serverSocketDescriptor);

}