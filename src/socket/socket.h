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
#include <atomic>
#include <syncstream>
#include <tuple>
#include <thread>
#include "../share_queue.h"

#define DEFAULT_PORT 42069
#define DEFAULT_BROADCAST 23127

using ListIPData = std::map<std::string, std::pair<std::string, std::string>>;
using IPStatusMsg = std::tuple<std::string, std::string, std::string>;

class STATUS {
public:
    static constexpr char FREE_SOCKET[] = "FREE";
    static constexpr char CREATE_SOCKET[] = "CREATE";
    static constexpr char DELETE_SOCKET[] = "DELETE";
    static constexpr char IN_CONNECTION_SOCKET[] = "CONNECTING";
};

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

    std::vector<std::string> getaddrinfo_IP(const char *nodename, const char *servname, const addrinfo *hints);
    std::string getAvailableIP(const char *nodename, const char *servname, const addrinfo *hints);
    uint32_t getBinaryAvailableIP(const char *nodename, const char *servname, const addrinfo *hints);
    std::string findSuitableIP(const char *nodename, const char *servname, const addrinfo *hints, std::string IP_sender, std::string subnetMask);

    bool update_list(ListIPData &listIP, std::string IP, std::string hostname, std::string status);
}