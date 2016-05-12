//
// Created by tomasz on 10.05.16.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

using namespace std;

class Client {
public:
    static bool setServerPortAndName(int port, string name);
    static Client & getClient();
    void connectWithMainServer();
    void runClient();
    ~Client();

private:
    // zablokowany konstruktor domyślny (singleton)
    Client(){};

    // przygotuj klienta
    void prepare();
    void sendData();
    // initialize openSSL library
    void initializeSSL();
    // initialize SSL context
    void initializeSSL_CTX();
    // initialize SSL bio structure
    void initializeSSL_BIO();
    int clientSocket;
    static string hostname;
    static bool running;
    static int port;
    struct sockaddr_in server_addr;
    struct hostent *server_hostent;

    // SSL parameters
    SSL_CTX *sslctx;
    SSL *ssl;
    BIO *bio;
    int certfd;
    int keyfd;

    static constexpr char *certPath = (char*)".ssl/cert.pem";
    static constexpr char *keyPath = (char*)".ssl/key.pem";

    struct ClientException {
        enum ErrorCode {
            SOCKET_FAILURE,
            NO_SERVER,
            CONNECT_FAILURE
        } errorCode;
        ClientException(ErrorCode code) : errorCode(code) {};
    };
};






#endif //CLIENT_CLIENT_H
