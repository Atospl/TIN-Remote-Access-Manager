#include <iostream>
#include "Server.h"
#include "FilesPaths.h"
#include <thread>
#include "HTTPServerFacade.h"
using namespace std;

int main() {
    FilesPaths::getInstance().init("/home/atos/Projects/TIN/ServerApp/src/config/build_default.ini");
    HTTPServerFacade httpServer = HTTPServerFacade::getServer();
    httpServer.runServer();

    while(1);
//    Server::setDefaultPort(8765);
//    Server::setMaxWaitingConns(10);
//    Server server = Server::getServer();
//    cout << "Hello, World, I'm alive!" << endl;
//    server.runServer();
    return 0;
}
