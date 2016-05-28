#ifndef SERVERAPP_IPTABLESCONTROLLER_H
#define SERVERAPP_IPTABLESCONTROLLER_H

#include <mutex>
#include <string>

class IptablesController {
private:
    static std::mutex iptablesMutex;
public:
    static bool grantAccess(std::string ip, unsigned int minutes);

};


#endif //SERVERAPP_IPTABLESCONTROLLER_H
