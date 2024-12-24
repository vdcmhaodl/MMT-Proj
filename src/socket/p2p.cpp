#include "p2p.h"

std::string multicastIP = "229.7.17.37";

bool P2P_Socket::getMessage(std::string &msg) {
    return queueMessage.try_pop(msg);
}

void P2P_Socket::addMessage(std::string msg) {
    queueMessage.push(msg);
}

P2P_Socket::P2P_Socket() {
    socketAPI::initializeSocket();
}

P2P_Socket::~P2P_Socket() {
    socketAPI::cleanup();
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

    int optVal = 1; 
    setsockopt(sendSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optVal, sizeof(optVal));
    setsockopt(recvSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optVal, sizeof(optVal));

    //Set target address 
    ZeroMemory(&sendHints, sizeof(sendHints));
    sendHints.sin_family = AF_INET;
    sendHints.sin_port = htons(DEFAULT_BROADCAST);
    sendHints.sin_addr.s_addr = socketAPI::getBinaryIP((char*)multicastIP.c_str());

    ZeroMemory(&recvHints, sizeof(recvHints));
    recvHints.sin_family = AF_INET;
    recvHints.sin_port = htons(DEFAULT_BROADCAST);
    recvHints.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket
    bind(recvSocket, (struct sockaddr*)&recvHints, sizeof(recvHints));

    // Membership setting
    JoinReq.imr_multiaddr.s_addr = socketAPI::getBinaryIP((char*)multicastIP.c_str());
    JoinReq.imr_interface.s_addr = htonl(INADDR_ANY);

    // Join membership
    setsockopt(recvSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&JoinReq, sizeof(JoinReq));
}

void P2P_Socket::start(std::string init_msg) {
    addMessage(init_msg);
    // std::osyncstream(std::cout) << std::this_thread::get_id() << ' ' << "INIT MSG: " << init_msg << '\n';
    std::thread send(&P2P_serverSocket::startSend, this);
    std::thread recv(&P2P_serverSocket::startRecv, this);
    send.detach();
    recv.detach();
}

void P2P_Socket::startSend() {
    // std::osyncstream(std::cout) << std::this_thread::get_id() << ' ' << "Start send message\n";
    while(isRunning) {
        std::string msg;
        if (getMessage(msg)) {
            int iResult = sendto(sendSocket, msg.c_str(), msg.size(), 0, (struct sockaddr*)&sendHints, sizeof(sendHints));
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
    // std::osyncstream(std::cout) << std::this_thread::get_id() << ' ' << "Start receive message\n";
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
        // std::osyncstream(std::cout) << std::this_thread::get_id() << ' ' << IPsender << ": " << msg << '\n';

        if (!debugger) {
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
    
    std::osyncstream(std::cout) << std::this_thread::get_id() << ' ' << "Fully clean!\n";
}

/////////////////////////////////////////////////////////////////

bool P2P_serverSocket::getState() {
    return isConnecting;
}

void P2P_serverSocket::flipState() {
    isConnecting = isConnecting ^ 1;
}

std::string P2P_serverSocket::get_true_IP_addr() {
    // std::osyncstream(std::cout) << "TRUE IP\n";
    std::unique_lock lock(mtx);
    return true_IP_addr;
}

void P2P_serverSocket::assgin_true_IP_addr(std::string IPaddr) {
    std::unique_lock lock(mtx);
    true_IP_addr = IPaddr;
}

bool P2P_serverSocket::makeRespond(std::string IPsender, std::string msg, std::string &respond) {
    // std::osyncstream(std::cout) << "Receive msg: " << msg << '\n';
    if (socketAPI::isServerMessage(msg)) {
        // std::osyncstream(std::cout) << "This is server message anyway " << '\n';
        return false;
    } 

    std::string subnetMask;
    socketAPI::decipherClientMessage(msg, subnetMask);

    if (get_true_IP_addr() == "xxx.xxx.xxx.xxx") {
        std::string ret = socketAPI::findSuitableIP(hostname, (char*)(std::to_string(DEFAULT_PORT)).c_str(), &broadcastHints, IPsender, subnetMask);
        if (ret.empty()) {
            ret = socketAPI::findSuitableIP(IPsender, subnetMask);
        }
        // std::osyncstream(std::cout) << "Found IP: " << ret << '\n';
        assgin_true_IP_addr(ret);
    }
    respond = socketAPI::createServerMessage(get_true_IP_addr(), hostname, getState() ? STATUS::IN_CONNECTION_SOCKET : STATUS::FREE_SOCKET);
    return true;
}

void P2P_serverSocket::start() {
    P2P_Socket::start(socketAPI::createServerMessage(get_true_IP_addr(), hostname, STATUS::CREATE_SOCKET));
}

void P2P_serverSocket::createMessage(std::string status) {
    addMessage(socketAPI::createServerMessage(get_true_IP_addr(), hostname, status));
}

void P2P_serverSocket::forceClose() {
    // std::osyncstream(std::cout) << std::this_thread::get_id() << ' ' << ' ' << "Closing...........\n";
    addMessage(socketAPI::createServerMessage(get_true_IP_addr(), hostname, STATUS::DELETE_SOCKET));
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
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
        // socketAPI::update_list(listIP, IP, hostname, status);
        if (socketAPI::update_list(listIP, IP, hostname, status)) {
            updateStatusIP.push(std::make_tuple(IP, hostname, status));
        }
        
        return false;
    }
}

void P2P_clientSocket::start() {
    // std::osyncstream(std::cout) << "START " << subnetMask << '\n';
    P2P_Socket::start(socketAPI::createClientMessage(subnetMask));
}

void P2P_clientSocket::forceClose() {
    P2P_Socket::forceClose();
}
