#include "server.h"

void serverSocket::broadcast() {
    socketAPI::initializeSocket();

    SOCKET serverBroadcast = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverBroadcast == INVALID_SOCKET) {
        std::cout << "socket() failed: " << WSAGetLastError() << '\n';
        socketAPI::cleanup();
        return;
    }

    struct sockaddr_in hints;
    ZeroMemory(&hints, sizeof(sockaddr_in));
    hints.sin_family = AF_INET;
    hints.sin_port = htons(DEFAULT_BROADCAST);
    hints.sin_addr.S_un.S_addr = ADDR_ANY;

    int iResult = bind(serverBroadcast, (sockaddr*)&hints, sizeof(hints));
    if (iResult == SOCKET_ERROR) {
        std::cout << "bind() failed: " << WSAGetLastError() << '\n';
        closesocket(serverBroadcast);
        socketAPI::cleanup();
        return;
    }

    while(isConnecting == false) {
        // Receive subnet mask
        char subnetmask[INET_ADDRSTRLEN];
        char clientIP[INET_ADDRSTRLEN];
        sockaddr sender;
        int len_sender = sizeof(sockaddr);
        ZeroMemory(&sender, sizeof(sender));
        ZeroMemory(subnetmask, sizeof(subnetmask));
        ZeroMemory(clientIP, sizeof(clientIP));

        int len_subnetmask = recvfrom(serverBroadcast, subnetmask, INET_ADDRSTRLEN, 0, &sender, &len_sender);
        if (len_subnetmask == SOCKET_ERROR) {
            std::cout << "recvfrom() failed: " << WSAGetLastError() << '\n';
            closesocket(serverBroadcast);
            socketAPI::cleanup();
            return;
        }

        sockaddr_in *tmp = reinterpret_cast<sockaddr_in*>(&sender);
        inet_ntop(AF_INET, &(tmp->sin_addr), clientIP, INET_ADDRSTRLEN);

        char hostname[256]{};
        if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
            std::cout << "gethostname() failed: " << WSAGetLastError() << '\n';
            closesocket(serverBroadcast);
            socketAPI::cleanup();
            return;
        }
        std::cout << hostname << '\n';

        addrinfo *result = nullptr, *ptr = nullptr, serverHints;
        ZeroMemory(&serverHints, sizeof(serverHints));
        serverHints.ai_family = AF_INET;
        serverHints.ai_socktype = SOCK_DGRAM;
        serverHints.ai_protocol = IPPROTO_UDP;

        int iResult = getaddrinfo(hostname, DEFAULT_PORT, &serverHints, &result);
        if (iResult != 0) {
            std::cout << "getaddrinfo failed: " << iResult << '\n';
            closesocket(serverBroadcast);
            socketAPI::cleanup();
            return;
        }

        for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
            if (ptr->ai_family == AF_INET && ptr->ai_protocol == IPPROTO_UDP && ptr->ai_socktype == SOCK_DGRAM) {
                sockaddr_in* sockaddr_ipv4 = reinterpret_cast<sockaddr_in*>(ptr->ai_addr); 
                char hostIP[INET_ADDRSTRLEN]; inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), hostIP, INET_ADDRSTRLEN); 
                // std::cout << "IP Address: " << hostIP << '\n';
                if (socketAPI::sameSubnet(clientIP, hostIP, subnetmask)) {
                    std::cout << "Broadcast from server completed!\n";
                    std::cout << "Client IPv4: " << clientIP << '\n';
                    std::cout << "Server IPv4: " << hostIP << '\n';
                    std::cout << "Subnet mask: " << subnetmask << '\n';

                    std::string string_hostIP(hostIP);
                    std::string string_hostname(hostname);
                    std::string respond = string_hostIP + std::string(1, ' ') + string_hostname;
                    std::cout << "respond: " << respond << '\n';
                    int iResult = sendto(serverBroadcast, (char*)respond.c_str(), respond.size(), 0, &sender, sizeof(sender));
                    if (iResult == SOCKET_ERROR) {
                        std::cout << "sendto() failed: " << WSAGetLastError() << '\n';
                        closesocket(serverBroadcast);
                        socketAPI::cleanup();
                        return;
                    }

                    closesocket(serverBroadcast);
                    isConnecting = true;
                    break;
                }
            }
        }
    }

    std::cout << "Shutting down...";
    socketAPI::cleanup();
}

bool serverSocket::initializeServer()
{
    addrinfo *result = nullptr, *ptr = nullptr, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    char hostname[256]{};
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        std::cout << "gethostname() failed: " << WSAGetLastError() << '\n';
        socketAPI::cleanup();
        return false;
    }
    // std::cout << hostname << '\n';

    int iResult = getaddrinfo(hostname, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed: " << iResult << '\n';
        socketAPI::cleanup();
        return false;
    }

    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << '\n';
        freeaddrinfo(result);
        socketAPI::cleanup();
        return false;
    }

    std::cout << "-----------------------------------------------------------\n";
    for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        if (ptr->ai_family == AF_INET) {
            sockaddr_in* sockaddr_ipv4 = reinterpret_cast<sockaddr_in*>(ptr->ai_addr); 
            char ipstr[INET_ADDRSTRLEN]; inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ipstr, INET_ADDRSTRLEN); 
            std::cout << "IP Address: " << ipstr << '\n';
        }
    }
    std::cout << "-----------------------------------------------------------\n";

    iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cout << "bind failed with error: " << WSAGetLastError() << '\n';
        freeaddrinfo(result);
        closesocket(listenSocket);
        socketAPI::cleanup();
        return false;
    }

    freeaddrinfo(result);
    return true;
}

bool serverSocket::listenClient() {
    #define MAXIMUM_CLIENT_CONNECT 5
    if (listen(listenSocket, MAXIMUM_CLIENT_CONNECT) == SOCKET_ERROR) {
        std::cout << "listen() failed: " << WSAGetLastError() << '\n';
        closesocket(listenSocket);
        socketAPI::cleanup();
        return false;
    }

    return true;
}

bool serverSocket::connectClient() {
    sockaddr addr_client;
    int addr_len(0);
    client = accept(listenSocket, NULL, NULL);
    if (client == INVALID_SOCKET) {
        std::cout << "accept() failed: " << WSAGetLastError() << '\n';
        closesocket(listenSocket);
        socketAPI::cleanup();
        return false;
    }

    std::cout << "close complete!\n";
    return true;
}

bool serverSocket::disconnect() {
    int iResult = shutdown(client, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cout << "shutdown() failed: " << WSAGetLastError() << '\n';
        closesocket(client);
        socketAPI::cleanup();
        return false;
    }
    return true;
}

bool serverSocket::serverCleanup() {
    int iResult = closesocket(listenSocket);
    if (iResult != 0) {
        std::cout << "closesocket() failed: " << WSAGetLastError() << '\n';
        return false;
    }
    
    iResult = closesocket(client);
    if (iResult != 0) {
        std::cout << "closesocket() failed: " << WSAGetLastError() << '\n';
        return false;
    }
    
    return true;
}
