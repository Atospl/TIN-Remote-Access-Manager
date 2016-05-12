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


    char buf[20];
    std::cout << "Hello! Incoming connection from: " << inet_ntop(AF_INET, &ip4Address, buf, 20) << std::endl;
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
        exit(1);
    }
}
