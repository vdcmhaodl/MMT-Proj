#pragma once
#include "socket.h"
#include <csignal>
#include <signal.h>
#include <unistd.h>
#include <functional>

struct serverSocket {
    SOCKET listenSocket = INVALID_SOCKET;
    SOCKET client = INVALID_SOCKET;

    bool initializeServer(std::string IP);
    bool listenClient();

    bool anyPendingConnection();

    bool disconnect();
    bool cleanup();
};