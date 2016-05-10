#include <iostream>
#include "Server.h"

using namespace std;

int main() {
    Server::setDefaultPort(8080);
    Server server = Server::getServer();
    cout << "Hello, World, I'm alive!" << endl;
    server.runServer();
    return 0;
}