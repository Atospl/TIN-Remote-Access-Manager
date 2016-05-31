//
// Created by chmielok on 5/5/16.
//

#ifndef TIN_REMOTE_ACCESS_MANAGER_SESSION_H
#define TIN_REMOTE_ACCESS_MANAGER_SESSION_H

#include <iostream>
#include "../../Shared/Message.h"
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "Session.h"

using namespace std;

class Session {
public:
    Session(int cls, int srvs, int adr, SSL_CTX *ctx) : clientSocket(cls), serverSocket(srvs), ip4Address(adr) {
        ssl = SSL_new(ctx);
    };
    Session() {}
    ~Session() {
        SSL_free(ssl);
        close(clientSocket);
    }
    void run();
    bool verifyUser(char *login, char *password);
private:
    // initializes openssl BIO structure
    void initializeSSLBIO();
    // performs server-side handshake
    void SSLHandshake();
    // hashes given string
    string sha512(string password);
    // converts char to hex
    string to_hex(unsigned char s);

    //Handles client's message
    void handleMessage(Message message);

    void handleAccessRequestMessage();

    void handleBookingLogRequestMessage();

    void handleBookingRequestMessage(uint32_t id, time_t data);

    void handleMachineDataRequestMessage(uint32_t id, char *information);

    void handleSuccessMessage(char *successMessage);

    void handleFailMessage(char *failMessage);

    void handleLoginMessage(char *login, char *password);



    int clientSocket;
    int serverSocket;
    int ip4Address;
    // SSL parameters
    SSL *ssl;
    struct SessionException {
        enum ErrorCode {
            LOGGING_OFF,
            SSL_ZERO_RETURN,
            SSL_WANT_READ,
            SSL_WANT_WRITE,
            SSL_SYSCALL
        } errorCode;
        SessionException(ErrorCode code) : errorCode(code) {};
    };


};


#endif //TIN_REMOTE_ACCESS_MANAGER_SESSION_H
