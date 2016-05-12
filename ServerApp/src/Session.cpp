//
// Created by chmielok on 5/5/16.
//
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "Session.h"
#include "../../Shared/Message.h"

void Session::run() {
    // perform necessary SSL operations
    initializeSSLBIO();
    SSLHandshake();

    int rval;
    Message buf;

    //odczytaj Message od klienta
    if ((rval = SSL_read(ssl, &buf, sizeof (Message) - 1)) == -1)
        perror("reading stream message");
    if (rval == 0)
        printf("Ending connection\n");
    handleMessage(buf);

    char buff[20];
    std::cout << "Hello! Incoming connection from: " << inet_ntop(AF_INET, &ip4Address, buff, 20) << std::endl;
    Message a;
    a.messageType = Message::MessageType::BOOKING;
    Message::MessageData::LoggingMessage loggingMessage;
    strcpy(loggingMessage.login, "aaa");
    strcpy(loggingMessage.password, "bbb");
    a.messageData.loggingMessage = loggingMessage;
    std::cout << "Struct has been made and destroyed!" <<endl;

    close(socket);
    delete this;
}

void Session::initializeSSLBIO() {
    bio = BIO_new(BIO_s_socket());
    BIO_set_fd(bio, socket, BIO_NOCLOSE);
    SSL_set_bio(ssl, bio, bio);
}


void Session::SSLHandshake() {
    if(SSL_accept(ssl) != 1) {
        // @FIXME
        exit(1000);
    }
}

void Session::handleMessage(Message message) {
    switch (message.messageType) {
        case Message::MessageType::LOGGING:
            cout << "LOGGING" << endl;
            cout << "login: " << message.messageData.loggingMessage.login << endl;
            cout << "password: " << message.messageData.loggingMessage.password << endl;
            break;

        case Message::MessageType::BOOKING:
            cout << "BOOKING" << endl;
            cout << "id: " << message.messageData.bookingMessage.id << endl;
            cout << "data: " << message.messageData.bookingMessage.data << endl;
            break;

        case Message::MessageType::ACCESS_REQUEST:
            cout << "ACCESS_REQUEST" << endl;
            break;

        case Message::MessageType::FAIL:
            cout << "FAIL" << endl;
            cout << "failMessage: " << message.messageData.failMessage << endl;
            break;

        case Message::MessageType::SUCCESS:
            cout << "SUCCESS" << endl;
            cout << "successMessage: " << message.messageData.successMessage << endl;
            break;

        case Message::MessageType::MACHINE_DATA:
            cout << "MACHINE_DATA" << endl;
            cout << "id: " << message.messageData.machineDataMessage.id << endl;
            cout << "information: " << message.messageData.machineDataMessage.information << endl;
            break;

        case Message::MessageType::BOOKING_LOG:
            cout << "BOOKING_LOG" << endl;
            break;

    }
}