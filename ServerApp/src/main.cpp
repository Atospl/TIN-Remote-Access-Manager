#include <iostream>
#include "Server.h"

using namespace std;

int main() {
    Server::setDefaultPort(8765);
    Server::setMaxWaitingConns(10);
    Server server = Server::getServer();
    cout << "Hello, World, I'm alive!" << endl;
    server.runServer();
    return 0;
}