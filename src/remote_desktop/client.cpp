#include "client.h"

std::vector<std::pair<std::string, std::string>> clientSocket::broadcast(std::string localIP, std::string subnetmask) {
    socketAPI::initializeSocket();

    SOCKET clientBroadcast = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientBroadcast == INVALID_SOCKET) {
        std::cout << "socket() failed: " << WSAGetLastError() << '\n';
        socketAPI::cleanup();
        return std::vector<std::pair<std::string, std::string>>();
    }

    struct sockaddr_in hints;
    ZeroMemory(&hints, sizeof(sockaddr_in));
    hints.sin_family = AF_INET;
    hints.sin_port = htons(DEFAULT_BROADCAST);
    hints.sin_addr.S_un.S_addr = socketAPI::getBinaryBroadcast((char*)localIP.c_str(), (char*)subnetmask.c_str(), NULL);

    int iResult;

    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    iResult = setsockopt(clientBroadcast, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
    if (iResult == SOCKET_ERROR) {
        std::cout << "setsockopt() failed: " << WSAGetLastError() << '\n';
        closesocket(clientBroadcast);
        socketAPI::cleanup();
        return std::vector<std::pair<std::string, std::string>>();
    }
    std::cout << "Set up time: " << tv.tv_sec << '\n';

    // int maxMsgSize; 
    // int optLen = sizeof(maxMsgSize);
    // iResult = setsockopt(clientBroadcast, SOL_SOCKET, SO_MAX_MSG_SIZE, (char*)&maxMsgSize, optLen);
    // std::cout << "Maximum packet size (byte): " << maxMsgSize << '\n';

    // Message: subnet mask
    iResult = sendto(clientBroadcast, (char*)subnetmask.c_str(), subnetmask.size(), 0, (sockaddr*)&hints, sizeof(hints));
    if (iResult == SOCKET_ERROR) {
        std::cout << "sendto() failed: " << WSAGetLastError() << '\n';
        closesocket(clientBroadcast);
        socketAPI::cleanup();
        return std::vector<std::pair<std::string, std::string>>();
    }

    std::vector<std::pair<std::string, std::string>> listServer;

    char respond[256];
    sockaddr server;

    for (int T = 1; T <= 128; T++) {
        ZeroMemory(respond, sizeof(respond));
        ZeroMemory(&server, sizeof(server));
        int len_server = sizeof(server);

        int len_respond = recvfrom(clientBroadcast, respond, 256, 0, &server, &len_server);
        if (len_respond == SOCKET_ERROR) {
            // std::cout << "recvfrom() failed: " << WSAGetLastError() << '\n';
            // closesocket(clientBroadcast);
            // socketAPI::cleanup();
            // return std::vector<std::pair<std::string, std::string>>();
            continue;
        }
        int pos = std::find(respond, respond + len_respond, ' ') - respond;
        std::string serverIP(respond, respond + pos), serverName(respond + pos + 1, respond + len_respond);
        listServer.emplace_back(serverIP, serverName);
    }

    std::cout << "LIST SERVER:\n";
    for (auto &[serverIP, serverName]: listServer) {
        std::cout << serverIP << ": " << serverName << '\n';
    }

    closesocket(clientBroadcast);
    socketAPI::cleanup();
    return listServer;
}

bool clientSocket::initializeClient(const char *serverIP)
{
    struct addrinfo *result = nullptr, *ptr = nullptr, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_TCP;

    int iResult = getaddrinfo(serverIP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed: " << iResult << '\n';
        socketAPI::cleanup();
        return false;
    }

    ptr = result;
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << '\n';
        freeaddrinfo(result);
        socketAPI::cleanup();
        return false;
    }

    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cout << "connect() failed: " << WSAGetLastError() << '\n';
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        std::cout << "Unable to connect to server\n";
        return false;
    }

    std::cout << "CONNECT SUCCESS!\n";

    // closesocket(ConnectSocket);
    // ConnectSocket = INVALID_SOCKET;
    
    std::cout << "close complete!\n";
    return true;
}

bool clientSocket::connectServer() {
    return false;
}

bool clientSocket::disconnect() {
    int iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cout << "shutdown() failed: " << WSAGetLastError() << '\n';
        closesocket(ConnectSocket);
        socketAPI::cleanup();
        return false;
    }
    return true;
}

bool clientSocket::clientCleanup() {
    int iResult = closesocket(ConnectSocket);
    if (iResult != 0) {
        std::cout << "closesocket() failed: " << WSAGetLastError() << '\n';
        return false;
    }
    return true;
}
