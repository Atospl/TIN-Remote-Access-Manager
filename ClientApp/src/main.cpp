#include <iostream>
#include "Client.h"
using namespace std;
//25.79.216.85
int main(int argc, char* argv[]) {
    if (argc != 3)
        cout << "Please provide server address and port." << endl;
    else {
        Client::setServerPortAndName(atoi(argv[2]),argv[1]);
        Client client = Client::getClient();
        cout << "Hello, World, I'm Klient" << "!" << endl;
        client.runClient();
    }
    return 0;
}