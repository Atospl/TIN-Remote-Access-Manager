//
// Created by atos on 04.06.16.
//

#include "HTTPSession.h"
#include "Session.h"
#include "Exception.h"

HTTPSession::HTTPSession(int cls, int adr, SSL_CTX *ctx) : Session(cls, adr, ctx)
{}

virtual void HTTPSession::run()
{
    // perform necessary SSL operations
    initializeSSLBIO();
    SSLHandshake();
-
    Message buf;
    char* pBuf = (char *)&buf;
    short bytesToRead = sizeof(Message);

    int bytesRead = 0;
    int readValue = 0;

    try {
        while (true) {
            readValue = SSL_read(ssl, pBuf, bytesToRead);

            if (readValue > 0) { // something was read, check if it is enough
                bytesRead += readValue;
                if (bytesRead == sizeof(Message)) {
                    handleMessage(buf);
                    pBuf = (char *)&buf;
                    bytesToRead = sizeof(Message);
                    bytesRead = 0;
                } else {
                    bytesToRead = sizeof(Message) - bytesRead;
                    pBuf = (char *)&buf + bytesRead;
                }
            } else { // an error occured
                int errorNumber = SSL_get_error(ssl, readValue);
                if (errorNumber == SSL_ERROR_WANT_READ)
                    continue;
                else
                    throw SessionException(SessionException::ErrorCode::SSL_ERROR, errorNumber);
            }

        }

    } catch (SessionException e){
        if (e.errorCode == SessionException::ErrorCode::SSL_ERROR)
        if (e.sslErrorNumber == SSL_ERROR_SYSCALL)
            cerr << "Connection ended" << endl;
        else
            cerr << "SSL error: " << e.sslErrorNumber;
        close(clientSocket);
        delete this;
    }
}