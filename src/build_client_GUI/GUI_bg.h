#pragma once

#include "../socket/client.h"
#include "../socket/socket.h"
#include "../socket/p2p.h"
#include "../app/services.h"
#include "UI.h"
#include "../mail/Gmail.h"
#include "../share_queue.h"
#include "../app/services.h"
#include "mediator.h"
#include <any>

class FullCommand {
public:
    Command command;
    Email email;
private:
};

class AppMediator : public Mediator {
public:
    std::string clientIPv4;
    std::string subnetMask;
    std::string username;
    std::string password;
    concurrent_queue<std::string> queueQuery;
    concurrent_queue<FullCommand> queueCommand;
    concurrent_queue<std::string> queueLog;
    ListIPData ListIP;
private:
};

class Broadcast : public Participant {
protected:
    std::atomic<bool> isRunning;
   
    ListIPData listIP;
public: 
    P2P_clientSocket broadcast;
    Broadcast();
    Broadcast(Mediator* mediator, std::string name);
    void initialize(std::vector<std::string> &signinInput);
    void start();

    void Receive(std::string msg);
    void Receive(std::string msg, std::string sender);
};

class Gmail : public Participant {
protected:
    GmailAccount clientMail;
    std::atomic<bool> isRunning;
    concurrent_queue<Mail> queueMail;
public:
    Gmail();
    Gmail(Mediator* mediator, std::string name);
    void initialize(std::vector<std::string> &signinInput);
    void start();

    Command constructCommand(Mail &mail);

    void Receive(std::string msg);
    void Receive(std::any *ptr);
    void Receive(std::any *ptr, std::string type);

};

class Client : public Participant {
protected:
    std::atomic<bool> isRunning;
    std::atomic<bool> IPdata;
    ListViewData listIP;
    concurrent_queue<FullCommand> queueCommand;
public:
    Client();
    Client(Mediator* mediator, std::string name);
    void initialize(std::vector<std::string> &signinInput);
    void start();
    void resolveCommand(FullCommand FC);

    void Receive(std::string msg);
    void Receive(std::any *ptr);
    void Receive(std::any *ptr, std::string type);
};

std::vector<std::string> getInputInfo();