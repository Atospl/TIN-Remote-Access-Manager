//
// Created by chmielok on 5/5/16.
//
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <sstream>
#include <openssl/err.h>
#include <openssl/sha.h>
#include "Session.h"
#include "../../Shared/Message.h"
#include "FileController.h"
#include "IptablesController.h"

void Session::run() {
    // perform necessary SSL operations
    initializeSSLBIO();
    SSLHandshake();

    Message buf;
    char* pBuf = (char *)&buf;
    short bytesToRead = sizeof(Message);

    int bytesRead = 0;
    int readValue = 0;

    try {
        while (true) {
            readValue = SSL_read(ssl, pBuf, bytesToRead);

            if (readValue > 0) { // something was read, check if it is enough
                bytesRead += readValue;
                if (bytesRead == sizeof(Message)) {
                    handleMessage(buf);
                    pBuf = (char *)&buf;
                    bytesToRead = sizeof(Message);
                    bytesRead = 0;
                } else {
                    bytesToRead = sizeof(Message) - bytesRead;
                    pBuf = (char *)&buf + bytesRead;
                }
            } else { // an error occured
                int errorNumber = SSL_get_error(ssl, readValue);
                if (errorNumber == SSL_ERROR_WANT_READ)
                    continue;
                else
                    throw SessionException(SessionException::ErrorCode::SSL_ERROR, errorNumber);
            }

        }

    } catch (SessionException e){
        if (e.errorCode == SessionException::ErrorCode::SSL_ERROR)
            if (e.sslErrorNumber == SSL_ERROR_SYSCALL)
                cerr << "Connection ended" << endl;
            else
                cerr << "SSL error: " << e.sslErrorNumber;
        close(clientSocket);
        delete this;
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
            handleLoginMessage(message.messageData.loggingMessage.login, message.messageData.loggingMessage.password);
            break;

        case MessageType::BOOKING:
            handleBookingRequestMessage(message.messageData.bookingMessage.id, message.messageData.bookingMessage.data);
            break;

        case MessageType::ACCESS_REQUEST:
            handleAccessRequestMessage();
            break;

        case MessageType::FAIL:
            handleFailMessage(message.messageData.failMessage);
            break;

        case MessageType::SUCCESS:
            handleSuccessMessage(message.messageData.successMessage);
            break;

        case MessageType::MACHINE_DATA:
            handleMachineDataRequestMessage(message.messageData.machineDataMessage.id, message.messageData.machineDataMessage.information);
            break;

        case MessageType::BOOKING_LOG:
            handleBookingLogRequestMessage();
            break;
        case MessageType::LOGGING_OFF:
            cout << "LOGGING_OFF" << endl;
            throw SessionException(SessionException::ErrorCode::LOGGING_OFF);
    }
}

bool Session::verifyUser(char * login, char * password) {
    Message message;
    vector<client> clients = FileController::getInstance().getClients();
    string hash = sha512(string(password));

    for(auto i : clients) {
        if (i.login == login)
            if (i.passHash.compare(hash) == 0) {
                message.messageType = MessageType::SUCCESS;
                void* pointer = (void*) &message;
                if (SSL_write(ssl, pointer, sizeof (Message)) == 0)
                    ERR_print_errors_fp(stderr);
                return true;
            }
    }

    message.messageType = MessageType::FAIL;
    void* pointer = (void*) &message;
    if (SSL_write(ssl, pointer, sizeof (Message)) == 0)
        ERR_print_errors_fp(stderr);
    return false;
}

void Session::handleBookingRequestMessage(uint32_t id, time_t data) {
    cout << "BOOKING" << endl;
    cout << "id: " << id << endl;
    cout << "data: " << data << endl;

}

void Session::handleAccessRequestMessage() {
    cout << "ACCESS_REQUEST" << endl;

}

void Session::handleMachineDataRequestMessage(uint32_t id, char * information) {
    cout << "MACHINE_DATA" << endl;
    cout << "id: " << id << endl;
    cout << "information: " << information << endl;
}

void Session::handleBookingLogRequestMessage() {
    cout << "BOOKING_LOG" << endl;
}

void Session::handleSuccessMessage(char * successMessage) {
    cout << "SUCCESS" << endl;
    cout << "successMessage: " << successMessage << endl;
}

void Session::handleFailMessage(char *failMessage) {
    cout << "FAIL" << endl;
    cout << "failMessage: " << failMessage << endl;
}

void Session::handleLoginMessage(char * login, char * password ){
    cout << "LOGGING" << endl;
    cout << "login: " << login << endl;
    cout << "password: " << password << endl;
    verifyUser(login, password);
}

string Session::sha512(string password) {
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512_CTX sha256;
    SHA512_Init(&sha256);
    SHA512_Update(&sha256, password.c_str(), password.length());
    SHA512_Final(hash, &sha256);

    string output = "";
    for(int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        output += to_hex(hash[i]);
    }
    return output;
}

string Session::to_hex(unsigned char s) {
    stringstream ss;
    ss << hex << (int) s;
    return ss.str();
}











