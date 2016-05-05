//
// Created by chmielok on 5/5/16.
//

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include "Session.h"

void Session::run() {
    char buf[20];
    std::cout << "Hello! Incoming connection from: " << inet_ntop(AF_INET, &ip4Address, buf, 100) << std::endl;
    close(socket);
    delete this;
}

