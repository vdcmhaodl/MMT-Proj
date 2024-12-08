#include "socket.h"
#include "ipconfig.h"

bool socketAPI::initializeSocket() {
    WSAData wsadata;

    int result = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (result != 0) {
        /* Failed to initialize */
        std::cout << "WSAStartup failed: " << result;
        return false;
    }

    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        std::cout << "Could not find a usable version of Winsock.dll\n";
        WSACleanup();
        return false;
    }

    return true;
}

bool socketAPI::cleanup() {
    int result = WSACleanup();
    if (result != 0) {
        std::cout << "Error: " << WSAGetLastError() << '\n';
        return false;
    }
    return true;
}

uint32_t socketAPI::getBinaryIP(char IPv4_address[]) {
    struct in_addr sa;
    int result = inet_pton(AF_INET, IPv4_address, &sa);
    if (result == 1) {
        return sa.S_un.S_addr;
    }
    if (result == 0) {
        std::cout << "Invalid!\n";
    }
    if (result == -1) {
        std::cout << "inet_pton() failed: " << WSAGetLastError() << '\n';
    }
    return false;
}

uint32_t socketAPI::getBinarySubnetMask(char subnetMask[]) {
    return getBinaryIP(subnetMask);
}

uint32_t socketAPI::getBinaryBroadcast(char IPv4_address[], char subnetMask[], char broadcast_address[]) {
    if (!getBinaryIP(IPv4_address) || !getBinarySubnetMask(subnetMask)) {
        return false;
    }

    struct in_addr ip, netmask, broadcast;
    inet_pton(AF_INET, IPv4_address, &ip);
    inet_pton(AF_INET, subnetMask, &netmask);
    broadcast.S_un.S_addr = ip.S_un.S_addr | ~netmask.S_un.S_addr;

    if (broadcast_address != NULL) {
        inet_ntop(AF_INET, &broadcast, broadcast_address, INET_ADDRSTRLEN);
    }

    return broadcast.S_un.S_addr;
}

bool socketAPI::sameSubnet(char hostAddress_1[], char hostAddress_2[], char subnetMask[]) {
    return getBinaryBroadcast(hostAddress_1, subnetMask, NULL) == getBinaryBroadcast(hostAddress_2, subnetMask, NULL);
}

bool socketAPI::sendMessage(SOCKET &connectSocket, std::string &message) {
    int iResult;
    int byteSent = 0;

    uint32_t messageLength = htonl(message.size());

    iResult = send(connectSocket, (char*)&messageLength, sizeof(uint32_t), 0);
    if (iResult == SOCKET_ERROR) {
        std::cout << "send() failed: " << WSAGetLastError() << '\n';
        cleanup();
        return false;
    }

    do {
        iResult = send(connectSocket, message.c_str() + byteSent, message.size() - byteSent, 0);
        if (iResult == SOCKET_ERROR) {
            std::cout << "send() failed: " << WSAGetLastError() << '\n';
            cleanup();
            return false;
        }

        byteSent += iResult;
    } while (byteSent < (int)message.size());

    uint32_t respond;
    iResult = recv(connectSocket, (char*)&respond, sizeof(uint32_t), 0); 
    if (iResult == SOCKET_ERROR) { 
        std::cout << "recv() failed: " << WSAGetLastError() << '\n'; 
        cleanup(); 
        return false; 
    }
    respond = ntohl(respond);

    return (respond == 200);
}

bool socketAPI::receiveMessage(SOCKET &connectSocket, std::string &message) {
    uint32_t dataLength;
    recv(connectSocket, (char*)&dataLength, sizeof(uint32_t), 0);
    dataLength = ntohl(dataLength);

    message.clear();
    message.assign(dataLength, 0x00);
    int iResult;
    int byteRecv = 0;

    do {
        iResult = recv(connectSocket, &(message[byteRecv]), dataLength - byteRecv, 0);
        if (iResult > 0) {
            byteRecv += iResult;
        }
        else if (iResult != 0) {
            std::cout << "recv() failed: " << WSAGetLastError() << '\n';
            cleanup();
            return false;
        }
    } while(iResult > 0 && byteRecv < (int)dataLength);

    uint32_t respond = (byteRecv == (int)dataLength ? 200 : 400);
    respond = htonl(respond); 
    iResult = send(connectSocket, (char*)&respond, sizeof(uint32_t), 0);
    if (iResult == SOCKET_ERROR) {
        std::cout << "send() failed: " << WSAGetLastError() << '\n';
        cleanup();
        return false;
    }

    return (byteRecv == (int)dataLength);
}

bool socketAPI::sendFile(SOCKET &connectSocket, std::string &pathFile) {
    HANDLE hFile = CreateFileA(pathFile.c_str(), 
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cout << "CreateFile() failed!\n";
        return false;
    }

    std::string filename = std::filesystem::path(pathFile).filename().string();
    bool resultFilename = sendMessage(connectSocket, filename);
    if (!resultFilename) {
        return false;
    }

    LARGE_INTEGER sizeFile;
    BOOL resultSizeFile = GetFileSizeEx(hFile, &sizeFile);
    if (!resultSizeFile) {
        CloseHandle(hFile);
        return false;
    }

    uint32_t __sizeFile = htonl(sizeFile.QuadPart);
    int iResult = send(connectSocket, (char*)&__sizeFile, sizeof(uint32_t), 0);
    if (iResult == SOCKET_ERROR) {
        std::cout << "send() failed: " << WSAGetLastError() << '\n';
        closesocket(connectSocket);
        cleanup();
        return false;
    }

    iResult = TransmitFile(connectSocket, hFile, 0, 0, NULL, NULL, 0);
    CloseHandle(hFile);
    if (!iResult) {
        std::cout << "TransmitFile() failed: " << WSAGetLastError() << '\n';
        closesocket(connectSocket);
        cleanup();
        return false;
    }

    uint32_t respond;
    iResult = recv(connectSocket, (char*)&respond, sizeof(uint32_t), 0); 
    if (iResult == SOCKET_ERROR) { 
        std::cout << "recv() failed: " << WSAGetLastError() << '\n'; 
        closesocket(connectSocket);
        cleanup(); 
        return false; 
    }
    respond = ntohl(respond);
    return (respond == 200);
}

