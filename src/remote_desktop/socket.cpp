#include "socket.h"

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
    } while (byteSent < message.size());

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
    } while(iResult > 0 && byteRecv < dataLength);

    uint32_t respond = (byteRecv == dataLength ? 200 : 400);
    respond = htonl(respond); 
    iResult = send(connectSocket, (char*)&respond, sizeof(uint32_t), 0);
    if (iResult == SOCKET_ERROR) {
        std::cout << "send() failed: " << WSAGetLastError() << '\n';
        cleanup();
        return false;
    }

    return (byteRecv == dataLength);
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

    HANDLE hFile = CreateFileA(filename.c_str(), 
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

    std::ofstream fout(filename.c_str(), std::ios::binary);
    if (!fout.is_open()) {
        std::cout << "Cannot open file!\n";
        return false;
    }
    fout.write((char*)&content[0], sizeof(char) * content.size());
    fout.close();
    return true;
}
