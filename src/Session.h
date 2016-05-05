//
// Created by chmielok on 5/5/16.
//

#ifndef TIN_REMOTE_ACCESS_MANAGER_SESSION_H
#define TIN_REMOTE_ACCESS_MANAGER_SESSION_H


class Session {
private:
    int socket;
    int ip4Address;
public:
    Session(int s, int adr) : socket(s), ip4Address(adr) {};
    void run();
};


#endif //TIN_REMOTE_ACCESS_MANAGER_SESSION_H
