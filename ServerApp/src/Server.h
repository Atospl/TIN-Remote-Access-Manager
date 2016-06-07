//
// Created by chmielok on 5/3/16.
//

#ifndef TIN_REMOTE_ACCESS_MANAGER_SERVER_H
#define TIN_REMOTE_ACCESS_MANAGER_SERVER_H

#include <iostream>
#include <string>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../../Shared/Message.h"
#include "FilesPaths.h"

using namespace std;

class Server {
public:
    // zmienia domyślny port dla serwera
    static bool setDefaultPort(int);
    // zmienia liczbę możliwych oczekujących połączeń
    static bool setMaxWaitingConns(int);
    // pobranie z pliku konfiguracyjnego
    static bool setServerPortAndName();
    // pobierz instancję serwera (singleton)
    static Server& getServer();
    // uruchom serwer
    void runServer();
    virtual ~Server();

    /** Validate user's password */
    bool verifyUser(const char *login, const char *password);

private:
    // zablokowany konstruktor domyślny (singleton)
    Server();
    // nasłuchuj i twórz wątki
    void listenForClients();
    // przygotuj serwer pod przyjmowanie klientów
    void prepare();

    /** hashes given string */
    string sha512(string password);
    // converts char to hex
    string to_hex(unsigned char s);

    // initialize openSSL library
    void initializeSSL();
    // initializes SSL conntext parameters
    void initializeSSL_CTX();

    static bool running;
    static int port;
    static int maxWaitingConns;
    int serverSocket;

    // SSL parameters
    SSL_CTX *sslctx;

    int certfd;
    int keyfd;

    char *certPath;
    char *keyPath;
    char *caCertPath;

};


#endif //TIN_REMOTE_ACCESS_MANAGER_SERVER_H
