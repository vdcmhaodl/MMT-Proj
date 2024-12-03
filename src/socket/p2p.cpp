#include "p2p.h"

bool P2P_Socket::getMessage(std::string &msg) {
    return queueMessage.try_pop(msg);
}

void P2P_Socket::addMessage(std::string msg) {
    queueMessage.push(msg);
}

// void P2P_Socket::popMessage() {
//     std::unique_lock lock(mtx);
//     queueMessage.pop();
// }

P2P_Socket::P2P_Socket() {
    socketAPI::initializeSocket();
}

P2P_Socket::~P2P_Socket() {
    socketAPI::cleanup();
}

bool P2P_Socket::makeRespond(std::string IPsender, std::string msg, std::string &respond) {
    return false;
}

void P2P_Socket::initialize(char *IP_addr, char *subnetMask) {
    this->IP_addr = IP_addr;
    this->subnetMask = subnetMask;

    sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    recvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    gethostname(hostname, sizeof(hostname));

    ZeroMemory(&broadcastHints, sizeof(broadcastHints));
    broadcastHints.ai_family = AF_INET;
    broadcastHints.ai_socktype = SOCK_DGRAM;
    broadcastHints.ai_protocol = IPPROTO_UDP;

    int broadcastEnable = 1; 
    setsockopt(sendSocket, SOL_SOCKET, SO_BROADCAST, (char*)&broadcastEnable, sizeof(broadcastEnable));

    ZeroMemory(&recvHints, sizeof(sockaddr_in));
    recvHints.sin_family = AF_INET;
    recvHints.sin_port = htons(DEFAULT_BROADCAST);
    recvHints.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    ZeroMemory(&sendHints, sizeof(sockaddr_in));
    sendHints.sin_family = AF_INET;
    sendHints.sin_port = htons(DEFAULT_BROADCAST);
    sendHints.sin_addr.S_un.S_addr = (IP_addr != NULL && subnetMask != NULL ? 
        socketAPI::getBinaryBroadcast(IP_addr, subnetMask, NULL) : 
        socketAPI::getBinaryAvailableIP(hostname, (char*)(std::to_string(DEFAULT_PORT)).c_str(), &broadcastHints));
        
    int reuse = 1; 
    if (setsockopt(recvSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) == -1) {
        perror("setsockopt");
    }
    if (bind(recvSocket, (sockaddr*)&recvHints, sizeof(recvHints)) == SOCKET_ERROR) {
        std::cout << "bind() failed: " << WSAGetLastError() << '\n';
    }
}

void P2P_Socket::start(std::string init_msg) {
    addMessage(init_msg);
    std::cout << "Init message: " << init_msg << '\n';
    std::thread send(&P2P_serverSocket::startSend, this);
    std::thread recv(&P2P_serverSocket::startRecv, this);
    send.detach();
    recv.detach();
}

void P2P_Socket::startSend() {
    std::cout << "Start send message\n";
    while(isRunning) {
        std::string msg;
        if (getMessage(msg)) {
            int iResult = sendto(sendSocket, (char*)msg.c_str(), msg.size(), 0, (sockaddr*)&sendHints, sizeof(sendHints));
            if (iResult == SOCKET_ERROR) {
                break;
            }
            else if (iResult == 0) {
                continue;
            }
        }
    }
}

void P2P_Socket::startRecv() {
    std::cout << "Start receive message\n";
    sockaddr sender;
    int len_sender = sizeof(sockaddr);
    ZeroMemory(&sender, sizeof(sender));
    char msg[256];

    while(isRunning) {
        ZeroMemory(msg, sizeof(msg));
        int len_msg = recvfrom(recvSocket, msg, sizeof(msg), 0, &sender, &len_sender);
        if (len_msg == SOCKET_ERROR) {
            break;
        }
        else if (len_msg == 0) {
            continue;
        }

        char IPsender[INET_ADDRSTRLEN];
        socketAPI::getIPaddr(&sender, IPsender);

        if (debugger) {
            std::cout << IPsender << ": " << msg << '\n';
        }
        else {
            std::string respond;
            if (makeRespond(IPsender, msg, respond)) {
                addMessage(respond);
            }
        }
    }
}

void P2P_Socket::forceClose() {
    isRunning = false;

    shutdown(recvSocket, SD_BOTH);
    closesocket(recvSocket);

    shutdown(sendSocket, SD_BOTH);    
    closesocket(sendSocket);
    
    std::cout << "Fully clean!\n";
}

/////////////////////////////////////////////////////////////////

bool P2P_serverSocket::getState() {
    return isConnecting;
}

void P2P_serverSocket::flipState() {
    isConnecting = isConnecting ^ 1;
}

std::string P2P_serverSocket::get_true_IP_addr() {
    std::shared_lock lock(mtx);
    return true_IP_addr;
}

void P2P_serverSocket::assgin_true_IP_addr(std::string IPaddr) {
    std::unique_lock lock(mtx);
    true_IP_addr = IPaddr;
}

bool P2P_serverSocket::makeRespond(std::string IPsender, std::string msg, std::string &respond) {
    if (socketAPI::isServerMessage(msg)) {
        return false;
    } 

    std::string subnetMask;
    socketAPI::decipherClientMessage(msg, subnetMask);

    if (get_true_IP_addr() == "xxx.xxx.xxx.xxx") {
        assgin_true_IP_addr(socketAPI::findSuitableIP(hostname, (char*)(std::to_string(DEFAULT_PORT)).c_str(), &broadcastHints, IPsender, subnetMask));
    }
    respond = socketAPI::createServerMessage(get_true_IP_addr(), hostname, getState() ? "CONNECTING" : "FREE");
    return true;
}

void P2P_serverSocket::start() {
    P2P_Socket::start(socketAPI::createServerMessage(get_true_IP_addr(), hostname, "NEW"));
}

void P2P_serverSocket::createMessage(std::string status) {
    addMessage(socketAPI::createServerMessage(get_true_IP_addr(), hostname, status));
}

void P2P_serverSocket::forceClose()
{
    addMessage(socketAPI::createServerMessage(get_true_IP_addr(), hostname, "DELETE"));
    P2P_Socket::forceClose();
}

///////////////////////////////////////////////////////////////////////////////

bool P2P_clientSocket::makeRespond(std::string IPsender, std::string msg, std::string &respond) {
    if (socketAPI::isClientMessage(msg)) {
        return false;
    }

    std::string IP;
    std::string hostname;
    std::string status;
    socketAPI::decipherServerMessage(msg, IP, hostname, status);

    if (IP == "xxx.xxx.xxx.xxx") {
        // send message 
        respond = socketAPI::createClientMessage(subnetMask);
        return true;
    }
    else {
        socketAPI::update_list(listIP, IP, hostname, status);
        return false;
    }
}

void P2P_clientSocket::start() {
    P2P_Socket::start(socketAPI::createClientMessage(subnetMask));
}

void P2P_clientSocket::forceClose() {
    P2P_Socket::forceClose();
}
