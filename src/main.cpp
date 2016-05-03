#include <iostream>
#include "Server.h"

using namespace std;

int main() {
    Server server = Server::getServer();
    cout << "Hello, World, I'm " << server.getName() << "!" << endl;
    server.runServer();
    return 0;
}