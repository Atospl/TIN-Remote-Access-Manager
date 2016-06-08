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
    Session(int cls, int adr, SSL_CTX *ctx) : clientSocket(cls), ip4Address(adr), verified(false) {
        ssl = SSL_new(ctx);
    };
    Session() {}
    virtual ~Session() {
        SSL_free(ssl);
        close(clientSocket);
    }
    virtual void run();

protected:
    // initializes openssl BIO structure
    void initializeSSLBIO();
    // performs server-side handshake
    void SSLHandshake();

    void sendData(Message);

    unsigned int checkAvailableTime();

private:
    //Handles client's message
    void handleMessage(Message message);

    void handleAccessRequestMessage();

    void handleBookingLogRequestMessage();

    void handleBookingRequestMessage(uint32_t id, time_t data);

    void handleSuccessMessage(char *successMessage);

    void handleFailMessage(char *failMessage);

    void handleLoginMessage(char *login, char *password);




    int clientSocket;
    int ip4Address;
    bool verified;
    string userLogin;

    // SSL parameters
    SSL *ssl;

};


#endif //TIN_REMOTE_ACCESS_MANAGER_SESSION_H
