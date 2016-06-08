//
// Created by tomasz on 10.05.16.
//

#include "Client.h"
#include "../../Shared/Message.h"

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <cstring>
#include <climits>
#include <unistd.h>
#include <algorithm>
using namespace std;

bool Client::running = false;
int Client::port = 8765;
string Client::hostname = "localhost";

Client::~Client() {
    if (running)
        close(clientSocket);
    SSL_CTX_free(sslctx);
}

Client &Client::getClient() {
    static Client client;
    return client;
}

void Client::runClient() {
    try {
        prepare();
        logIn();
        getDataToTransfer();
    } catch (ClientException e) {
        switch (e.errorCode) {
            case ClientException::ErrorCode::SSL_ERROR:
                cerr << "OpenSSL error:" << endl;
                ERR_print_errors_fp(stderr);
                break;
            case ClientException::ErrorCode::CONNECT_FAILURE:
                cerr << "Connection failed." << endl;
                break;
            case ClientException::ErrorCode::NO_SERVER:
                cerr << "No server was found." << endl;
                break;
            case ClientException::ErrorCode::SOCKET_FAILURE:
                cerr << "Failure while creating socket." << endl;
                break;
            case ClientException::ErrorCode::LOGGING_IN_FAILURE:
                break;
        }
        close(clientSocket);
    }
}

void Client::prepare() {
    // Initialize SSL configuration and variables
    initializeSSL();
    initializeSSL_CTX();
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
        throw ClientException(ClientException::ErrorCode::SOCKET_FAILURE);
    running = true;
    server_addr.sin_family = AF_INET;
    server_hostent = gethostbyname(hostname.c_str());
    if (server_hostent == (struct hostent *) 0)
        throw ClientException(ClientException::ErrorCode::NO_SERVER);

    memcpy((char *) &server_addr.sin_addr, (char *) server_hostent->h_addr, server_hostent->h_length);

    server_addr.sin_port = htons(port);

    if (connect(clientSocket, (struct sockaddr *) &server_addr, sizeof server_addr) != 0) {
        throw ClientException(ClientException::ErrorCode::CONNECT_FAILURE);
    }

    initializeSSL_BIO();
    if (SSL_connect(ssl) == -1) {
        ERR_print_errors_fp(stderr);
        exit(1000);
    }
}

void Client::getDataToTransfer() {
    int c;
    bool again = true;
    while (again) {
        cout << "What do you want to do?" << endl;
        cout << "1 - Book an access." << endl;
        cout << "2 - Request access from this ip." << endl;
        cout << "3 - Get booking log for the machine." << endl;
        cout << "4 - Log out." << endl;
        cin >> c;
        switch (c) {
            case 1:
                sendBookingRequestMessage();
                break;

            case 2:
                sendAccessRequestMessage();
                break;

            case 3:
                sendBookingLogRequestMessage();
                break;

            case 4:
                sendLoggingOffMessage();
                again = false;
                break;

            default:
                cout << "Incorrect choice" << endl;
        }
    }
}

void Client::initializeSSL() {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
}

void Client::initializeSSL_CTX() {
    sslctx = SSL_CTX_new(SSLv23_client_method());
    if (sslctx == NULL)
        throw ClientException(ClientException::ErrorCode::SSL_ERROR);

    // Load OpenSSL certificate file
    if (SSL_CTX_use_certificate_file(sslctx, certPath, SSL_FILETYPE_PEM) <= 0)
        throw ClientException(ClientException::ErrorCode::SSL_ERROR);

    // Load OpenSSL Private Key
    if (SSL_CTX_use_PrivateKey_file(sslctx, keyPath, SSL_FILETYPE_PEM) <= 0)
        throw ClientException(ClientException::ErrorCode::SSL_ERROR);

    // Verify private key
    if (!SSL_CTX_check_private_key(sslctx))
        throw ClientException(ClientException::ErrorCode::SSL_ERROR);

    ssl = SSL_new(sslctx);
}

void Client::initializeSSL_BIO() {
    SSL_set_fd(ssl, clientSocket);
}

bool Client::setServerPortAndName(int p, string name) {
    if (!running) {
        port = p;
        hostname = name;
    }
    else
        return false;
    return true;
}

bool Client::setServerPortAndName() {
    //TODO - pobranie z pliku konfiguracyjnego
    return true;
}

void Client::logIn() {
    Message message;
    message.messageType = MessageType::LOGGING;

    Message::MessageData::LoggingMessage loggingMessage;

    char login[32];
    cout << "Login: " << endl;
    cin.getline(login, 32);
    char *password = getpass("Password: ");

    strcpy(loggingMessage.login, login);
    strcpy(loggingMessage.password, password);
    message.messageData.loggingMessage = loggingMessage;

    sendData(message);
    if (!handleResponse())
        throw ClientException(ClientException::ErrorCode::LOGGING_IN_FAILURE);
}

void Client::sendLoggingOffMessage() {
    Message message;
    message.messageType = LOGGING_OUT;
    sendData(message);
}

void Client::sendData(Message message) {
    if (SSL_write(ssl, &message, sizeof(Message)) == 0)
        throw ClientException(ClientException::ErrorCode::SSL_ERROR);
}

