#include "server.h"
#include <io.h>

// void serverSocket::broadcast() {
//     socketAPI::initializeSocket();

//     SOCKET serverBroadcast = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//     if (serverBroadcast == INVALID_SOCKET) {
//         std::cout << "socket() failed: " << WSAGetLastError() << '\n';
//         socketAPI::cleanup();
//         return;
//     }

//     struct sockaddr_in hints;
//     ZeroMemory(&hints, sizeof(sockaddr_in));
//     hints.sin_family = AF_INET;
//     hints.sin_port = htons(DEFAULT_BROADCAST);
//     hints.sin_addr.S_un.S_addr = ADDR_ANY;

//     int iResult = bind(serverBroadcast, (sockaddr*)&hints, sizeof(hints));
//     if (iResult == SOCKET_ERROR) {
//         std::cout << "bind() failed: " << WSAGetLastError() << '\n';
//         closesocket(serverBroadcast);
//         socketAPI::cleanup();
//         return;
//     }

//     char hostname[256]{};
//     if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
//         std::cout << "gethostname() failed: " << WSAGetLastError() << '\n';
//         closesocket(serverBroadcast);
//         socketAPI::cleanup();
//         return;
//     }
//     std::cout << hostname << '\n';

//     addrinfo *result = nullptr, *ptr = nullptr, serverHints;
//     ZeroMemory(&serverHints, sizeof(serverHints));
//     serverHints.ai_family = AF_INET;
//     serverHints.ai_socktype = SOCK_DGRAM;
//     serverHints.ai_protocol = IPPROTO_UDP;

//     int iResult = getaddrinfo(hostname, DEFAULT_PORT, &serverHints, &result);
//     if (iResult != 0) {
//         std::cout << "getaddrinfo failed: " << iResult << '\n';
//         closesocket(serverBroadcast);
//         socketAPI::cleanup();
//         return;
//     }

//     char subnetmask[INET_ADDRSTRLEN];
//     char clientIP[INET_ADDRSTRLEN];
//     char hostIP[INET_ADDRSTRLEN];
//     sockaddr sender;
//     int len_sender = sizeof(sockaddr);
//     ZeroMemory(&sender, sizeof(sender));
//     ZeroMemory(subnetmask, sizeof(subnetmask));

//     while(isDiscovered == false) {
//         int len_subnetmask = recvfrom(serverBroadcast, subnetmask, INET_ADDRSTRLEN, 0, &sender, &len_sender);
//         if (len_subnetmask == SOCKET_ERROR) {
//             std::cout << "Ahaha recvfrom() failed: " << WSAGetLastError() << '\n';
//             closesocket(serverBroadcast);
//             socketAPI::cleanup();
//             return;
//         }

//         socketAPI::getIPaddr(&sender, clientIP);
        
//         for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
//             socketAPI::getIPaddr(ptr->ai_addr, hostIP);
//             // std::cout << "IP Address: " << hostIP << '\n';
//             if (socketAPI::sameSubnet(clientIP, hostIP, subnetmask)) {
//                 std::string string_hostIP(hostIP);
//                 std::string string_hostname(hostname);
//                 std::string respond = string_hostIP + std::string(1, ' ') + string_hostname;
//                 std::cout << "respond: " << respond << '\n';
//                 int iResult = sendto(serverBroadcast, (char*)respond.c_str(), respond.size(), 0, &sender, sizeof(sender));
//                 if (iResult == SOCKET_ERROR) {
//                     std::cout << "sendto() failed: " << WSAGetLastError() << '\n';
//                     closesocket(serverBroadcast);
//                     socketAPI::cleanup();
//                     return;
//                 }

//                 closesocket(serverBroadcast);
//                 isDiscovered = true;
//                 break;
//             }
//         }
//     }

//     std::cout << "Shutting down...";
//     socketAPI::cleanup();
// }

