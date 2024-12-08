#pragma once

#include "../socket/client.h"
#include "../socket/socket.h"
#include "../socket/p2p.h"
#include "../app/services.h"
#include "../mail/Gmail.h"
#include "../share_queue.h"

extern std::string clientIPv4;
extern std::string subnetMask;
extern std::string username;
extern std::string password;
extern GmailAccount clientMail;
extern concurrent_queue<std::string> queueQuery;
extern ListIPData ListIP;
extern P2P_clientSocket broadcast;

std::vector<std::string> getInputInfo();

void initializeBroadcast(std::vector<std::string> &signinInput);
void initializeClient(std::vector<std::string> &signinInput);
void initializeMail(std::vector<std::string> &signinInput);
void initializeUI(std::vector<std::string> &signinInput);

extern std::atomic<bool> isRunning;

int startBroadcast();
int startClient();
int startMail();
int startUI();