//
// Created by maciek on 11.05.16.
//

#ifndef FILECONTROLLER_FILECONTROLLER_H
#define FILECONTROLLER_FILECONTROLLER_H

#include <string>
#include "time.h"
#include <vector>
#include <string>

struct client
{
    std::string login;
    unsigned long passHash;
    unsigned long salt;
};

struct reservation
{
    std::string userLogin;
    int machineId;
    time_t date;
};

struct machine
{
    int ipAdress;
    std::string name;
    int id;
};

class FileController {
public:
    static FileController & getInstance();
    ~FileController();

    std::vector<reservation> getReservations();
    std::vector<client> getClients();
    machine getMachine();

private:
    FileController(){};
    std::vector<std::string> getValuesCsv(std::string);
    bool isNumber(const std::string&);
};


#endif //FILECONTROLLER_FILECONTROLLER_H
