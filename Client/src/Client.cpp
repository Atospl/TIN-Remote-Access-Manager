//
// Created by tomasz on 10.05.16.
//

#include "Client.h"
#include "../../Shared/Message.h"

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

using namespace std;

bool Client::running = false;
int Client::port = 8080;
string Client::hostname = "localhost";

Client::~Client() {
    close(clientSocket);
}

Client &Client::getClient() {
    static Client client;
    return client;
}

void Client::runClient() {

}

void Client::prepare(){
    cout<<"PREPARE"<<endl;
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
        throw ClientException(ClientException::ErrorCode::SOCKET_FAILURE);

    server_addr.sin_family = AF_INET;
    server_hostent = gethostbyname(hostname.c_str());
    if (server_hostent == (struct hostent *) 0)
        throw ClientException(ClientException::ErrorCode::NO_SERVER);

    memcpy((char *) &server_addr.sin_addr, (char *) server_hostent->h_addr, server_hostent->h_length);

    server_addr.sin_port = htons(port);

    if (connect(clientSocket, (struct sockaddr *) &server_addr, sizeof server_addr) == -1) {
        throw ClientException(ClientException::ErrorCode::CONNECT_FAILURE);
    }

}

void Client::connectWithMainServer(){
    try {
        prepare();
        sendData();
    } catch (ClientException e) {
        cerr << "FAILURE. Code" << e.errorCode << endl;
    }
    close (clientSocket);
}

void Client::sendData() {

    Message message;
    char c;
    cout<<"Co chcesz wyslac"<<endl;
    cout<<"0 - LOGGING"<<endl;
    cout<<"1 - BOOKING"<<endl;
    cout<<"2 - ACCESS_REQUEST"<<endl;
    cout<<"3 - FAIL"<<endl;
    cout<<"4 - SUCCESS"<<endl;
    cout<<"5 - MACHINE_DATA"<<endl;
    cout<<"6 - BOOKING_LOG"<<endl;
    c = cin.get();
    cin.ignore(INT_MAX,'\n');
    switch (c){
        case '0':{
            message.messageType = Message::MessageType::LOGGING;
            Message::MessageData::LoggingMessage loggingMessage;
            strcpy(loggingMessage.login ,"login");
            strcpy(loggingMessage.password, "password");
            message.messageData.loggingMessage = loggingMessage;
            break;
        }
        case '1':{
            message.messageType = Message::MessageType::BOOKING;
            Message::MessageData::BookingMessage bookingMessage;
            bookingMessage.id = 666;
            time(&bookingMessage.data);
            message.messageData.bookingMessage = bookingMessage;
            break;
        }
        case '2':{
            message.messageType = Message::MessageType::ACCESS_REQUEST;
            break;
        }
        case '3':{
            message.messageType = Message::MessageType::FAIL;
            strcpy(message.messageData.failMessage ,"failMessage");
            break;
        }
        case '4':{
            message.messageType = Message::MessageType::SUCCESS;
            strcpy(message.messageData.successMessage ,"successMessage");
            break;
        }
        case '5':{
            message.messageType = Message::MessageType::MACHINE_DATA;
            Message::MessageData::MachineDataMessage machineDataMessage;
            strcpy(machineDataMessage.information ,"information");
            machineDataMessage.id = 666;
            message.messageData.machineDataMessage = machineDataMessage;
            break;
        }
        case '6':{
            message.messageType = Message::MessageType::BOOKING_LOG;
            Message::MessageData::MachineDataMessage machineDataMessage;
            machineDataMessage.id = 8;
            strcpy(machineDataMessage.information, "information");
            message.messageData.machineDataMessage = machineDataMessage;
            break;
        }
    }
    void * pointer = (void*) &message;
    if (write( clientSocket, pointer, sizeof (Message) ) == -1)
        perror("writing on stream socket");
}

bool Client::setServerPortAndName(int p, string name){
    if(!running){
        port = p;
        hostname = name;
    }
    else
        return false;
    return true;
}