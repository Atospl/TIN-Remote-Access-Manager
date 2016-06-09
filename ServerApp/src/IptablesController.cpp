//
// Created by chmielok on 5/28/16.
//

#include <thread>
#include "IptablesController.h"

using namespace std;

bool IptablesController::grantAccess(std::string ip) {
    string command = "iptables -I INPUT -p tcp -s " + ip + " --dport 22 -j ACCEPT";
    return system(command.c_str()) == 0;
}

bool IptablesController::revokeAccess(std::string ip) {
    string command = "iptables -D INPUT -p tcp -s " + ip + " --dport 22 -j ACCEPT";
    return system(command.c_str()) == 0;
}

bool IptablesController::grantWaitAndRevoke(std::string ip, unsigned int minutes) {
    bool success;
    if (grantAccess(ip)) {
        this_thread::__sleep_for(chrono::seconds(minutes * 60), chrono::nanoseconds(0));
        success = revokeAccess(ip);
    }
    else
        success = false;
    return success;
}

void IptablesController::grantLimitedAccess(std::string ip, unsigned int minutes) {
    thread workerThread(&IptablesController::grantWaitAndRevoke, ip, minutes);
    workerThread.detach();
}

