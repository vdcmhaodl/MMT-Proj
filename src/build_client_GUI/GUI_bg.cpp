#include "GUI_bg.h"
#include <atomic>

std::vector<std::string> getInputInfo() {
    InputWindow inp_win(640, 360);
    if (!inp_win.Create("Sign in", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX, WS_EX_CLIENTEDGE)) {
        return std::vector<std::string>();
    }

    ShowWindow(inp_win.Window(), SW_SHOWDEFAULT);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // std::cerr << inp_win.IPaddr << '\n';
    // std::cerr << inp_win.subnetMask << '\n';
    // std::cerr << inp_win.mail << '\n';
    // std::cerr << inp_win.password << '\n';
    // std::cerr << inp_win.selectedOption << '\n';

    std::vector<std::string> result{inp_win.IPaddr, inp_win.subnetMask, inp_win.mail, inp_win.password, inp_win.selectedOption};
    return (inp_win.validinput() ? result : std::vector<std::string>());
}

Broadcast::Broadcast() : Participant() {}

Broadcast::Broadcast(Mediator *mediator, std::string name) : Participant(mediator, name) {}
void Broadcast::initialize(std::vector<std::string> &signinInput) {
    isRunning = true;
    // std::string clientIPv4 = signinInput[0];
    // std::string subnetMask = signinInput[1];
    broadcast.initialize((char*)signinInput[0].c_str(), (char*)signinInput[1].c_str());
}

void Broadcast::start() {
    broadcast.start();

    while(isRunning) {
        IPStatusMsg msg;
        if (broadcast.updateStatusIP.try_pop(msg)) {
            std::unique_lock<std::mutex> lock(mtx);
            auto &[IP, hostname, status] = msg;
            std::osyncstream(std::cout) << "Receive new IP update!\n";
            socketAPI::update_list(listIP, IP, hostname, status);
            mediator->Forward(new std::any(listIP), "IP", COMPONENT::UI_COMPONENT);
            mediator->Forward(new std::any(std::string{"RECEIVE NEW IP UDATE"}), "LOG", COMPONENT::UI_COMPONENT);
            // COPYDATASTRUCT cds = wrapperData(socketAPI::createServerMessage(IP, hostname, status), MainWindow::IP_MESSAGE);
            // SendMessageA(win.Window(), WM_COMMAND, MAKEWORD(MainWindow::IP_UPDATE_MESSAGE, 0), 0);
        }
    }

    broadcast.forceClose();
}

void Broadcast::Receive(std::string msg) {
    std::osyncstream(std::cout) << "Broadcast receive " << msg << '\n';
    if (msg == "exit") {
        isRunning = false;
    }
    else if (msg == "request") {
        mediator->Broadcast(this, new std::any(listIP), "IP");
        // mediator->Forward(new std::any(listIP), "IP", COMPONENT::UI_COMPONENT);
    }
    else {
        Participant::Receive(msg);
    }
}

void Broadcast::Receive(std::string msg, std::string sender) {
    std::osyncstream(std::cout) << "Broadcast receive " << msg << " from " << sender;
    if (msg == "exit") {
        isRunning = false;
    }
    else if (msg == "request") {
        mediator->Forward(new std::any(listIP), "IP", sender);
    }
    else {
        Participant::Receive(msg, sender);
    }
}

Gmail::Gmail() {}
Gmail::Gmail(Mediator *mediator, std::string name) : Participant(mediator, name) {}

void Gmail::initialize(std::vector<std::string> &signinInput) {
    isRunning = true;
    std::string username = signinInput[2];
    std::string password = signinInput[3];
    clientMail.initializeInfo(username, password);
}

