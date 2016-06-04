//
// Created by atos on 04.06.16.
//

#ifndef SERVERAPP_HTTPSESSION_H
#define SERVERAPP_HTTPSESSION_H

#include "Session.h"

class HTTPSession : public Session {
public:
    HTTPSession(int cls, int adr, SSL_CTX *ctx);

    virtual ~HTTPSession();

    virtual void run();
};


#endif //SERVERAPP_HTTPSESSION_H
