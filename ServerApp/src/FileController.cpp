//
// Created by maciek on 11.05.16.
//
#include <fstream>
#include <sstream>
#include <iostream>
#include <mutex>

#include "FileController.h"
#include "FilesPaths.h"

using namespace std;

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
    machineFile.open(FilesPaths::getInstance().getMachinePath());

    if(machineFile.is_open())
    {
        //cout << "Machine file opened!" << endl;
        str << machineFile.rdbuf();
        machineFile.close();
    }
    else
    {
        machineFileMutex.unlock();
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
    //clientsFile.open(clientsFilePath.c_str());
    clientsFile.open(FilesPaths::getInstance().getClientsPath());

    if(clientsFile.is_open())
    {
        //cout << "Clients file opened!" << endl;
        str << clientsFile.rdbuf();
        clientsFile.close();
        clientsFileMutex.unlock();

    }
    else
    {
        clientsFileMutex.unlock();

        cout<<"Error opening clients file"<<endl;
        return clients;
    }

    // ignore first line
    getline(str, line);
    while(getline(str,line))
    {
        vector<string> values = getValuesCsv(line);
        client client;
        if(values.size() != 3 || !isNumber(values[1], true) || !isNumber(values[2]))
            continue;

        client.login = values[0];
        client.passHash = values[1];
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
    reservationsFile.open(FilesPaths::getInstance().getReservationsPath());

    if(reservationsFile.is_open())
    {
        //cout << "Reservations file opened!" << endl;
        str << reservationsFile.rdbuf();
        reservationsFile.close();
        reservationsFileMutex.unlock();
    }
    else
    {
        reservationsFileMutex.unlock();
        cout<<"Error opening reservations file"<<endl;
        return reservations;
    }
    // ignore first line
    getline(str, line);
    while(getline(str,line))
    {
        vector<string> values = getValuesCsv(line);


        if(values.size() != 7 ||
                !isNumber(values[1]) ||
                !isNumber(values[2]) ||
                !isNumber(values[3]) ||
                !isNumber(values[4]) ||
                !isNumber(values[5]) ||
                !isNumber(values[6]))
            continue;

        reservation reservation;

        reservation.userLogin = values[0];

        reservation.machineId = stoi(values[1]);

        time_t rawtime;
        struct tm * date = localtime(&rawtime);
        date->tm_hour = stoi(values[2]);
        date->tm_min = stoi(values[3]);
        date->tm_sec = 0;
        date->tm_mday = stoi(values[4]);
        date->tm_mon = stoi(values[5]);
        date->tm_year = stoi(values[6]);
        date->tm_isdst = -1;


        reservation.date = mktime(date);

        reservations.push_back(reservation);
    }

    return reservations;
}

bool FileController::addReservation(std::string userLogin, int machineId, time_t date)
{
    stringstream str;
    string line;

    reservationsFileMutex.lock();
    ifstream reservationsFile;
    reservationsFile.open(FilesPaths::getInstance().getReservationsPath());

    if(reservationsFile.is_open()) {
        str << reservationsFile.rdbuf();
        reservationsFile.close();
    } else {
        reservationsFileMutex.unlock();
        cout<<"Error opening reservations file."<<endl;
        return false;
    }
    // ignore first line
    getline(str, line);

    //sprawdzenie, czy nie ma kofliktu rezerwacji, czy nie istnieje już jakaś na tę datę
    while(getline(str,line))
    {
        vector<string> values = getValuesCsv(line);

        if(values.size() != 7 ||
                !isNumber(values[1]) ||
                !isNumber(values[2]) ||
                !isNumber(values[3]) ||
                !isNumber(values[4]) ||
                !isNumber(values[5]) ||
                !isNumber(values[6]))
            continue;

        reservation reservation;

        reservation.userLogin = values[0];

        reservation.machineId = stoi(values[1]);

        time_t rawtime;
        struct tm * dateTm = localtime(&rawtime);
        dateTm->tm_hour = stoi(values[2]);
        dateTm->tm_min = stoi(values[3]);
        dateTm->tm_sec = 0;
        dateTm->tm_mday = stoi(values[4]);
        dateTm->tm_mon = stoi(values[5]);
        dateTm->tm_year = stoi(values[6]);
        dateTm->tm_isdst = -1;


        time_t dateToCompare = mktime(dateTm);

        cout<<ctime(&dateToCompare)<<endl;
        cout<<ctime(&date)<<endl;
        cout<< abs(difftime(dateToCompare, date))<< endl;
        //jeśli różnica czasu rezerwacji < 1 godzina, to zwraca false
        if (abs(difftime(dateToCompare, date)) < 3600)
        {
            reservationsFileMutex.unlock();
            return false;
        }

    }


    struct tm * dateTm;
    dateTm = localtime(&date);
    dateTm->tm_mon = dateTm->tm_mon - 1;//bo strftime zwraca miesiące z zakresu 1-12, a chcemy mieć 0-11
    dateTm->tm_year = dateTm->tm_year - 1900;//bo strfitme zwraca cały rok, a my chcemy mieć rok liczony od 1900
    //std::string dateString;
    char dateString [80];
    strftime(dateString, sizeof(dateString), "%H,%M,%d,%m,%Y", dateTm);

    std::string reservationToAdd = userLogin + "," + std::to_string(machineId) + "," + dateString;

    std::ofstream reservationsFileOut;

    reservationsFileOut.open(FilesPaths::getInstance().getReservationsPath(), std::ios::app);

    if(reservationsFileOut.is_open())
    {
        //cout << "Reservations file opened!" << endl;
        reservationsFileOut << endl <<reservationToAdd;
        reservationsFileOut.close();
        reservationsFileMutex.unlock();
        return true;
    }
    else
    {
        reservationsFileMutex.unlock();
        cout<<"Error opening reservations file"<<endl;
        return false;
    }
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

bool FileController::isNumber(const std::string& str, bool isHex) {
    if (isHex)
        return !str.empty() && str.find_first_not_of("0123456789abcdefABCDEF") == string::npos;
    else
        return !str.empty() && str.find_first_not_of("0123456789") == string::npos;
}

time_t FileController::firstAvailableDate(time_t date)
{
    vector<reservation> reservations = getReservations();

    bool dateLegal = false;

    while(!dateLegal)
    {
        dateLegal = true;
        for(auto i : reservations)
            if(abs(difftime(i.date, date)) < 3600)
            {
                dateLegal = false;

                struct tm * dateTm = localtime(&i.date);
                dateTm->tm_hour = dateTm->tm_hour + 1;
                date = mktime(dateTm);

                break;
            }
    }

    return date;
}
