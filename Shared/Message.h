//
// Created by chmielok on 5/10/16.
//

#ifndef TIN_REMOTE_ACCESS_MANAGER_MESSAGE_H
#define TIN_REMOTE_ACCESS_MANAGER_MESSAGE_H

#include <cstdint>
#include <ctime>
enum MessageType {
    LOGGING = 1,
    BOOKING = 2,
    ACCESS_REQUEST = 3,
    FAIL = 4,
    SUCCESS = 5,
    MACHINE_DATA = 6,
    BOOKING_LOG = 7
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
