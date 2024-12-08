#pragma once

#include "../socket/client.h"
#include "../socket/socket.h"
#include "../socket/p2p.h"
#include "../app/services.h"
#include "UI.h"
#include "../mail/Gmail.h"
#include "../share_queue.h"

class FullCommand : public Command {
    
};

extern std::string clientIPv4;
extern std::string subnetMask;
extern std::string username;
extern std::string password;
extern GmailAccount clientMail;
extern concurrent_queue<std::string> queueQuery;
extern concurrent_queue<Email> queueMail;
extern concurrent_queue<std::string> queueLog;
extern ListIPData ListIP;
extern P2P_clientSocket broadcast;
extern MainWindow win;

std::vector<std::string> getInputInfo();

void initializeBroadcast(std::vector<std::string> &signinInput);
void initializeClient(std::vector<std::string> &signinInput);
void initializeMail(std::vector<std::string> &signinInput);
void initializeUI(std::vector<std::string> &signinInput);

extern std::atomic<bool> isRunning;

extern std::thread::id idThreadBroadcast;
extern std::thread::id idThreadClient;
extern std::thread::id idThreadMail;
extern std::thread::id idThreadUI;

int startBroadcast();
int startClient();
int startMail();
int startUI();