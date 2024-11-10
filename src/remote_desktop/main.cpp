#include "server.h"
#include "client.h"

int main(int argc, char** argv) {
    socketAPI::initializeSocket();

    int type;
    std::cin >> type;
    std::cin.ignore();
    if (type == 0) {
        serverSocket server;
        server.broadcast();
        // std::cout << "initializeServer() " << server.initializeServer() << '\n';
        // std::cout << "listenClient(): " << server.listenClient() << '\n';
        // std::cout << "connectClient(): " << server.connectClient() << '\n';

        // std::cout << "Message: ";
        // std::string message;
        // std::getline(std::cin, message);
        // std::cout << "Send result: " << socketAPI::sendMessage(server.client, message) << '\n';

        // std::cout << "File path: ";
        // std::string filepath;
        // std::cin >> filepath;
        // std::cout << "sendFile(): " << socketAPI::sendFile(server.client, filepath) << '\n';

        // std::cout << "disconnect(): " << server.disconnect() << '\n';
        // std::cout << "serverCleanup(): " << server.serverCleanup() << '\n';
    }
    else if (type == 1) {
        // std::string IP;
        // std::cin >> IP;
        clientSocket client;
        std::string host_ip, netmask;
        char broadcast_address[INET_ADDRSTRLEN];
        std::cin >> host_ip >> netmask;
        std::vector<std::pair<std::string, std::string>> listServer = client.broadcast(host_ip, netmask);
        // std::cout << client.initializeClient(IP.c_str()) << '\n';

        // std::string message;
        // std::cout << "Receive result: " << socketAPI::receiveMessage(client.ConnectSocket, message) << '\n';
        // std::cout << "Message: " << message << '\n';

        // // Sleep(5000);
        // // std::cout << "File path: ";
        // std::string filepath = ".";
        // // std::cin >> filepath;
        // std::cout << "receiveFile(): " << socketAPI::receiveFile(client.ConnectSocket, filepath) << '\n';

        // std::cout << "disconnect(): " << client.disconnect() << '\n';
        // std::cout << "clientCleanup(): " << client.clientCleanup() << '\n';
    }

    socketAPI::cleanup();
    return 0;
}