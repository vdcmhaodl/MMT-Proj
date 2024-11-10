#pragma once
#include "socket.h"

struct serverSocket {
    SOCKET listenSocket = INVALID_SOCKET;
    SOCKET client = INVALID_SOCKET;

    bool isConnecting = false;
    void broadcast();

    bool initializeServer();
    bool listenClient();
    bool connectClient();
    bool disconnect();
    bool serverCleanup();
};