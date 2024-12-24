#include "server.h"
#include <io.h>

bool serverSocket::initializeServer(std::string IP) {
    char hostname[256]{};
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        std::osyncstream(std::cout) << "gethostname() failed: " << WSAGetLastError() << '\n';
        socketAPI::cleanup();
        return false;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in hints;
    hints.sin_family = AF_INET;
    hints.sin_port = htons(DEFAULT_PORT);
    hints.sin_addr.S_un.S_addr = socketAPI::getBinaryIP((char*)IP.c_str());

    int optval = 1;
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));

    int iResult = bind(listenSocket, (sockaddr*)&hints, sizeof(hints));
    if (iResult == SOCKET_ERROR) {
        std::osyncstream(std::cout) << "bind failed with error: " << WSAGetLastError() << '\n';
        closesocket(listenSocket);
        socketAPI::cleanup();
        return false;
    }

    return true;
}

bool serverSocket::listenClient() {
    #define MAXIMUM_CLIENT_CONNECT 1
    if (listen(listenSocket, MAXIMUM_CLIENT_CONNECT) == SOCKET_ERROR) {
        std::osyncstream(std::cout) << "listen() failed: " << WSAGetLastError() << '\n';
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


    if (client == INVALID_SOCKET) { 
        if (WSAGetLastError() == WSAEWOULDBLOCK) {
            // std::osyncstream(std::cout) << std::this_thread::get_id() << ' ' << "No pending connections. Checking again..." << std::endl; 
            // Sleep for a short time before checking again continue; 
            } else { 
                // An actual error occurred 
            std::osyncstream(std::cerr) << std::this_thread::get_id() << ' ' << "accept() failed: " << WSAGetLastError() << std::endl; 
        }
        
        return false;
    }
    return true;
}

bool serverSocket::disconnect() {
    int iResult = shutdown(client, SD_BOTH);
    if (iResult == SOCKET_ERROR) {
        std::osyncstream(std::cout) << "shutdown() failed: " << WSAGetLastError() << '\n';
        closesocket(client);
        socketAPI::cleanup();
        return false;
    }

    iResult = closesocket(client);
    if (iResult != 0) {
        std::osyncstream(std::cout) << "closesocket() failed: " << WSAGetLastError() << '\n';
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
            std::osyncstream(std::cout) << "closesocket() failed: " << WSAGetLastError() << '\n';
            return false;
        }
    }

    if (client != INVALID_SOCKET){
        iResult = closesocket(client);
        if (iResult != 0) {
            std::osyncstream(std::cout) << "closesocket() failed: " << WSAGetLastError() << '\n';
            return false;
        }
    }
    return true;
}
