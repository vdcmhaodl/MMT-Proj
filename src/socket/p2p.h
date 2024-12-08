#pragma once
#include "socket.h"
#include "ipconfig.h"
#include <tuple>

extern std::string multicastIP;

struct P2P_Socket {
    std::atomic<bool> debugger = false;
    std::atomic<bool> isRunning = true;
    std::shared_mutex mtx;

    SOCKET sendSocket = INVALID_SOCKET;
    SOCKET recvSocket = INVALID_SOCKET;
    ip_mreq JoinReq;

    char hostname[256];
    char* IP_addr = NULL;
    char* subnetMask = NULL;

    addrinfo broadcastHints;
    struct sockaddr_in recvHints;
    struct sockaddr_in sendHints;

    concurrent_queue<std::string> queueMessage;

    bool getMessage(std::string &msg);
    void addMessage(std::string msg);
    // void popMessage();
    
    P2P_Socket();
    virtual ~P2P_Socket();

    virtual bool makeRespond(std::string IPsender, std::string msg, std::string &respond) = 0;

    void initialize(char* IP_addr = NULL, char* subnetMask = NULL);
    void start(std::string init_msg);
    virtual void start() = 0;
    void startSend();
    void startRecv();
    virtual void forceClose();
};

struct P2P_serverSocket : public P2P_Socket {
    std::atomic<bool> isConnecting = false;
    std::string true_IP_addr = "xxx.xxx.xxx.xxx";

    bool getState();
    void flipState();
    std::string get_true_IP_addr();
    void assgin_true_IP_addr(std::string IPaddr);

    bool makeRespond(std::string IPsender, std::string msg, std::string &respond);
    void start();

    void createMessage(std::string status);
    void forceClose();
};

struct P2P_clientSocket : public P2P_Socket {
    ListIPData listIP;
    concurrent_queue<IPStatusMsg> updateStatusIP;

    bool makeRespond(std::string IPsender, std::string msg, std::string &respond);
    void start();

    void forceClose();
};