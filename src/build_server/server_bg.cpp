#include "server_bg.h"

P2P_serverSocket broadcast;
serverSocket server;
bool isRunning = true;
bool fullyInitialize = false;

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
    switch (fdwCtrlType) {
    case CTRL_C_EVENT:
    case CTRL_SHUTDOWN_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_CLOSE_EVENT:
        // Beep(6000, 2000);
        isRunning = false;
        MessageBoxA(GetConsoleWindow(), "CLOSING...", "Post message", MB_OK);
        if (!fullyInitialize) {
            endServer();
        }
        return TRUE;
    default:
        return FALSE;
    }
}

void initializeServer() {
    socketAPI::initializeSocket();
    broadcast.initialize();
    if (SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
        printf("\nThe Control Handler is installed.\n");
        // while (1) {}
    }
}

int startServer() {
    broadcast.start();
    // std::osyncstream(std::cout) << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA " << std::this_thread::get_id() << ' ' << broadcast.get_true_IP_addr() << '\n';
    while(broadcast.get_true_IP_addr() == "xxx.xxx.xxx.xxx") {
        // std::osyncstream(std::cout) << broadcast.get_true_IP_addr() << '\n';
    }
    // std::osyncstream(std::cout) << std::this_thread::get_id() << "IP address: " << broadcast.get_true_IP_addr() << '\n';
    std::cout << server.initializeServer(broadcast.get_true_IP_addr()) << '\n';
    std::cout << server.listenClient() << '\n';
    fullyInitialize = true;
    // std::cout << "Finish setup\n";

    auto time_start = std::chrono::high_resolution_clock::now();
    while(isRunning) {
        // auto now = std::chrono::high_resolution_clock::now();
        // std::chrono::duration<long double> timeElapsed = now - time_start;
        // if (timeElapsed.count() < 0.5) { 
        //     continue;
        // }
        // else if (timeElapsed.count() > 0.5) {
        //     time_start = now;
        //     if (!server.anyPendingConnection()) {
        //         continue;
        //     }
        // }
        if (!server.anyPendingConnection()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(750));
            continue;
        }

        std::cout << "NEW CONNECTION\n";
        // Start connecting
        // server.connectClient();
        broadcast.createMessage(STATUS::IN_CONNECTION_SOCKET);
        // Work
        executeCommand();
        // auto folder = std::filesystem::temp_directory_path();
        // std::string filepath = folder.string();
        // Finish connecting
        std::cout << "FINISH\n";
        server.disconnect();
        broadcast.createMessage(STATUS::FREE_SOCKET);
    }

    return 0;
}

void executeCommand() {
    std::string stringCommand, filepath;
    socketAPI::receiveMessage(server.client, stringCommand);
    std::osyncstream(std::cout) << "RECEIVE: " << stringCommand << '\n';
    Command command;
    command.construct(stringCommand);
    // TODO: Execute this "command" variable.
    std::string filename;





    
    // Expect sending the result file with name "filename"
    socketAPI::sendFile(server.client, filename);
    // Services::processCommand(command);
    // socketAPI::sendFile(server.client, filepath);
    // socketAPI::sendMessage(server.client, command);
    // std::this_thread::sleep_for(std::chrono::microseconds(2500));
}

void endServer() {
    // MessageBoxA(GetConsoleWindow(), "Start free resource!", "FINISH", MB_OK);
    broadcast.forceClose();
    server.cleanup();
    socketAPI::cleanup();
    MessageBoxA(GetConsoleWindow(), "Free resource completed!", "FINISH", MB_OK);
}
