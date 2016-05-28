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
    ~Session() {
        SSL_free(ssl);
        close(clientSocket);
    }
    void run();

private:
    // initializes openssl BIO structure
    void initializeSSLBIO();
    // performs server-side handshake
    void SSLHandshake();

    bool verifyUser(Message message);
    //Handles client's message
    void handleMessage(Message message);

    int clientSocket;
    int serverSocket;
    int ip4Address;
    // SSL parameters
    SSL *ssl;
};


#endif //TIN_REMOTE_ACCESS_MANAGER_SESSION_H
