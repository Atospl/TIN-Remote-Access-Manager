//
// Created by chmielok on 5/5/16.
//
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include "Session.h"
#include "../../Shared/Message.h"
#include "FileController.h"

void Session::run() {
    // perform necessary SSL operations
    initializeSSLBIO();
    SSLHandshake();

    int rval;
    Message buf;
    int ssl_error;
    try {
        while (1) {
            if ((rval = SSL_read(ssl, &buf, sizeof(Message))) == 0)
        //sprawdzic czy czyta wszystkie bajty
                ERR_print_errors_fp(stderr);

            switch (ssl_error = SSL_get_error(ssl, rval)) {
                case SSL_ERROR_NONE:
                    cout << "SSL_ERROR_NONE" << endl;
                    break;

                case SSL_ERROR_ZERO_RETURN:
                    //connection closed by client, clean up
                    throw SessionException(SessionException::ErrorCode::SSL_ZERO_RETURN);

                case SSL_ERROR_WANT_READ:
                    //the operation did not complete, block the read
                    throw SessionException(SessionException::ErrorCode::SSL_WANT_READ);

                case SSL_ERROR_WANT_WRITE:
                    //the operation did not complete
                    throw SessionException(SessionException::ErrorCode::SSL_WANT_WRITE);

                case SSL_ERROR_SYSCALL:
                    //some I/O error occured (could be caused by false start in Chrome for instance), disconnect the client and clean up
                    cout << "SSL_ERROR_SYSCALL" << endl;
                    throw SessionException(SessionException::ErrorCode::SSL_SYSCALL);
                default:
                    cout << "default" << endl;
            }
            if (rval > 0)
                handleMessage(buf);
        }
    }
    catch (SessionException e) {
        cout<<"Closing " << endl;
        close(clientSocket);
    }
}

void Session::initializeSSLBIO() {
    SSL_set_fd(ssl, clientSocket);
}


void Session::SSLHandshake() {
//    SSL_set_accept_state(ssl);
    if (SSL_accept(ssl) == -1) {
        ERR_print_errors_fp(stderr);
        exit(1000);
    }
}

void Session::handleMessage(Message message) {
    switch (message.messageType) {
        case MessageType::LOGGING:
            cout << "LOGGING" << endl;
            cout << "login: " << message.messageData.loggingMessage.login << endl;
            cout << "password: " << message.messageData.loggingMessage.password << endl;
            verifyUser(message);
            break;

        case MessageType::BOOKING:
            cout << "BOOKING" << endl;
            cout << "id: " << message.messageData.bookingMessage.id << endl;
            cout << "data: " << message.messageData.bookingMessage.data << endl;
            break;

        case MessageType::ACCESS_REQUEST:
            cout << "ACCESS_REQUEST" << endl;
            break;

        case MessageType::FAIL:
            cout << "FAIL" << endl;
            cout << "failMessage: " << message.messageData.failMessage << endl;
            break;

        case MessageType::SUCCESS:
            cout << "SUCCESS" << endl;
            cout << "successMessage: " << message.messageData.successMessage << endl;
            break;

        case MessageType::MACHINE_DATA:
            cout << "MACHINE_DATA" << endl;
            cout << "id: " << message.messageData.machineDataMessage.id << endl;
            cout << "information: " << message.messageData.machineDataMessage.information << endl;
            break;

        case MessageType::BOOKING_LOG:
            cout << "BOOKING_LOG" << endl;
            break;
        case MessageType::LOGGING_OFF:
            cout << "LOGGING_OFF" << endl;
            throw SessionException(SessionException::ErrorCode::LOGGING_OFF);
    }
}

bool Session::verifyUser(Message message) {
    /*vector<client> clients = FileController::getInstance().getClients();
    unsigned char digest[SHA512_DIGEST_LENGTH];

    *//** @TODO SHA512 hash creation - move to separate function *//*
    SHA512((unsigned char*)&message.messageData.loggingMessage.password,
           strlen(message.messageData.loggingMessage.password),
           (unsigned char*)&digest);
    char hexDigest[SHA512_DIGEST_LENGTH*2+1];
    for(int i=0; i < SHA512_DIGEST_LENGTH; ++i)
        sprintf(&hexDigest[i*2], "%02x", (unsigned int)digest[i]);
    *//***********************************************************//*

    for(auto i : clients) {
        if (i.login == message.messageData.loggingMessage.login)
            if (i.passHash.compare(hexDigest) == 0) {
                return true;
            }
        else
            return false;
    }
    return false;*/
    message.messageType = MessageType::SUCCESS;
    void* pointer = (void*) &message;
    if (SSL_write(ssl, pointer, sizeof (Message)) == 0)
        ERR_print_errors_fp(stderr);
}