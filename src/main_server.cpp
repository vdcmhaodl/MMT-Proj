#include "socket/server.h"
#include "socket/socket.h"
#include "app/services.h"
#include <assert.h>

int main() {
    socketAPI::initializeSocket();

    int limit = 1;

    while(true) {
        serverSocket server;
        if (!server.isDiscovered) {
            std::cout << "START BROADCAST\n";
            server.broadcast();
            server.isDiscovered = true;
        }
        assert(server.isDiscovered);
        std::cout << "DISCOVERED! Wait for connecting...\n";

        timeout_occurred = false;

        server.initializeServer();
        server.listenClient();
        bool connecting = server.connectClient();
        if (!connecting) {
            continue;
        }

        std::string command;
        socketAPI::receiveMessage(server.client, command);

        auto folder = std::filesystem::temp_directory_path();
        std::string filename = "result.txt";
        folder /= filename;
        std::string filepath = folder.string();
        command += std::string(1, ' ') + filepath;

        Services::processCommand(command);
        
        socketAPI::sendFile(server.client, filepath);

        server.disconnect();
        server.serverCleanup();

        if (--limit == 0) {
            break;
        }
    }

    socketAPI::cleanup();
}