#include "../socket/server.h"
#include "../socket/socket.h"
#include "../socket/p2p.h"
#include "../app/services.h"
#include <iostream>
#include <assert.h>
#include <filesystem>

extern P2P_serverSocket broadcast;
extern serverSocket server;
extern bool isRunning;
extern bool fullyInitialize;

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);
void initializeServer();
int startServer();
void executeCommand();
void endServer();