bool socketAPI::receiveFile(SOCKET &connectSocket, std::string &pathFile) {
    std::string filename;
    bool resultFilename = receiveMessage(connectSocket, filename);
    if (!resultFilename) {
        return false;
    }
    pathFile += filename;

    HANDLE hFile = CreateFileA(pathFile.c_str(), 
                               GENERIC_WRITE,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cout << "CreateFile() failed!\n";
        return false;
    }
    CloseHandle(hFile);

    std::string content;
    bool iResult = receiveMessage(connectSocket, content);
    if (!iResult) {
        return false;
    }

    std::ofstream fout(pathFile.c_str(), std::ios::binary);
    if (!fout.is_open()) {
        std::cout << "Cannot open file!\n";
        return false;
    }
    fout.write((char*)&content[0], sizeof(char) * content.size());
    fout.close();
    //
    // pathFile = filename;
    //
    return true;
}

void socketAPI::getIPaddr(sockaddr *socket_addr, char IP_addr[INET_ADDRSTRLEN]) {
    ZeroMemory(IP_addr, sizeof(char) * INET_ADDRSTRLEN);
    sockaddr_in *tmp = reinterpret_cast<sockaddr_in*>(socket_addr);
    inet_ntop(AF_INET, &(tmp->sin_addr), IP_addr, INET_ADDRSTRLEN);
}

bool socketAPI::isClientMessage(std::string message) {
    return message.substr(0, 6) == "client";
}

bool socketAPI::isServerMessage(std::string message) {
    return message.substr(0, 6) == "server";
}

std::string socketAPI::createClientMessage(std::string subnetMask) {
    std::string delim = "|";
    return std::string("client") + delim + subnetMask;
}

std::string socketAPI::createServerMessage(std::string IP_addr, std::string name, std::string status) {
    std::string delim = "|";
    return std::string("server") + delim + IP_addr + delim + name + delim + status;
}

void socketAPI::decipherClientMessage(std::string message, std::string &subnetMask) {
    std::stringstream ss;
    std::string header;
    ss << message;
    std::getline(ss, header, '|');
    std::getline(ss, subnetMask);
    assert(header == "client");
}

void socketAPI::decipherServerMessage(std::string message, std::string &IP_addr, std::string &name, std::string &status) {
    std::stringstream ss;
    std::string header;
    ss << message;
    std::getline(ss, header, '|');
    std::getline(ss, IP_addr, '|');
    std::getline(ss, name, '|');
    std::getline(ss, status);
    assert(header == "server");
}

std::vector<std::string> socketAPI::getaddrinfo_IP(const char *nodename, const char *servname, const addrinfo *hints) {
    addrinfo *result, *ptr = NULL;
    int iResult = getaddrinfo(nodename, servname, hints, &result);
    if (iResult != 0) {
        return std::vector<std::string>();
    }

    // std::cout << "AVAILABLE:\n";
    char IP[INET_ADDRSTRLEN];
    std::vector<std::string> listIP;
    for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        getIPaddr(ptr->ai_addr, IP);
        listIP.emplace_back(IP);
        // std::cout << IP << '\n';
    }

    freeaddrinfo(result);
    return listIP;
}

std::string socketAPI::getAvailableIP(const char *nodename, const char *servname, const addrinfo *hints) {
    std::vector<std::string> listIP = getaddrinfo_IP(nodename, servname, hints);
    return listIP[rand() % (int)listIP.size()];
}

uint32_t socketAPI::getBinaryAvailableIP(const char *nodename, const char *servname, const addrinfo *hints) {
    std::vector<std::string> listIP = getaddrinfo_IP(nodename, servname, hints);
    return getBinaryIP((char*)listIP[rand() % (int)listIP.size()].c_str());
}

std::string socketAPI::findSuitableIP(const char *nodename, const char *servname, const addrinfo *hints, std::string IP_sender, std::string subnetMask) {
    std::vector<std::string> listIP = getaddrinfo_IP(nodename, servname, hints);
    for (auto &hostIP : listIP) {
        // std::cout << "IP Address: " << hostIP << '\n';
        if (socketAPI::sameSubnet((char*)IP_sender.c_str(), (char*)hostIP.c_str(), (char*)subnetMask.c_str())) {
            return hostIP;
        }
    }

    return std::string();
}

bool socketAPI::update_list(ListIPData &listIP, std::string IP, std::string hostname, std::string status) {
    auto it = listIP.find(IP);
    if (it == listIP.end()) {
        listIP.insert({IP, make_pair(hostname, STATUS::FREE_SOCKET)});
        return true;
    }

    auto new_update = make_pair(hostname, status);
    if (new_update != it->second) {
        if (status == STATUS::CREATE_SOCKET) {
            it->second = make_pair(hostname, STATUS::FREE_SOCKET);
        }
        else if (status == STATUS::DELETE_SOCKET) {
            listIP.erase(it);
        }
        else {
            it->second = new_update;
        }
        return true;
    }
    return false;
}
