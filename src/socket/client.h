#pragma once
#include "socket.h"

struct clientSocket {
    SOCKET ConnectSocket = INVALID_SOCKET;

    bool initializeClient(const char* serverIP);
    bool disconnect();
    bool clientCleanup();
};