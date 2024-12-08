#include "GUI_bg.h"

int main() {
    socketAPI::initializeSocket();

    std::vector<std::string> signinInput = getInputInfo();

    if (signinInput.empty()) {
        MessageBoxW(NULL, L"Invalid input!", L"SIGN IN", MB_OK);
        return 0;
    }

    // for (auto it: signinInput) {
    //     std::cerr << it << ' ';
    // }

    initializeBroadcast(signinInput);
    initializeClient(signinInput);
    initializeMail(signinInput);
    initializeUI(signinInput);

    std::thread broadcastThread(startBroadcast);
    // std::thread clientThread(startClient);
    // std::thread mailThread(startMail);
    std::thread UIThread(startUI);

    idThreadBroadcast = broadcastThread.get_id();
    // idThreadClient = clientThread.get_id();
    // idThreadMail = mailThread.get_id();
    idThreadUI = UIThread.get_id();

    broadcastThread.join();
    // clientThread.join();
    // mailThread.join();
    UIThread.join();

    socketAPI::cleanup();

    return 0;
}