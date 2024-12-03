#include "server_background.h"

int main() {
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    startServer();
    endServer();
}