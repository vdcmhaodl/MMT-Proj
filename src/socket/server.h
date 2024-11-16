#pragma once
#include "socket.h"
#include <csignal>
#include <signal.h>
#include <unistd.h>
#include <functional>

extern bool timeout_occurred;

struct serverSocket {
    SOCKET listenSocket = INVALID_SOCKET;
    SOCKET client = INVALID_SOCKET;
    int timeListen = 60;

    bool isConnecting = false;
    bool isDiscovered = false;
    void broadcast();

    bool initializeServer();
    bool listenClient();
    bool connectClient();
    bool disconnect();
    bool serverCleanup();
};