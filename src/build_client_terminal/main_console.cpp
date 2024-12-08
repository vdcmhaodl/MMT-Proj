#include "console_bg.h"

int main() {
    socketAPI::initializeSocket();

    std::vector<std::string> signinInput = getInputInfo();

    if (signinInput.empty()) {
        // std::wstring respond = L"Co vay cung nhap sai";
        // MessageBoxW(NULL, respond.c_str(), L"Ga", MB_OK);
        return 0;
    }

    initializeBroadcast(signinInput);
    initializeClient(signinInput);
    initializeUI(signinInput);

    std::thread broadcastThread(startBroadcast);
    std::thread clientThread(startClient);
    std::thread UIThread(startUI);

    broadcastThread.join();
    clientThread.join();
    UIThread.join();

    socketAPI::cleanup();
}