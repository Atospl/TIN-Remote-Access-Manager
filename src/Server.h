//
// Created by chmielok on 5/3/16.
//

#ifndef TIN_REMOTE_ACCESS_MANAGER_SERVER_H
#define TIN_REMOTE_ACCESS_MANAGER_SERVER_H


class Server {
public:
    static Server & getServer() {
        static Server server;
        return server;
    }
    char* getName();
    void runServer();
private:
    bool running = false;
    Server();
    int port;
    char* name;
    int serverSocketDescriptor;
    int socketDescriptor;
};


#endif //TIN_REMOTE_ACCESS_MANAGER_SERVER_H
