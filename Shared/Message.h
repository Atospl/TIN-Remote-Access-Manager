//
// Created by chmielok on 5/10/16.
//

#ifndef TIN_REMOTE_ACCESS_MANAGER_MESSAGE_H
#define TIN_REMOTE_ACCESS_MANAGER_MESSAGE_H

#include <cstdint>
#include <ctime>
enum MessageType {
    BOOKING = 1,
    ACCESS_REQUEST = 2,
    FAIL = 3,
    SUCCESS = 4,
    MACHINE_DATA = 5,
    BOOKING_LOG = 6,
    LOGGING = 7
};

struct Message {
    MessageType messageType;

    union MessageData {
        struct LoggingMessage {
            char login[32];
            char password[20];
        } loggingMessage;

        struct BookingMessage {
            uint32_t id;
            time_t data;
        } bookingMessage;

        char failMessage[64];
        char successMessage[64];

        struct MachineDataMessage {
            uint32_t id;
            char information[32];
        } machineDataMessage;

        struct BookingLogMessage {
            uint32_t id;
            char information[32];
        };

    } messageData;
};


#endif //TIN_REMOTE_ACCESS_MANAGER_MESSAGE_H
