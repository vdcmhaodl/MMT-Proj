#pragma once
#include "socket.h"

struct clientSocket {
    SOCKET ConnectSocket = INVALID_SOCKET;

    // std::vector<std::pair<std::string, std::string>> broadcast(std::string localIP, std::string subnetmask);

    bool initializeClient(const char* serverIP);
    bool connectServer();
    bool disconnect();
    bool clientCleanup();
};