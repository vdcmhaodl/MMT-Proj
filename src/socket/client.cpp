#include "client.h"

bool clientSocket::initializeClient(const char *serverIP) {
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        std::osyncstream(std::cout) << "Error at socket(): " << WSAGetLastError() << '\n';
        socketAPI::cleanup();
        return false;
    }

    struct sockaddr_in hints;
    hints.sin_port = htons(DEFAULT_PORT);
    hints.sin_family = AF_INET;
    hints.sin_addr.s_addr = socketAPI::getBinaryIP((char*)serverIP);

    int iResult = connect(ConnectSocket, (struct sockaddr*)&hints, sizeof(hints));
    if (iResult == SOCKET_ERROR) {
        std::osyncstream(std::cout) << "connect() failed: " << WSAGetLastError() << '\n';
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    return true;
}

bool clientSocket::disconnect() {
    int iResult = shutdown(ConnectSocket, SD_BOTH);
    if (iResult == SOCKET_ERROR) {
        std::osyncstream(std::cout) << "shutdown() failed: " << WSAGetLastError() << '\n';
        closesocket(ConnectSocket);
        socketAPI::cleanup();
        return false;
    }
    return true;
}

bool clientSocket::clientCleanup() {
    int iResult = closesocket(ConnectSocket);
    if (iResult != 0) {
        std::osyncstream(std::cout) << "closesocket() failed: " << WSAGetLastError() << '\n';
        return false;
    }
    return true;
}
