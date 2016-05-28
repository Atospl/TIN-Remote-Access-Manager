#include <iostream>
#include "Client.h"
using namespace std;

int main() {
    Client::setServerPortAndName(8765,"localhost");
    Client client = Client::getClient();
    cout << "Hello, World, I'm Klient" << "!" << endl;
    client.connectWithMainServer();
    client.runClient();
    return 0;
}