void Gmail::start() {
    while(isRunning) {
        std::queue<Mail> newMail = clientMail.getEmailQueue();
        std::osyncstream(std::cout) << "NUMBER OF MAIL: " << newMail.size() << '\n';
        if (!newMail.size()) {
            continue;
        }

        std::unique_lock<std::mutex> lock(mtx);
        FullCommand FC;
        while(!newMail.empty()) {
            mediator->Forward(new std::any(std::string{"RECEIVE NEW MAIL"}), "LOG", COMPONENT::UI_COMPONENT);
            Mail tmp = newMail.front();
            std::osyncstream(std::cout) << "[" << tmp.sender.subject << "]" << ' ' << "[" << tmp.content << "]";
            FC.command = constructCommand(newMail.front());
            FC.email = newMail.front().sender;
            std::osyncstream(std::cout) << "[" << "start forwarding" << "]";
            mediator->Forward(new std::any(FC), COMPONENT::CLIENT_COMPONENT);
            std::osyncstream(std::cout) << "[" << "forward complete?" << "]";
            newMail.pop();
        }

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

Command Gmail::constructCommand(Mail &mail) {
    std::stringstream ss;
    Command command;
    std::string tmp, stringName, stringOption;

    ss << mail.sender.subject;
    ss >> command.type >> tmp >> command.action;
    ss.clear();

    std::string cleansedContent;
    for (char ch : mail.content) { 
        if (ch != '\r') { 
            cleansedContent += ch; 
        } 
    }
    ss << cleansedContent;

    std::getline(ss, command.target);
    std::getline(ss, stringName);
    std::getline(ss, stringOption);

    // std::osyncstream(std::cout) << "[" << command.type << "]" << "[" << command.action << "]" << "[" << command.target << "]" << '\n';
    command.parseString(stringName, command.listName);
    command.parseString(stringOption, command.listOption);

    return command;
}

void Gmail::Receive(std::string msg) {
    if (msg == "exit") {
        isRunning = false;
    }
    else {
        Participant::Receive(msg);
    }
}

Client::Client() {}
Client::Client(Mediator *mediator, std::string name) : Participant(mediator, name) {}

void Client::initialize(std::vector<std::string> &signinInput) {
    isRunning = true;
    std::ofstream fout("Info.txt");
    if (!fout.is_open()) {
        return;
    }

    // fout << "IP address: " << clientIPv4 << '\n';
    // fout << "Subnet mask: " << subnetMask << '\n';
    // fout << "Mail account: " << username << '\n';

    fout.close();
}

void Client::start() {
    while(isRunning) {
        FullCommand FC;
        if (!queueCommand.try_pop(FC)) {
            continue;
        }

        std::lock_guard<std::mutex> lock(mtx);

        std::osyncstream(std::cout) << "RECEIVE NEW COMMAND:\n";
        std::osyncstream(std::cout) << "EMAIL:\n";
        std::osyncstream(std::cout) << "ACCOUNT: " << FC.email.account << '\n';
        std::osyncstream(std::cout) << "MESSAGE ID: " << FC.email.messageID << '\n';
        std::osyncstream(std::cout) << "COMMAND: " << FC.command.to_string() << '\n';

        resolveCommand(FC);
    }
}

void Client::resolveCommand(FullCommand FC) {
    // check IP 
    std::osyncstream(std::cout) << "START RESOLVE COMMAND\n";
    IPdata = false;
    mediator->Forward(COMPONENT::CLIENT_COMPONENT, COMPONENT::BROADCAST_COMPONENT, "request");
    while(!IPdata) {}
    if (listIP.find(FC.command.target) == listIP.end()) {
        mediator->Forward(new std::any(std::string{"ERROR: IP NOT FOUND"}), "LOG", COMPONENT::UI_COMPONENT);
        return;
    }

    clientSocket client;
    client.initializeClient((char*)FC.command.target.c_str());
    auto msg = FC.command.to_string();
    socketAPI::sendMessage(client.ConnectSocket, msg);
    socketAPI::receiveMessage(client.ConnectSocket, msg);
    std::osyncstream(std::cout) << "MSG FROM SERVER: " << msg << '\n';
    client.disconnect();
    client.clientCleanup();
}

void Client::Receive(std::string msg) {
    if (msg == "exit") {
        isRunning = false;
    }
    else {
        Participant::Receive(msg);
    }
}

void Client::Receive(std::any *ptr) {
    std::cout << "Receive " << ptr << '\n';
    FullCommand FC = std::any_cast<FullCommand>(*ptr);
    queueCommand.push(FC);
    delete ptr;
}

void Client::Receive(std::any *ptr, std::string type) {
    if (type == "IP") {
        listIP = std::any_cast<ListIPData>(*ptr);
        IPdata = true;
        delete ptr;
    }
}
