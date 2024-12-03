#include "server_background.h"

P2P_serverSocket broadcast;
serverSocket server;
bool isRunning = true;

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
    switch (fdwCtrlType) {
        // CTRL-CLOSE: confirm that the user wants to exit.
    case (CTRL_CLOSE_EVENT || CTRL_LOGOFF_EVENT || CTRL_SHUTDOWN_EVENT):
        Beep(600, 200);
        isRunning = false;
        return TRUE;

    default:
        return FALSE;
    }
}

void initializeServer() {
    socketAPI::initializeSocket();
    if (SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
        printf("\nThe Control Handler is installed.\n");
        // while (1) {}
    }
}

int startServer() {
    broadcast.start();
    while(broadcast.get_true_IP_addr() == "xxx.xxx.xxx.xxx") {}
    server.initializeServer(broadcast.get_true_IP_addr());
    server.listenClient();

    while(isRunning) {
        if (!server.anyPendingConnection())
            continue;
        
        // Start connecting
        server.connectClient();
        broadcast.createMessage("CONNECTING");
        // Work
        std::string command, filepath;
        socketAPI::receiveMessage(server.client, command);
        Services::processCommand(command);
        socketAPI::sendFile(server.client, filepath);

        // auto folder = std::filesystem::temp_directory_path();
        // std::string filepath = folder.string();
        // Finish connecting
        server.disconnect();
        broadcast.createMessage("FREE");
    }
}

void endServer() {
    broadcast.createMessage("DELETE");
    broadcast.forceClose();
    socketAPI::cleanup();
}
