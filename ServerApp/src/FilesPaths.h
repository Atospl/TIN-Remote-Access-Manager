//
// Created by maciek on 30.05.16.
//

#ifndef FILECONTROLLER_FILESPATHS_H
#define FILECONTROLLER_FILESPATHS_H
#include <string>

class FilesPaths {
public:
    static FilesPaths & getInstance();
    ~FilesPaths();

    bool init(std::string);
    std::string getClientsPath();
    std::string getReservationsPath();
    std::string getMachinePath();
    std::string getCaCertPath();
    std::string getServerCertPath();
    std::string getServerKeyPath();
    std::string getClientCertPath();
    std::string getClientKeyPath();
    int getServerAddr();
    int getServerPort();
    int getHttpsPort();

private:
    FilesPaths(){};
    std::string clientsPath, reservationsPath, machinePath, caCertPath, serverCertPath, serverKeyPath, clientCertPath, clientKeyPath;
    int serverAddress, serverPort, httpsPort;
};


#endif //FILECONTROLLER_FILESPATHS_H
