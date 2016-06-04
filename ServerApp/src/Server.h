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
    virtual static bool setDefaultPort(int);
    // zmienia liczbę możliwych oczekujących połączeń
    virtual static bool setMaxWaitingConns(int);
    // pobranie z pliku konfiguracyjnego
    virtual static bool setServerPortAndName();
    // pobierz instancję serwera (singleton)
    virtual static Server& getServer();
    // uruchom serwer
    virtual void runServer();
    virtual ~Server();

protected:
    // zablokowany konstruktor domyślny (singleton)
    Server();
    // nasłuchuj i twórz wątki
    virtual void listenForClients();
    // przygotuj serwer pod przyjmowanie klientów
    virtual void prepare();

    // initialize openSSL library
    virtual void initializeSSL();
    // initializes SSL conntext parameters
    virtual void initializeSSL_CTX();

    static bool running;
    static int port;
    static int maxWaitingConns;
    int serverSocket;

    // SSL parameters
    SSL_CTX *sslctx;

    int certfd;
    int keyfd;

    char *certPath;// = (char*)"../.ssl/serverCert.pem";
    char *keyPath;// = (char*)"../.ssl/serverKey.pem";
    char *caCertPath;// = (char*)"../.ssl/cacert.pem";

};


#endif //TIN_REMOTE_ACCESS_MANAGER_SERVER_H
