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
#include "../../Shared/Message.h"

using namespace std;

class Client {
public:
    static bool setServerPortAndName(int port, string name);
    static bool setServerPortAndName();
    static Client & getClient();
    void runClient();
    ~Client();

private:
    // zablokowany konstruktor domyślny (singleton)
    Client(){};

    // przygotuj klienta
    void prepare();
    // wyslanie danych
    void sendData(Message message);

    bool handleResponse();

    Message receiveData();

    void getDataToTransfer();

    void sendBookingRequestMessage();

    void sendAccessRequestMessage();

    void sendMachineDataRequestMessage();

    void sendBookingLogRequestMessage();

    bool isInteger(const std::string & s);

    bool isCorrectDate(std::string, int, int);

    void logIn();

    void logOut();
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

    static constexpr char *certPath = (char*)"../.ssl/clientCert.pem";
    static constexpr char *keyPath = (char*)"../.ssl/clientKey.pem";

    struct ClientException {
        enum ErrorCode {
            SOCKET_FAILURE,
            NO_SERVER,
            CONNECT_FAILURE,
            LOGGING_IN_FAILURE,
            LOGGING_OFF,
            SSL_ERROR
        } errorCode;
        int sslErrorNumber;
        string additionalInfo;
        ClientException(ErrorCode code, int sslError = 0, string info = "") :
                errorCode(code),
                sslErrorNumber(sslError),
                additionalInfo(info) {};
    };
};

#endif //CLIENT_CLIENT_H
