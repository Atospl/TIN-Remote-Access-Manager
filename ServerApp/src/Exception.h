//
// Created by atos on 04.06.16.
//

#ifndef SERVERAPP_EXCEPTION_H
#define SERVERAPP_EXCEPTION_H

struct ServerException {
    enum ErrorCode {
        SOCKET_FAILURE,
        BIND_FAILURE,
        LISTEN_FAILURE,
        ACCEPT_FAILURE
    } errorCode;
    ServerException(ErrorCode code) : errorCode(code) {};
};

struct SessionException {
    enum ErrorCode {
        LOGGING_OFF,
        SSL_ERROR,
        MESSAGE_NOT_RECOGNIZED
    } errorCode;
    int sslErrorNumber;
    SessionException(ErrorCode code, int sslErrorNumber = 0) : errorCode(code), sslErrorNumber(sslErrorNumber) {};
};

struct FileHandlerException {
    enum ErrorCode {
        FILE_OPEN
    } errorCode;
    FileHandlerException(ErrorCode code) : errorCode(code) {};
};

#endif //SERVERAPP_EXCEPTION_H
