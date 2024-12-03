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
// #include <ioctlsocket>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstdint>
#include <fstream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <sstream>
#include <assert.h>
#include <mutex>
#include <shared_mutex>
#include <queue>
#include <map>
#include "share_queue.h"

#define DEFAULT_PORT 42069
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

    void getIPaddr(sockaddr *socket_addr, char IP_addr[INET_ADDRSTRLEN]);

    bool isClientMessage(std::string message);
    bool isServerMessage(std::string message);
    std::string createClientMessage(std::string subnetMask);
    std::string createServerMessage(std::string IP_addr, std::string name, std::string status);
    void decipherClientMessage(std::string message, std::string &subnetMask);
    void decipherServerMessage(std::string message, std::string &IP_addr, std::string &name, std::string &status);

    std::vector<std::pair<sockaddr, int>> getaddrinfo_IP(const char *nodename, const char *servname, const addrinfo *hints);
    std::string getAvailableIP(const char *nodename, const char *servname, const addrinfo *hints);
    uint32_t getBinaryAvailableIP(const char *nodename, const char *servname, const addrinfo *hints);
    std::string findSuitableIP(const char *nodename, const char *servname, const addrinfo *hints, std::string IP_sender, std::string subnetMask);

    void update_list(std::map<std::string, std::pair<std::string, std::string>> &listIP, std::string IP, std::string hostname, std::string status);
}