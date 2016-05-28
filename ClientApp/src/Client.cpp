//
// Created by tomasz on 10.05.16.
//

#include "Client.h"
#include "../../Shared/Message.h"

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <cstring>
#include <climits>


using namespace std;

bool Client::running = false;
int Client::port = 8765;
string Client::hostname = "localhost";

Client::~Client() {
    if (running)
        close(clientSocket);
    SSL_CTX_free(sslctx);
}

Client &Client::getClient() {
    static Client client;
    return client;
}

void Client::runClient() {
    try {
        prepare();
        sendData();
    } catch (ClientException e) {
        cerr << "FAILURE. Source: ";
        switch (e.errorCode) {
            case ClientException::ErrorCode::CONNECT_FAILURE:
                cerr << "Connect failure";
                break;
            case ClientException::ErrorCode::NO_SERVER:
                cerr << "No server found";
                break;
            case ClientException::ErrorCode::SOCKET_FAILURE:
                cerr << "Socket creation failure";
                break;
        }
        cerr << endl;
        close(clientSocket);
    }
}

void Client::prepare(){
    // Initialize SSL configuration and variables
    initializeSSL();
    initializeSSL_CTX();
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
        throw ClientException(ClientException::ErrorCode::SOCKET_FAILURE);
    running = true;
    server_addr.sin_family = AF_INET;
    server_hostent = gethostbyname(hostname.c_str());
    if (server_hostent == (struct hostent *) 0)
        throw ClientException(ClientException::ErrorCode::NO_SERVER);

    memcpy((char *) &server_addr.sin_addr, (char *) server_hostent->h_addr, server_hostent->h_length);

    server_addr.sin_port = htons(port);

    if (connect(clientSocket, (struct sockaddr *) &server_addr, sizeof server_addr) != 0) {
        throw ClientException(ClientException::ErrorCode::CONNECT_FAILURE);
    }


    initializeSSL_BIO();
    if(SSL_connect(ssl) == -1) {
        ERR_print_errors_fp(stderr);
        exit(1000);
    }
}

void Client::sendData() {

    Message message;
    int c;
    cout<<"Co chcesz wyslac"<<endl;
    cout<<"1 - LOGGING"<<endl;
    cout<<"2 - BOOKING"<<endl;
    cout<<"3 - ACCESS_REQUEST"<<endl;
    cout<<"4 - FAIL"<<endl;
    cout<<"5 - SUCCESS"<<endl;
    cout<<"6 - MACHINE_DATA"<<endl;
    cout<<"7 - BOOKING_LOG"<<endl;
    cin >> c;
    switch (c){
        case MessageType::LOGGING:
            message.messageType = MessageType::LOGGING;
            Message::MessageData::LoggingMessage loggingMessage;
            cout<<"Podaj login"<<endl;
            char login[32];
            char password[20];
            cin >> login;
            cout<<"Podaj haslo"<<endl;
            cin >> password;
            strcpy(loggingMessage.login ,login);
            strcpy(loggingMessage.password, password);
            message.messageData.loggingMessage = loggingMessage;
            break;

        case MessageType::BOOKING:
            message.messageType = MessageType::BOOKING;
            Message::MessageData::BookingMessage bookingMessage;
            cout<<"Podaj id"<<endl;
            uint32_t id;
            cin >> id;
            bookingMessage.id = id;
            time(&bookingMessage.data);
            message.messageData.bookingMessage = bookingMessage;
            break;

        case MessageType::ACCESS_REQUEST:
            message.messageType = MessageType::ACCESS_REQUEST;
            break;

        case MessageType::FAIL:
            message.messageType = MessageType::FAIL;
            cout<<"Podaj wiadomosc bledu"<<endl;
            char failMessage[64];
            cin >> failMessage;
            strcpy(message.messageData.failMessage ,failMessage);
            break;

        case MessageType::SUCCESS:
            message.messageType = MessageType::SUCCESS;
            cout<<"Podaj wiadomosc sukcesu"<<endl;
            char successMessage[64];
            cin >> successMessage;
            strcpy(message.messageData.successMessage ,successMessage);
            break;

        case MessageType::MACHINE_DATA:
            message.messageType = MessageType::MACHINE_DATA;
            Message::MessageData::MachineDataMessage machineDataMessage;
            cout<<"Podaj id"<<endl;
            cin >> id;
            cout<<"Podaj informacje"<<endl;
            char information[32];
            cin >> information;
            strcpy(machineDataMessage.information ,information);
            machineDataMessage.id = id;
            message.messageData.machineDataMessage  = machineDataMessage;
            break;

        case MessageType::BOOKING_LOG:
            message.messageType = MessageType::BOOKING_LOG;
            break;
        default:
            cout<<"Nie poprawna wartość"<<endl;
            return;

    }
    void* pointer = (void*) &message;
    if (SSL_write(ssl, pointer, sizeof (Message)) == 0)
        ERR_print_errors_fp(stderr);
}

void Client::initializeSSL() {
    SSL_library_init();

    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

}

void Client::initializeSSL_CTX() {
    sslctx = SSL_CTX_new(SSLv3_client_method());
    if(sslctx == NULL)
    {
        ERR_print_errors_fp(stderr);
        exit(1);
    }
    // Load OpenSSL cerrificate file
    if(SSL_CTX_use_certificate_file(sslctx, certPath, SSL_FILETYPE_PEM) <= 0){
        ERR_print_errors_fp(stderr);
        exit(1);
    }
    // Load OpenSSL Private Key
    if(SSL_CTX_use_PrivateKey_file(sslctx, keyPath, SSL_FILETYPE_PEM) <= 0){
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    /* verify private key */
    if (!SSL_CTX_check_private_key(sslctx))
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }

    ssl = SSL_new(sslctx);
}

void Client::initializeSSL_BIO() {
    SSL_set_fd(ssl, clientSocket);
}

bool Client::setServerPortAndName(int p, string name){
    if(!running){
        port = p;
        hostname = name;
    }
    else
        return false;
    return true;
}

bool Client::setServerPortAndName(){
    //todo - pobranie z pliku konfiguracyjnego
    return true;
}