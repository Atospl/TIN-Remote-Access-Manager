//
// Created by maciek on 11.05.16.
//
#include <fstream>
#include <sstream>
#include <iostream>
#include <mutex>

#include "FileController.h"

using namespace std;

static const string machineFilePath = "machine";
static const string reservationsFilePath = "reservations";
static const string clientsFilePath = "clients";

mutex clientsFileMutex;
mutex reservationsFileMutex;
mutex machineFileMutex;

FileController &FileController::getInstance() {
    static FileController instance;
    return instance;
}

FileController::~FileController() {

}

//jeśli nie udało sie otworzyć pliku lub dane są błędne, zwraca maszynę o adresie -1
machine FileController::getMachine()
{
    stringstream str;
    machine toReturn;
    string line;

    machineFileMutex.lock();
    ifstream machineFile;
    machineFile.open(machineFilePath.c_str());

    if(machineFile.is_open())
    {
        //cout << "Machine file opened!" << endl;
        str << machineFile.rdbuf();
        machineFile.close();
    }
    else
    {
        cout<<"Error opening machine file"<<endl;
        toReturn.ipAdress = -1;
        return toReturn;
    }

    machineFileMutex.unlock();
    // ignore the first line
    getline(str, line);

    getline(str, line);
    vector<string> values = getValuesCsv(line);

    if(values.size() == 3 && isNumber(values[0]) && isNumber(values[2]))
    {
        toReturn.id= stoi(values[2]);
        toReturn.ipAdress = stoi(values[0]);
        toReturn.name = values[1];
    }
    else
    {
        cout << "Machine file data broken." << endl;
        toReturn.ipAdress = -1;
        return toReturn;
    }

    return toReturn;
}

//pomija błedne wpisy, dla błędnego pliku wypisuje komunikat na cout i zwraca pustą listę
vector<client> FileController::getClients() {
    vector <client> clients;

    stringstream str;
    string line;

    clientsFileMutex.lock();
    ifstream clientsFile;
    clientsFile.open(clientsFilePath.c_str());

    if(clientsFile.is_open())
    {
        //cout << "Clients file opened!" << endl;
        str << clientsFile.rdbuf();
        clientsFile.close();
    }
    else
    {
        cout<<"Error opening clients file"<<endl;
        return clients;
    }

    // ignore first line
    getline(str, line);
    while(getline(str,line))
    {
        vector<string> values = getValuesCsv(line);
        client client;

        if(values.size() != 3 || !isNumber(values[1]) || !isNumber(values[2]))
        //if(values.size() != 3)
            continue;

        client.login = values[0];

        //if(number(values[1])) sprawdzić czy liczba
        client.passHash = stoul(values[1]);

        client.salt = stoul(values[2]);

        clients.push_back(client);
    }

    return clients;
}

//pomija błedne wpisy, dla błędnego pliku wypisuje komunikat na cout i zwraca pustą listę
vector<reservation> FileController::getReservations() {
    vector <reservation> reservations;

    stringstream str;
    string line;

    reservationsFileMutex.lock();
    ifstream reservationsFile;
    reservationsFile.open(reservationsFilePath.c_str());

    if(reservationsFile.is_open())
    {
        //cout << "Reservations file opened!" << endl;
        str << reservationsFile.rdbuf();
        reservationsFile.close();
    }
    else
    {
        cout<<"Error opening reservations file"<<endl;
        return reservations;
    }
    // ignore first line
    getline(str, line)
    while(getline(str,line))
    {
        vector<string> values = getValuesCsv(line);

        int i = values.size();

        if(values.size() != 7 || !isNumber(values[1]) || !isNumber(values[2])  || !isNumber(values[3]) || !isNumber(values[4]) || !isNumber(values[5]) || !isNumber(values[6]))
            continue;

        reservation reservation;

        reservation.userLogin = values[0];

        reservation.machineId = stoi(values[1]);

        struct tm * date = new struct tm;
        date->tm_hour = stoi(values[2]);
        date->tm_min = stoi(values[3]);
        date->tm_sec = 0;
        date->tm_mday = stoi(values[4]);
        date->tm_mon = stoi(values[5]);
        date->tm_year = stoi(values[6]);

        reservation.date = mktime(date);

        delete date;
        reservations.push_back(reservation);
    }

    return reservations;
}
//dla wejściowego stringa z csv zwraca wektor rozdzielonych stringów
vector<string> FileController::getValuesCsv(string line) {
    unsigned long curr = 0;
    unsigned long i = curr;
    vector<string> result;

    while(curr < line.size())
    {
        while(line[curr] != ',' && curr < line.size())
            ++curr;

        result.push_back(line.substr(i, curr - i));
        ++curr;
        i = curr;
       // ++curr;
    }
    return result;
}

bool FileController::isNumber(const std::string& str) {
    std::string::const_iterator it = str.begin();
    while (it != str.end() && isdigit(*it))
        ++it;
    return !str.empty() && it == str.end();
}
