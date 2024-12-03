#pragma once

#include "socket/client.h"
#include "socket/socket.h"
#include "socket/p2p.h"
#include "app/services.h"
#include "app/UI.h"
#include "mail/Gmail.h"

extern std::string clientIPv4;
extern std::string subnetMask;
extern std::string username;
extern std::string password;
extern Gmail clientMail;
extern std::queue<std::string> queueQuery;
extern P2P_clientSocket broadcast;

std::vector<std::string> getInputInfo();

void initializeBroadcast(std::vector<std::string> &signinInput);
void initializeClient(std::vector<std::string> &signinInput);
void initializeUI(std::vector<std::string> &signinInput);

std::atomic<bool> isRunning(true);

int startBroadcast();
int startClient();
int startUI();