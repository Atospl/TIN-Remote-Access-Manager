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
#include <unistd.h>

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
        logIn();
        getDataToTransfer();
    } catch (ClientException e) {
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
            case ClientException::ErrorCode::LOGGING_IN_FAILURE:
                cerr << "Logging in failure";
                break;
            case ClientException::ErrorCode::LOGGING_OFF:
                cerr << "Logging out" << endl;
                logOut();
                break;
        }
        cerr << "Closing" << endl;
        close(clientSocket);
        exit(1);
    }
}

void Client::prepare() {
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
    if (SSL_connect(ssl) == -1) {
        ERR_print_errors_fp(stderr);
        exit(1000);
    }
}

void Client::getDataToTransfer() {

    int c;
    while (1) {
        cout << "Co chcesz wyslac" << endl;
        cout << "1 - BOOKING" << endl;
        cout << "2 - ACCESS_REQUEST" << endl;
        cout << "3 - FAIL" << endl;
        cout << "4 - SUCCESS" << endl;
        cout << "5 - MACHINE_DATA" << endl;
        cout << "6 - BOOKING_LOG" << endl;
        cout << "7 - LOGGING_OUT" << endl;
        cin >> c;
        switch (c) {
            case MessageType::BOOKING:
                sendBookingRequestMessage();;
                break;

            case MessageType::ACCESS_REQUEST:
                sendAccessRequestMessage();
                break;

            case MessageType::FAIL:
                sendFailMessage();
                break;

            case MessageType::SUCCESS:
                sendSuccessMessage();
                break;

            case MessageType::MACHINE_DATA:
                sendMachineDataRequestMessage();
                break;

            case MessageType::BOOKING_LOG:
                sendBookingLogRequestMessage();
                break;
            case MessageType::LOGGING_OFF:
                throw ClientException(ClientException::LOGGING_OFF);
            default:
                cout << "Nie poprawna wartość" << endl;
                return;

        }
    }
}

void Client::initializeSSL() {
    SSL_library_init();

    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

}

void Client::initializeSSL_CTX() {
    sslctx = SSL_CTX_new(SSLv23_client_method());
    if (sslctx == NULL) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }
    // Load OpenSSL cerrificate file
    if (SSL_CTX_use_certificate_file(sslctx, certPath, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }
    // Load OpenSSL Private Key
    if (SSL_CTX_use_PrivateKey_file(sslctx, keyPath, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    /* verify private key */
    if (!SSL_CTX_check_private_key(sslctx)) {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }

    ssl = SSL_new(sslctx);
}

void Client::initializeSSL_BIO() {
    SSL_set_fd(ssl, clientSocket);
}

bool Client::setServerPortAndName(int p, string name) {
    if (!running) {
        port = p;
        hostname = name;
    }
    else
        return false;
    return true;
}

bool Client::setServerPortAndName() {
    //todo - pobranie z pliku konfiguracyjnego
    return true;
}

void Client::logIn() {
    Message message;
    char login[32];
    message.messageType = MessageType::LOGGING;
    Message::MessageData::LoggingMessage loggingMessage;
    cout << "Podaj login" << endl;
    cin.getline(login, 32);
    char *password = getpass("Password: ");
    strcpy(loggingMessage.login, login);
    strcpy(loggingMessage.password, password);
    message.messageData.loggingMessage = loggingMessage;

    void *pointer = (void *) &message;
    if (SSL_write(ssl, pointer, sizeof(Message)) == 0)
        ERR_print_errors_fp(stderr);

    if (SSL_read(ssl, &message, sizeof(Message)) == 0)
        throw ClientException(ClientException::ErrorCode::LOGGING_IN_FAILURE);

    if (message.messageType == SUCCESS) {
        cout << "Success" << endl;
    }
    else if (message.messageType == FAIL) {
        cerr << "Login FAIL" << endl;
        throw ClientException(ClientException::LOGGING_IN_FAILURE);
    }
    else {
        cerr << "Wrong messegeType" << endl;
        throw ClientException(ClientException::LOGGING_IN_FAILURE);
    }

}

void Client::logOut() {
    Message message;
    message.messageType = LOGGING_OFF;
    sendData(message);
}

void Client::sendData(Message message) {
    void *pointer = (void *) &message;
    if (SSL_write(ssl, pointer, sizeof(Message)) == 0)
        ERR_print_errors_fp(stderr);
}

void Client::sendBookingRequestMessage() {
    Message message;
    message.messageType = MessageType::BOOKING;
    Message::MessageData::BookingMessage bookingMessage;
    cout << "Podaj id" << endl;
    uint32_t id;
    cin >> id;
    bookingMessage.id = id;
    time(&bookingMessage.data);
    message.messageData.bookingMessage = bookingMessage;
    sendData(message);
}

void Client::sendAccessRequestMessage() {
    Message message;
    message.messageType = MessageType::ACCESS_REQUEST;
    sendData(message);
}

void Client::sendBookingLogRequestMessage() {
    Message message;
    message.messageType = MessageType::BOOKING_LOG;
    sendData(message);
}

void Client::sendMachineDataRequestMessage() {
    Message message;
    uint32_t id;
    message.messageType = MessageType::MACHINE_DATA;
    Message::MessageData::MachineDataMessage machineDataMessage;
    cout << "Podaj id" << endl;
    cin >> id;
    cout << "Podaj informacje" << endl;
    char information[32];
    cin >> information;
    strcpy(machineDataMessage.information, information);
    machineDataMessage.id = id;
    message.messageData.machineDataMessage = machineDataMessage;
    sendData(message);
}

void Client::sendSuccessMessage() {
    Message message;
    message.messageType = MessageType::SUCCESS;
    cout << "Podaj wiadomosc sukcesu" << endl;
    char successMessage[64];
    cin >> successMessage;
    strcpy(message.messageData.successMessage, successMessage);
    sendData(message);
}

void Client::sendFailMessage() {
    Message message;
    message.messageType = MessageType::FAIL;
    cout << "Podaj wiadomosc bledu" << endl;
    char failMessage[64];
    cin >> failMessage;
    strcpy(message.messageData.failMessage, failMessage);
    sendData(message);
}











