#pragma once
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include "socket.h"

#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

/* Note: could also use malloc() and free() */

namespace socketAPI {
    void printUnicastAddress(PIP_ADAPTER_UNICAST_ADDRESS &ua);
    void printAnycastAddress(PIP_ADAPTER_ANYCAST_ADDRESS aa);
    void printMulticastAddress(PIP_ADAPTER_MULTICAST_ADDRESS ma);
    void printDnsServerAddress(PIP_ADAPTER_DNS_SERVER_ADDRESS da);
    void printPrefix(PIP_ADAPTER_PREFIX prefix);
    void outputAdapterInfo(ULONG family = AF_UNSPEC);
    
    std::pair<std::string, std::string> getUnicastAddress(PIP_ADAPTER_UNICAST_ADDRESS &ua);
    std::vector<std::pair<std::string, std::string>> getAllAddress(ULONG family = AF_INET);
    std::string getAvailableIP();
    uint32_t getBinaryAvailableIP();
    std::string findSuitableIP(std::string IP_sender, std::string subnetMask);
}