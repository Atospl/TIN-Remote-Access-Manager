//
// Created by chmielok on 5/5/16.
//

#ifndef TIN_REMOTE_ACCESS_MANAGER_SESSION_H
#define TIN_REMOTE_ACCESS_MANAGER_SESSION_H

#include <iostream>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace std;

class Session {
public:
    Session(int s, int adr, SSL_CTX *ctx) : socket(s), ip4Address(adr) {
        ssl = SSL_new(ctx);
    };
    void run();

private:
    // initializes openssl BIO structure
    void initializeSSLBIO();
    // performs server-side handshake
    void SSLHandshake();

    int socket;
    int ip4Address;
    // SSL parameters
    SSL *ssl;
    BIO *bio;
};


#endif //TIN_REMOTE_ACCESS_MANAGER_SESSION_H
