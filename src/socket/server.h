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
    // int timeListen = 60;

    // void broadcast();

    bool initializeServer(std::string IP);
    bool listenClient();

    bool anyPendingConnection();
    bool connectClient();


    bool disconnect();
    bool serverCleanup();
};