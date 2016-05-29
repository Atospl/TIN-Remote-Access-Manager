#ifndef SERVERAPP_IPTABLESCONTROLLER_H
#define SERVERAPP_IPTABLESCONTROLLER_H

#include <mutex>
#include <string>

class IptablesController {
private:
    static bool grantAccess(std::string ip);
    static bool revokeAccess(std::string ip);
    static bool grantWaitAndRevoke(std::string ip, unsigned int minutes);
public:
    static void grantLimitedAccess(std::string ip, unsigned int minutes);
};


#endif //SERVERAPP_IPTABLESCONTROLLER_H
