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
        // printf("\nThe Control Handler is installed.\n");
        // while (1) {}
    }
}

int startServer() {
    broadcast.start();
    while(broadcast.get_true_IP_addr() == "xxx.xxx.xxx.xxx") {}

    server.initializeServer(broadcast.get_true_IP_addr());
    server.listenClient();
    fullyInitialize = true;
    std::osyncstream(std::cout) << "INITIALIZE COMPLETED!\nIP address: " << broadcast.get_true_IP_addr() << "\n";

    while(isRunning) {
        std::osyncstream(std::cout) << "WATING FOR CONNECTION...\n";
        if (!server.anyPendingConnection()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(750));
            continue;
        }

        std::osyncstream(std::cout) << "NEW CONNECTION!\n";
        std::osyncstream(std::cout) << "Time: " << Services::getCurrentTimeString() << '\n';
        broadcast.createMessage(STATUS::IN_CONNECTION_SOCKET);

        executeCommand();

        std::osyncstream(std::cout) << "FINISH CONNECTION!\n";
        server.disconnect();
        broadcast.createMessage(STATUS::FREE_SOCKET);
    }

    return 0;
}

void executeCommand() {
    std::string stringCommand, filepath;
    socketAPI::receiveMessage(server.client, stringCommand);
    // std::osyncstream(std::cout) << "RECEIVE: " << stringCommand << '\n';
    Command command;
    command.construct(stringCommand);
    command.print();
    // TODO: Execute this "command" variable.

    std::vector<std::string> listFilename;

    auto ServiceFunc = Services::servicesMap.find({command.type, command.action});
    if (ServiceFunc == Services::servicesMap.end()) {
        // std::string failure = Command::generateFilepath(10, ".txt");

        // std::ofstream fout (failure.c_str());
        // std::osyncstream(std::cerr) << "Invalid command\n";
        // fout.close();

        std::string failure = "Invalid command\n";
        listFilename.push_back(failure);
    }
    else {
        listFilename = (ServiceFunc->second)(command);
    }

    std::string numFile = std::to_string(listFilename.size());
    socketAPI::sendMessage(server.client, numFile);

    std::string message[2] = {"file", "content"};
    for (auto filename: listFilename) {
        // std::osyncstream(std::cout) << "filename: " << filename << " " << std::filesystem::exists(filename) << "\n";
        if (std::filesystem::exists(filename)) {
            socketAPI::sendMessage(server.client, message[0]);
            socketAPI::sendFile(server.client, filename);
            Services::deleteFile(filename);
        }
        else {
            socketAPI::sendMessage(server.client, message[1]);
            socketAPI::sendMessage(server.client, filename);
        }
    }
}

void endServer() {
    broadcast.forceClose();
    server.cleanup();
    socketAPI::cleanup();
    MessageBoxA(GetConsoleWindow(), "Free resource completed!", "FINISH", MB_OK);
}
