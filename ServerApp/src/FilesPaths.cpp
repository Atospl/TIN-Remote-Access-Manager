//
// Created by maciek on 30.05.16.
//

#include "FilesPaths.h"
#include "FileController.h"
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

FilesPaths &FilesPaths::getInstance() {
    static FilesPaths instance;
    return instance;
}

FilesPaths::~FilesPaths()
{

}

bool FilesPaths::init(string buildPath)
{
    stringstream str;
    string line;


    ifstream file;
    file.open(buildPath);

    if(file.is_open())
    {
        str << file.rdbuf();
        file.close();
    }
    else
        return false;

    while(getline(str, line))
    {
        vector<string> values = FileController::getValuesCsv(line);

        if(values.size() != 2)
            continue;
        //    std::string clientsPath, reservationsPath, machinePath, caCertPath, serverCertPath, serverKeyPath, clientCertPath, clientKeyPath;
        //adres servera, portservera, port https

        if(values[0] == "CLIENTS")
            clientsPath = values[1];
        else if(values[0] == "RESERVATIONS")
            reservationsPath = values[1];
        else if(values[0] == "MACHINE")
            machinePath = values[1];
        else if(values[0] == "CACERT")
            caCertPath = values[1];
        else if(values[0] == "SERVERCERT")
            serverCertPath = values[1];
        else if(values[0] == "SERVERKEY")
            serverKeyPath = values[1];
        else if(values[0] == "CLIENTCERT")
            clientCertPath = values[1];
        else if(values[0] == "CLIENTKEY")
            clientKeyPath = values[1];
        else if(values[0] == "SERVERADDR")
            serverAddress = stoi(values[1]);
        else if(values[0] == "SERVERPORT")
            serverPort = stoi(values[1]);
        else if(values[0] == "httpsPort")
            httpsPort = stoi(values[1]);
    }
}

std::string FilesPaths::getClientsPath()
{
    return clientsPath;
}
std::string FilesPaths::getReservationsPath()
{
    return reservationsPath;
}
std::string FilesPaths::getMachinePath()
{
    return machinePath;
}
std::string FilesPaths::getCaCertPath()
{
    return caCertPath;
}
std::string FilesPaths::getServerCertPath()
{
    return serverCertPath;
}
std::string FilesPaths::getServerKeyPath()
{
    return serverKeyPath;
}
std::string FilesPaths::getClientCertPath()
{
    return clientCertPath;
}
std::string FilesPaths::getClientKeyPath()
{
    return clientKeyPath;
}
int FilesPaths::getServerAddr()
{
    return serverAddress;
}
int FilesPaths::getServerPort()
{
    return serverPort;
}
int FilesPaths::getHttpsPort()
{
    return httpsPort;
}