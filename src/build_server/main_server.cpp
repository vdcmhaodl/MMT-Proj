#include "server_bg.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // if (argc == 1 || argv[1] != "debug-mode") {
    //     ShowWindow(GetConsoleWindow(), SW_HIDE);
    // }
    std::cerr << "START!!!!\n";
    initializeServer();
    startServer();
    endServer();
    std::cerr << "END!!!!\n";
}