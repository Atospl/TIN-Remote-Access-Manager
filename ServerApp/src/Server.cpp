//
// Created by Paweł Chmielak on 5/3/16.
//
#include "Server.h"
#include "Session.h"
#include "../../Shared/Message.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "Exception.h"
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <iomanip>
#include <sstream>
#include "FileController.h"
#include "IptablesController.h"
#include <thread>

using namespace std;

bool Server::running = false;
int Server::port = 8765;
int Server::maxWaitingConns = 10;

Server::Server(){
    string x = FilesPaths::getInstance().getServerCertPath();
    certPath = new char[x.length() + 1];
    strcpy(certPath, x.c_str());

    x = FilesPaths::getInstance().getServerKeyPath();
    keyPath = new char[x.length() + 1];
    strcpy(keyPath, x.c_str());

    x = FilesPaths::getInstance().getCaCertPath();
    caCertPath = new char[x.length() + 1];
    strcpy(caCertPath, x.c_str());

    port = FilesPaths::getInstance().getServerPort();
};

Server::~Server() {
    if (running)
        close(serverSocket);
    SSL_CTX_free(sslctx);
}

Server &Server::getServer() {
    static Server server;
    return server;
}

void Server::runServer() {
    try {
        prepare();
        listenForClients();
    } catch (ServerException e) {
        cerr << "FAILURE. Source: ";
        switch (e.errorCode) {
            case ServerException::ErrorCode::SOCKET_FAILURE:
                cerr << "Socket creation failure";
                break;
            case ServerException::ErrorCode::BIND_FAILURE:
                cerr << "Socket binding failure";
                break;
            case ServerException::ErrorCode::LISTEN_FAILURE:
                cerr << "Making socket passive failure";
                break;
            case ServerException::ErrorCode::ACCEPT_FAILURE:
                cerr << "Connection acceptance failure";
                break;
        }
        cerr << endl;
    }
    close (serverSocket);
}

bool Server::setDefaultPort(int p) {
    if (!running)
        port = p;
    else
        return false; // nie można zmienić portu już działającego serwera
    return true;
}

bool Server::setMaxWaitingConns(int cap) {
    if (!running)
        maxWaitingConns = cap;
    else
        return false; // nie można zmienić dla już działającego serwera
    return true;
}

bool Server::setServerPortAndName(){
    //todo - pobranie z pliku konfiguracyjnego
    return true;
}
void Server::prepare() {
    initializeSSL();
    initializeSSL_CTX();

    int status, enable = 1;

    // faza tworzenia gniazda
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
        throw ServerException(ServerException::ErrorCode::SOCKET_FAILURE);
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    running = true;

    // faza przypięcia gniazda do programu z określonym portem
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    status = bind(serverSocket, (sockaddr*) &address, sizeof(address));
    if (status < 0)
        throw ServerException(ServerException::ErrorCode::BIND_FAILURE);

    // przerobienie gniazda na pasywne (serwer)
    status = listen(serverSocket, maxWaitingConns);
    if (status < 0)
        throw ServerException(ServerException::ErrorCode::LISTEN_FAILURE);
}

void Server::listenForClients() {
    int clientSocket;
    sockaddr_in address_info;
    socklen_t data_length = sizeof(sockaddr_in);

    Message buf;

    // czekaj na kolejne połączenia
    while (0 < (clientSocket = accept(serverSocket, (sockaddr*)&address_info, &data_length)) ) {
        Session * session;
        session = new Session(clientSocket, address_info.sin_addr.s_addr, sslctx);

        //odczytaj Message od klienta
        thread sessionThread(&Session::run, session); // utwórz wątek dla nowego połączenia
        sessionThread.join(); // uruchom wątek
    }
    cerr << errno << endl;
    throw ServerException(ServerException::ErrorCode::ACCEPT_FAILURE);
}

void Server::initializeSSL() {
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
}

void Server::initializeSSL_CTX() {
    sslctx = SSL_CTX_new(SSLv23_server_method());
    if(sslctx == NULL)
        ERR_print_errors_fp(stderr);
    // create new key every time
    SSL_CTX_set_options(sslctx, SSL_OP_SINGLE_DH_USE);
    SSL_CTX_set_verify(sslctx, SSL_VERIFY_NONE, NULL);

    /* Load trusted CA. */
    if (SSL_CTX_load_verify_locations(sslctx, NULL, caCertPath) != 1) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    if(SSL_CTX_set_default_verify_paths(sslctx) != 1){
        ERR_print_errors_fp(stderr);
        exit(1);
    }
    // set up server certificate file
    if(SSL_CTX_use_certificate_file(sslctx, certPath, SSL_FILETYPE_PEM) <= 0){
        ERR_print_errors_fp(stderr);
        exit(1);
    }
    // set up server private key file
    if(SSL_CTX_use_PrivateKey_file(sslctx, keyPath, SSL_FILETYPE_PEM) <= 0){
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    /* verify private key */
    if (!SSL_CTX_check_private_key(sslctx))
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }

}

bool Server::verifyUser(const char *login, const char *password) {
    Message message;
    vector<client> clients = FileController::getInstance().getClients();
    string hash = sha512(string(password));
    cout << clients.size();
    for(auto i : clients) {
        if (i.login == login)
            return true;
        if (i.passHash.compare(hash) == 0) {
            return true;
        }
    }
    return false;
}

string Server::sha512(string password) {
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

string Server::to_hex(unsigned char s) {
    stringstream ss;
    ss << setfill('0') << setw(2) << hex << (int) s;
    return ss.str();
}

