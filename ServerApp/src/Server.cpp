//
// Created by Paweł Chmielak on 5/3/16.
//

#include "Server.h"
#include "Session.h"
#include "../../Shared/Message.h"
#include <sys/types.h>
#include <sys/socket.h>
//#include <openssl/applink.c>

#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

using namespace std;

bool Server::running = false;
int Server::port = 8080;
int Server::maxWaitingConns = 10;

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
        cerr << "FAILURE. Source: ";
        switch (e.errorCode) {
            case ServerException::ErrorCode::SOCKET_FAILURE:
                cerr << "Socket creation failure";
                break;
            case ServerException::ErrorCode::BIND_FAILURE:
                cerr << "Socket binding failure";
                break;
            case ServerException::ErrorCode::LISTEN_FAILURE:
                cerr << "Making socket passive failure";
                break;
            case ServerException::ErrorCode::ACCEPT_FAILURE:
                cerr << "Connection acceptance failure";
                break;
        }
        cerr << endl;
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
    initializeSSL();

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
    status = listen(serverSocket, maxWaitingConns);
    if (status < 0)
        throw ServerException(ServerException::ErrorCode::LISTEN_FAILURE);
}

void Server::listenForClients() {
    int socket;
    sockaddr_in address_info;
    socklen_t data_length = sizeof(sockaddr_in);

    int rval;
    Message buf;

    // czekaj na kolejne połączenia
    while (0 < (socket = accept(serverSocket, (sockaddr*)&address_info, &data_length)) ) {
        Session * session;
        session = new Session(socket, address_info.sin_addr.s_addr);
        //odczytaj Message od klienta
        if ((rval = read(socket,&buf, sizeof (Message))) == -1)
            perror("reading stream message");
        if (rval == 0)
            printf("Ending connection\n");
        handleMessage(buf);

        thread sessionThread(&Session::run, session); // utwórz wątek dla nowego połączenia
        sessionThread.join(); // uruchom wątek
    }
    cerr << errno << endl;
    throw ServerException(ServerException::ErrorCode::ACCEPT_FAILURE);
}

void Server::initializeSSL() {
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
}

void Server::initializeSSL_CTX() {
    sslctx = SSL_CTX_new(SSLv23_server_method());
    // create new key every time
    SSL_CTX_set_options(sslctx, SSL_OP_SINGLE_DH_USE);
    // set up server certificate file
    certfd = SSL_CTX_use_certificate_file(sslctx, certPath, SSL_FILETYPE_PEM);
    // set up server private key file
    keyfd = SSL_CTX_use_PrivateKey_file(sslctx, keyPath, SSL_FILETYPE_PEM);
}

void Server::handleMessage(Message message) {
    switch (message.messageType){
        case Message::MessageType::LOGGING:{
            cout<<"Logging data"<<endl;
            cout<<"login: "<<message.messageData.loggingMessage.login<<endl;
            cout<<"password: "<<message.messageData.loggingMessage.password<<endl;
            break;
        }
        case Message::MessageType::BOOKING:{
            cout<<"BOOKING"<<endl;
            cout<<"id: "<<message.messageData.bookingMessage.id<<endl;
            cout<<"data: "<<message.messageData.bookingMessage.data<<endl;
            break;
        }
        case Message::MessageType::ACCESS_REQUEST:{
            cout<<"ACCESS_REQUEST"<<endl;
            break;
        }
        case Message::MessageType::FAIL:{
            cout<<"FAIL"<<endl;
            cout<<"failMessage: "<<message.messageData.failMessage<<endl;
            break;
        }
        case Message::MessageType::SUCCESS:{
            cout<<"SUCCESS"<<endl;
            cout<<"successMessage: "<<message.messageData.successMessage<<endl;
            break;
        }
        case Message::MessageType::MACHINE_DATA:{
            cout<<"MACHINE_DATA"<<endl;
            cout<<"id: "<<message.messageData.machineDataMessage.id<<endl;
            cout<<"information: "<<message.messageData.machineDataMessage.information<<endl;
            break;
        }
        case Message::MessageType::BOOKING_LOG:{
            cout<<"BOOKING_LOG"<<endl;
            break;
        }
    }
}


