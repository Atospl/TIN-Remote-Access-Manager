//
// Created by maciek on 11.05.16.
//

#ifndef FILECONTROLLER_FILECONTROLLER_H
#define FILECONTROLLER_FILECONTROLLER_H

#include <string>
#include "time.h"
#include <vector>
#include <string>
#include <cmath>

struct client
{
    std::string login;
    std::string passHash;
    std::string salt;
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
    bool addReservation(std::string userLogin, int machineId, time_t date);
    std::vector<std::string> static getValuesCsv(std::string);
    time_t firstAvailableDate(time_t);


private:
    FileController(){};
    bool isNumber(const std::string&, bool isHex = false);
};


#endif //FILECONTROLLER_FILECONTROLLER_H
