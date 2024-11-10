#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <mswsock.h>
#include <filesystem>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstdint>
#include <fstream>
#include <vector>
#include <bitset>
#include <algorithm>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "42069"
#define DEFAULT_BROADCAST 23127
#define DEFAULT_BUFLEN 1024
#define OK_RESPOND 200
#define ERROR_RESPOND 400

namespace socketAPI {
    bool initializeSocket();
    bool cleanup();

    uint32_t getBinaryIP(char IPv4_address[]);
    uint32_t getBinarySubnetMask(char subnetMask[]);
    uint32_t getBinaryBroadcast(char IPv4_address[], char subnetMask[], char broadcast_address[]);
    bool sameSubnet(char hostAddress_1[], char hostAddress_2[], char subnetMask[]);
    
    bool sendMessage(SOCKET& connectSocket, std::string &message);
    bool receiveMessage(SOCKET &connectSocket, std::string &message);
    bool sendFile(SOCKET &connectSocket, std::string &pathFile);
    bool receiveFile(SOCKET &connectSocket, std::string &pathFile);
}