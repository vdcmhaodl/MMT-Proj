#include "socket/client.h"
#include "socket/socket.h"
#include "app/services.h"
#include "mail/Gmail.h"
#include "app/UI.h"

int main() {
    socketAPI::initializeSocket();

    int limit = 1;

    while(true) {
        clientSocket client;
        std::string localIP, subnetMask;
        std::cin >> localIP >> subnetMask;
        std::vector<std::pair<std::string, std::string>> listServer = client.broadcast(localIP, subnetMask);

        std::cout << "LIST IPv4s\n";
        for (auto &[serverIP, serverName]: listServer) {
            std::cout << serverIP << ": " << serverName << '\n';
        }

        std::string choosen_IP;
        std::cin >> choosen_IP;

        client.initializeClient(choosen_IP.c_str());

        std::string command;
        std::cin >> command;
        socketAPI::sendMessage(client.ConnectSocket, command);

        std::string file;
        std::cin >> file;
        socketAPI::receiveFile(client.ConnectSocket, file);

        client.disconnect();
        client.clientCleanup();

        if (--limit == 0) {
            break;
        }
    }

    socketAPI::cleanup();
}