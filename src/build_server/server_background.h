#include "socket/server.h"
#include "socket/socket.h"
#include "socket/p2p.h"
#include "app/services.h"
#include <assert.h>

extern P2P_serverSocket broadcast;
extern serverSocket server;
extern bool isRunning;

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);
void initializeServer();
int startServer();
void endServer();