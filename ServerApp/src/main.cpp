#include <iostream>
#include "Server.h"
#include "FilesPaths.h"
using namespace std;

int main() {
    FilesPaths::getInstance().init("../src/config/build_default.ini");
    Server::setDefaultPort(8765);
    Server::setMaxWaitingConns(10);
    Server server = Server::getServer();
    cout << "Hello, World, I'm alive!" << endl;
    server.runServer();
    return 0;
}