bool serverSocket::initializeServer(std::string IP) {
    // addrinfo *result = nullptr, *ptr = nullptr;
    // addrinfo hints;

    // ZeroMemory(&hints, sizeof(hints));
    // hints.ai_family = AF_INET;
    // hints.ai_protocol = IPPROTO_TCP;
    // hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI_PASSIVE;

    char hostname[256]{};
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        std::cout << "gethostname() failed: " << WSAGetLastError() << '\n';
        socketAPI::cleanup();
        return false;
    }
    // std::cout << hostname << '\n';

    // int iResult = getaddrinfo(hostname, DEFAULT_PORT, &hints, &result);
    // if (iResult != 0) {
    //     std::cout << "getaddrinfo failed: " << iResult << '\n';
    //     socketAPI::cleanup();
    //     return false;
    // }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // if (listenSocket == INVALID_SOCKET) {
    //     std::cout << "Error at socket(): " << WSAGetLastError() << '\n';
    //     freeaddrinfo(result);
    //     socketAPI::cleanup();
    //     return false;
    // }

    // std::cout << "-----------------------------------------------------------\n";
    // for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
    //     if (ptr->ai_family == AF_INET) {
    //         sockaddr_in* sockaddr_ipv4 = reinterpret_cast<sockaddr_in*>(ptr->ai_addr); 
    //         char ipstr[INET_ADDRSTRLEN]; inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ipstr, INET_ADDRSTRLEN); 
    //         std::cout << "IP Address: " << ipstr << '\n';
    //     }
    // }
    // std::cout << "-----------------------------------------------------------\n";

    sockaddr_in hints;
    hints.sin_family = AF_INET;
    hints.sin_port = htons(DEFAULT_PORT);
    hints.sin_addr.S_un.S_addr = socketAPI::getBinaryIP((char*)IP.c_str());

    int optval = 1;
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));

    int iResult = bind(listenSocket, (sockaddr*)&hints, sizeof(hints));
    if (iResult == SOCKET_ERROR) {
        std::cout << "bind failed with error: " << WSAGetLastError() << '\n';
        // freeaddrinfo(result);
        closesocket(listenSocket);
        socketAPI::cleanup();
        return false;
    }

    // freeaddrinfo(result);
    return true;
}

bool serverSocket::listenClient() {
    #define MAXIMUM_CLIENT_CONNECT 1
    if (listen(listenSocket, MAXIMUM_CLIENT_CONNECT) == SOCKET_ERROR) {
        std::cout << "listen() failed: " << WSAGetLastError() << '\n';
        closesocket(listenSocket);
        socketAPI::cleanup();
        return false;
    }

    // Non-blocking connect()
    unsigned long mode = 1; 
    ioctlsocket(listenSocket, FIONBIO, &mode);

    return true;
}

// bool timeout_occurred = false;

// // Timer ID 
// #define TIMER_ID 1 
// // Signal handler for timeout 
// void handle_alarm(int sig) { 
//     timeout_occurred = true;
// } 
// // Timer callback function 
// void CALLBACK TimerProc(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime) { 
//     raise(SIGINT); 
// }

bool serverSocket::anyPendingConnection() {
    client = accept(listenSocket, NULL, NULL);

    // std::cout << (client == INVALID_SOCKET) << '\n';

    if (client == INVALID_SOCKET) { 
        if (WSAGetLastError() == WSAEWOULDBLOCK) {
            // No pending connections; continue loop 
            std::osyncstream(std::cout) << std::this_thread::get_id() << ' ' << "No pending connections. Checking again..." << std::endl; 
            // Sleep for a short time before checking again continue; 
            } else { 
                // An actual error occurred 
            std::osyncstream(std::cerr) << std::this_thread::get_id() << ' ' << "accept() failed: " << WSAGetLastError() << std::endl; 
        }
        
        return false;
    }
    return true;
}

bool serverSocket::connectClient() {
    // std::signal(SIGINT, handle_alarm); 
    // UINT_PTR timerId = SetTimer(NULL, TIMER_ID, timeListen * 1000, (TIMERPROC)TimerProc); 
    // if (timerId == 0) { 
    //     closesocket(listenSocket);
    //     socketAPI::cleanup();
    //     return false;
    // }

    // sockaddr addr_client;
    // int addr_len(0);
    
    // if (timeout_occurred) {
    //     return false;
    // }

    return true;
}

bool serverSocket::disconnect() {
    int iResult = shutdown(client, SD_BOTH);
    if (iResult == SOCKET_ERROR) {
        std::cout << "shutdown() failed: " << WSAGetLastError() << '\n';
        closesocket(client);
        socketAPI::cleanup();
        return false;
    }

    iResult = closesocket(client);
    if (iResult != 0) {
        std::cout << "closesocket() failed: " << WSAGetLastError() << '\n';
        return false;
    }

    client = INVALID_SOCKET;
    return true;
}

bool serverSocket::cleanup() {
    int iResult;

    if (listenSocket != INVALID_SOCKET){
        iResult = closesocket(listenSocket);
        if (iResult != 0) {
            std::cout << "closesocket() failed: " << WSAGetLastError() << '\n';
            return false;
        }
    }

    if (client != INVALID_SOCKET){
        iResult = closesocket(client);
        if (iResult != 0) {
            std::cout << "closesocket() failed: " << WSAGetLastError() << '\n';
            return false;
        }
    }
    return true;
}
