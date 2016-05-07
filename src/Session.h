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
    Session(int s, int adr) : socket(s), ip4Address(adr) {};
    void run();

private:
    int socket;
    int ip4Address;
};


#endif //TIN_REMOTE_ACCESS_MANAGER_SESSION_H
