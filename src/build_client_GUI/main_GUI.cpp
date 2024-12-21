#include "GUI_bg.h"

int main() {
    socketAPI::initializeSocket();

    std::vector<std::string> signinInput = getInputInfo();

    if (signinInput.empty()) {
        MessageBoxW(NULL, L"Invalid input!", L"SIGN IN", MB_OK);
        return 0;
    }

    // std::vector<std::string> signinInput;
    // signinInput = std::vector<std::string> {"192.168.2.19", "255.255.255.0", "mmt23clc007017236@gmail.com", "fvlo wnaj azrh tzuq", "Automatic"};
    
    // for (auto s : signinInput)
    //     std::cout << s << '\n';

    // std::cout << "We good\n";

    Mediator mediator;

    UI ui(954, 480, 0, &mediator, COMPONENT::UI_COMPONENT);
    Gmail gmail(&mediator, COMPONENT::MAIL_COMPONENT);
    Client client(&mediator, COMPONENT::CLIENT_COMPONENT);
    Broadcast broadcast(&mediator, COMPONENT::BROADCAST_COMPONENT);

    mediator.Register(&ui);
    mediator.Register(&gmail);
    mediator.Register(&client);
    mediator.Register(&broadcast);

    // std::cout << signinInput[0] << ' ' << signinInput[1] << '\n';

    ui.initialize(signinInput);
    gmail.initialize(signinInput);
    client.initialize(signinInput);
    broadcast.initialize(signinInput);

    // std::cout << "check again " << broadcast.broadcast.IP_addr << ' ' << broadcast.broadcast.subnetMask << '\n';

    std::thread UIThread(&UI::start, &ui);
    std::thread mailThread(&Gmail::start, &gmail);
    std::thread clientThread(&Client::start, &client);
    std::thread broadcastThread(&Broadcast::start, &broadcast);

    broadcastThread.join();
    clientThread.join();
    mailThread.join();
    UIThread.join();

    socketAPI::cleanup();
    return 0;
}