void Client::sendBookingRequestMessage() {
    Message message;
    message.messageType = MessageType::BOOKING;
    Message::MessageData::BookingMessage bookingMessage;
    cout << "Machine id: " << endl;
    cin >> bookingMessage.id;

    string date;
    string year;
    string month;
    string day;
    string hour;
    string minute;

    cout << "Date ( format: hour/minute/day/mth/year )" << endl;
    cin >> date;
    if (count(date.begin(), date.end(), '/') == 4) {

        hour = date.substr(0, date.find('/'));
        date = date.substr(hour.length() + 1);
        minute = date.substr(0, date.find('/'));
        date = date.substr(minute.length() + 1);
        day = date.substr(0, date.find('/'));
        date = date.substr(day.length() + 1);
        month = date.substr(0, date.find('/'));
        date = date.substr(month.length() + 1);
        year = date;

        if (isInteger(year) && isCorrectDate(month, 1, 12) && isCorrectDate(day, 1, 31)
            && isCorrectDate(hour, 0, 23) && isCorrectDate(minute, 0, 59)) {

            time_t rawtime;
            struct tm * bookingTime = localtime(&rawtime);

            bookingTime->tm_year = stoi(year) - 1900;
            bookingTime->tm_mon = stoi(month) - 1; // 0-11
            bookingTime->tm_mday = stoi(day);
            bookingTime->tm_hour = stoi(hour);
            bookingTime->tm_min = stoi(minute);
            bookingTime->tm_sec = 0;
            bookingTime->tm_isdst = -1;

            bookingMessage.date = mktime(bookingTime);
            message.messageData.bookingMessage = bookingMessage;

            sendData(message);
            handleResponse();
        }
        else{
            cerr << "Wrong values" << endl;
        }
    }
    else {
        cerr << "Wrong date format" << endl;
    }
}

void Client::sendAccessRequestMessage() {
    Message message;
    message.messageType = MessageType::ACCESS_REQUEST;

    sendData(message);
    handleResponse();
}

void Client::sendBookingLogRequestMessage() {
    Message message;
    message.messageType = MessageType::BOOKING_LOG;
    Message::MessageData::BookingMessage bookingMessage;

    string date;
    string year;
    string month;
    string day;
    string hour;
    string minute;

    cout << "Date ( format: hour/minute/day/mth/year )" << endl;
    cin >> date;
    if (count(date.begin(), date.end(), '/') == 4) {

        hour = date.substr(0, date.find('/'));
        date = date.substr(hour.length() + 1);
        minute = date.substr(0, date.find('/'));
        date = date.substr(minute.length() + 1);
        day = date.substr(0, date.find('/'));
        date = date.substr(day.length() + 1);
        month = date.substr(0, date.find('/'));
        date = date.substr(month.length() + 1);
        year = date;

        if (isInteger(year) && isCorrectDate(month, 1, 12) && isCorrectDate(day, 1, 31)
            && isCorrectDate(hour, 0, 23) && isCorrectDate(minute, 0, 59)) {

            time_t rawtime;
            struct tm * bookingTime = localtime(&rawtime);

            bookingTime->tm_year = stoi(year) - 1900;
            bookingTime->tm_mon = stoi(month) - 1; // 0-11
            bookingTime->tm_mday = stoi(day);
            bookingTime->tm_hour = stoi(hour);
            bookingTime->tm_min = stoi(minute);
            bookingTime->tm_sec = 0;
            bookingTime->tm_isdst = -1;

            bookingMessage.date = mktime(bookingTime);
            message.messageData.bookingMessage = bookingMessage;

            sendData(message);
            handleResponse();
        }
        else{
            cerr << "Wrong values" << endl;
        }
    }
    else {
        cerr << "Wrong date format" << endl;
    }


}

Message Client::receiveData() {
    Message buf;
    char *pBuf = (char *) &buf;
    short bytesToRead = sizeof(Message);

    int bytesRead = 0;
    int readValue = 0;

    while (true) {
        readValue = SSL_read(ssl, pBuf, bytesToRead);

        if (readValue > 0) { // something was read, check if it is enough
            bytesRead += readValue;
            if (bytesRead == sizeof(Message)) {
                break;
            } else {
                bytesToRead = sizeof(Message) - bytesRead;
                pBuf = (char *) &buf + bytesRead;
            }
        } else { // an error occured
            int errorNumber = SSL_get_error(ssl, readValue);
            if (errorNumber == SSL_ERROR_WANT_READ)
                continue;
            else
                throw ClientException(ClientException::ErrorCode::SSL_ERROR, errorNumber);
        }
    }
    return buf;
}

bool Client::handleResponse() {
    Message message = receiveData();
    if (message.messageType == MessageType::SUCCESS) {
        cout << message.messageData.successMessage << endl;
        return true;
    }
    else if(message.messageType = MessageType::BOOKING_LOG)
    {
        cout << "Next available date is: " << ctime(&message.messageData.bookingMessage.date) << endl;
    }
    else {
        cerr << message.messageData.failMessage << endl;
        return false;
    }
}

bool Client::isCorrectDate(string val, int lowerThan, int biggerThan) {
    return (isInteger(val) && (stoi(val) >= lowerThan)
            && (stoi(val) <= biggerThan));
}

bool Client::isInteger(const std::string &str) {
    return !str.empty() && str.find_first_not_of("0123456789") == string::npos;
}
