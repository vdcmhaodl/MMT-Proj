#include "socket.h"
#include "p2p.h"
#include <iostream>

bool isRunning = true;

void countdown() {
    std::this_thread::sleep_for(std::chrono::seconds(30));
    isRunning = false;
}

void countdown_condition(int s) {
    std::this_thread::sleep_for(std::chrono::seconds(s));
}

int main(int argc, char** argv) {
    std::thread T(countdown);

    P2P_Socket S;
    if (argc > 1) {
        S.debugger = std::stoi(argv[1]);
        if (S.debugger) {
            std::cout << "DEBUG TIME!\n";
        }
    }

    int option;
    std::cin >> option;
    if (option == 0) {
        std::cout << "SERVER\n";
        S.initialize();
    }
    else if (option == 1) {
        std::cout << "CLIENT\n";
        std::string IP, subnetMask;
        std::cin >> IP >> subnetMask;
        S.initialize((char*)IP.c_str(), (char*)subnetMask.c_str());
    }

    std::cout << "Hey!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    S.start("Hello");
    std::cout << "Done detachhhhhhhhhhhhhhhhhhhhhhhhhhhhhh\n";

    // int cnt = 0;
    while(isRunning) {
        // std::string inp = std::string(argv[1]) + std::string(" ") + std::to_string(cnt);
        std::string inp;
        std::cin >> inp;
        S.addMessage(inp);
        // std::thread waitforlove(countdown_condition, 1);
        // waitforlove.join();
        // ++cnt;
    }
    S.forceClose();

    T.join();
}