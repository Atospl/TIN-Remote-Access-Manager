//
// Created by chmielok on 5/5/16.
//
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <iomanip>
#include <sstream>
#include <openssl/err.h>
#include <openssl/sha.h>
#include "Session.h"
#include "Exception.h"
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
            handleLoginMessage(message.messageData.loggingMessage.login,
                               message.messageData.loggingMessage.password);
            break;

        case MessageType::BOOKING:
            handleBookingRequestMessage(message.messageData.bookingMessage.id,
                                        message.messageData.bookingMessage.date);
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


        case MessageType::BOOKING_LOG:
            handleBookingLogRequestMessage();
            break;

        case MessageType::LOGGING_OUT:
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
            return true;
// TODO poprawić funkcję haszującą
//            if (i.passHash.compare(hash) == 0) {
//                message.messageType = MessageType::SUCCESS;
//                void* pointer = (void*) &message;
//                if (SSL_write(ssl, pointer, sizeof (Message)) == 0)
//                    ERR_print_errors_fp(stderr);
//                return true;
//            }
    }
    return false;
}

void Session::handleBookingRequestMessage(uint32_t id, time_t date) {
    Message message;

    if (verified) {
        bool result = FileController::getInstance().addReservation(userLogin, id, date);

        if (result) {
            message.messageType = MessageType::SUCCESS;
            strcpy(message.messageData.failMessage, "Reservation added successfully.");
        } else {
            message.messageType = MessageType::FAIL;
            strcpy(message.messageData.failMessage, "Reservation failed. Please choose another date.");
        }
    } else {
        message.messageType = MessageType::FAIL;
        strcpy(message.messageData.failMessage, "User not verified.");
    }

    sendData(message);
}

void Session::handleAccessRequestMessage() {
    Message message;

    if (verified) {
        unsigned int minutes = checkAvailableTime();

        if (minutes != 0) {
            char buf[16];
            //IptablesController::grantLimitedAccess(inet_ntop(AF_INET, &ip4Address, buf, 16), minutes);
            message.messageType = MessageType::SUCCESS;
            string temp = "Access granted for " + to_string(minutes) + " minutes";
            strcpy(message.messageData.successMessage, temp.c_str());
        } else {
            message.messageType = MessageType::FAIL;
            strcpy(message.messageData.failMessage, "Access forbidden. You don't have a reservation for this time.");
        }
    } else {
        message.messageType = MessageType::FAIL;
        strcpy(message.messageData.failMessage, "User not verified.");
    }

    sendData(message);
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
    Message message;

    if (verified = verifyUser(login, password)) {
        message.messageType = MessageType::SUCCESS;
        strcpy(message.messageData.successMessage, "Logging successful");
        userLogin = login;
    } else {
        message.messageType = MessageType::FAIL;
        strcpy(message.messageData.failMessage, "Bad credentials");
    }

    sendData(message);
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
    ss << setfill('0') << setw(2) << hex << (int) s;
    return ss.str();
}

void Session::sendData(Message message) {
    if (SSL_write(ssl, &message, sizeof (Message)) == 0)
        throw SessionException(SessionException::ErrorCode::SSL_ERROR);
}

unsigned int Session::checkAvailableTime() {
    auto wholeList = FileController::getInstance().getReservations();
    vector<reservation> personalList;

    for (auto itr : wholeList) { // get reservations by user's login
        if (itr.userLogin == userLogin)
            personalList.push_back(itr);
    }

    time_t currentTime = time(nullptr);
    time_t timeHourAgo = currentTime - (60/*seconds*/ * 60/*minutes*/);

    for (auto itr : personalList) {
        if (itr.date > timeHourAgo && itr.date < currentTime) {
            unsigned int minutes = (itr.date + (60 * 60)) - currentTime;
            minutes /= 60;
            return minutes;
        }
    }

    return 0;
}















