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

    // std::ifstream fin("Default_account.txt");
    // if (fin.is_open()) {
    //     std::string default_username;
    //     std::string default_password;
    //     std::getline(fin, default_username);
    //     std::getline(fin, default_password);

    //     inp_win.mail = default_username;
    //     inp_win.password = default_password;
    //     inp_win.selectedOption = "Automatic";

    //     fin.close();
    // }

    std::vector<std::string> result{inp_win.IPaddr, inp_win.subnetMask, inp_win.mail, inp_win.password};
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
            // std::osyncstream(std::cout) << "Receive new IP update!\n";
            socketAPI::update_list(listIP, IP, hostname, status);
            mediator->Forward(new std::any(listIP), "IP", COMPONENT::UI_COMPONENT);

            std::string newMsg = Services::getCurrentTimeString() + "|" + "IP update: " + IP + " - " + hostname + " - " + status + "\n";
            mediator->Forward(new std::any(newMsg), "LOG", COMPONENT::UI_COMPONENT);
            // COPYDATASTRUCT cds = wrapperData(socketAPI::createServerMessage(IP, hostname, status), MainWindow::IP_MESSAGE);
            // SendMessageA(win.Window(), WM_COMMAND, MAKEWORD(MainWindow::IP_UPDATE_MESSAGE, 0), 0);
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    broadcast.forceClose();
}

void Broadcast::Receive(std::string msg) {
    // std::osyncstream(std::cout) << "Broadcast receive " << msg << '\n';
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
    // std::osyncstream(std::cout) << "Broadcast receive " << msg << " from " << sender;
    if (msg == "exit") {
        isRunning = false;
    }
    else if (msg == "request") {
        // std::cout << "\nABC\n";
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
        mtx.lock();
        // std::osyncstream(std::cout) << "Start checking mail" << '\n';
        std::queue<Mail> newMail = clientMail.getEmailQueue();
        mtx.unlock();

        // std::osyncstream(std::cout) << "NUMBER OF MAIL: " << newMail.size() << '\n';
        if (!newMail.size()) {
            std::this_thread::sleep_for(std::chrono::seconds(20));
            continue;
        }
        
        // std::unique_lock<std::mutex> lock(mtx);
        FullCommand FC;
        while(!newMail.empty()) {
            Mail tmp = newMail.front();
            // std::osyncstream(std::cout) << "[" << tmp.sender.subject << "]" << ' ' << "[" << tmp.content << "]";
            FC.command = constructCommand(newMail.front());
            FC.email = newMail.front().sender;

            std::string newMsg = Services::getCurrentTimeString() + "|" + "New mail: " + FC.command.to_string() + "\n";
            mediator->Forward(new std::any(newMsg), "LOG", COMPONENT::UI_COMPONENT);

            // std::osyncstream(std::cout) << "[" << "start forwarding" << "]";
            mediator->Forward(new std::any(FC), COMPONENT::CLIENT_COMPONENT);
            // std::osyncstream(std::cout) << "[" << "forward complete?" << "]";
            newMail.pop();
        }
        std::this_thread::sleep_for(std::chrono::seconds(20));
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

    // std::cout << cleansedContent << "\n" << command.target
    //             << "\n" << stringName << "\n" << stringOption << "\n";

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

void Gmail::Receive(std::any *ptr) {
    // std::cout << "Receive " << ptr << '\n';
    std::pair<Email, std::vector<std::string>> FC = std::any_cast<std::pair<Email, std::vector<std::string>>>(*ptr);

    
    for (std::string filename: FC.second) {
        mtx.lock();
        clientMail.repEmail(FC.first, "", filename);
        mtx.unlock();
    }
    
    delete ptr;
}

void Gmail::Receive(std::any *ptr, std::string type) {
    // std::osyncstream(std::cout) << "Receive " << ptr << "; type " << type << '\n';
    std::pair<Email, std::vector<std::string>> FC = std::any_cast<std::pair<Email, std::vector<std::string>>>(*ptr);
    
    if (type == "content") {
        for (std::string content: FC.second) {
            // std::osyncstream(std::cout) << "Type " << type << "  content: " << content << '\n';
            mtx.lock();
            clientMail.repEmail(FC.first, content, "");
            mtx.unlock();
        }
    }
    else if (type == "file") {
        for (std::string filename: FC.second) {
            // std::osyncstream(std::cout) << "Type " << type << "  filename: " << filename << '\n';
            mtx.lock();
            clientMail.repEmail(FC.first, "", filename);
            mtx.unlock();
        }
    }
    // std::string newMsg = Services::getCurrentTimeString() + "|" + "Request from " + FC.first.account + " success!";
    // mediator->Forward(new std::any(newMsg), "LOG", COMPONENT::UI_COMPONENT);
    
    delete ptr;
}

Client::Client() {}
Client::Client(Mediator *mediator, std::string name) : Participant(mediator, name) {}

void Client::initialize(std::vector<std::string> &signinInput) {
    isRunning = true;
    std::ofstream fout("Info.txt");
    if (!fout.is_open()) {
        return;
    }

    fout << "IP address     : " << signinInput[0] << '\n';
    fout << "Subnet mask    : " << signinInput[1] << '\n';
    fout << "Mail account   : " << signinInput[2] << '\n';

    fout.close();
}

void Client::start() {
    while(isRunning) {
        FullCommand FC;
        if (!queueCommand.try_pop(FC)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        std::lock_guard<std::mutex> lock(mtx);

        std::string newMsg = Services::getCurrentTimeString() + "|" + "Start resolve command: " + FC.command.to_string();
        mediator->Forward(new std::any(newMsg), "LOG", COMPONENT::UI_COMPONENT);

        // std::osyncstream(std::cout) << "RECEIVE NEW COMMAND:\n";
        // std::osyncstream(std::cout) << "EMAIL:\n";
        // std::osyncstream(std::cout) << "ACCOUNT: " << FC.email.account << '\n';
        // std::osyncstream(std::cout) << "MESSAGE ID: " << FC.email.messageID << '\n';
        // std::osyncstream(std::cout) << "COMMAND: " << FC.command.to_string() << '\n';

        resolveCommand(FC);
    }
}

void Client::resolveCommand(FullCommand FC) {
    // check IP 
    // std::osyncstream(std::cout) << "START RESOLVE COMMAND\n";
    IPdata = false;
    mediator->Forward(COMPONENT::CLIENT_COMPONENT, COMPONENT::BROADCAST_COMPONENT, "request");
    while(!IPdata) {}
    if (FC.command.type == "IP") {
        std::string content = "SERVER IP:\n";
        for (auto &[IP, info] : listIP) {
            auto &[hostname, status] = info;
            content += '{' + IP + '|' + hostname + '|' + status + '}' + '\n';
        }
        mediator->Forward(new std::any(std::pair<Email, std::vector<std::string>>(FC.email, std::vector<std::string>{content})), "content", COMPONENT::MAIL_COMPONENT);
        return;
    }
    else if (FC.command.type == "help") {
        std::string result = "FAIL to get HELP!";
        std::ifstream fin("Help.txt");
        if (fin.is_open()) {
            result = "Help.txt";
            fin.close();
            mediator->Forward(new std::any(std::pair<Email, std::vector<std::string>>(FC.email, std::vector<std::string>{result})), "file", COMPONENT::MAIL_COMPONENT);
        } 
        else {
            mediator->Forward(new std::any(std::pair<Email, std::vector<std::string>>(FC.email, std::vector<std::string>{result})), "content", COMPONENT::MAIL_COMPONENT);
        }
        return;
    }

    if (listIP.find(FC.command.target) == listIP.end()) {
        std::string newMsg = Services::getCurrentTimeString() + "|" + "Cannot find the target IP " + FC.command.to_string();
        mediator->Forward(new std::any(newMsg), "LOG", COMPONENT::UI_COMPONENT);
        return;
    }

    clientSocket client;
    client.initializeClient((char*)FC.command.target.c_str());
    auto msg = FC.command.to_string();
    socketAPI::sendMessage(client.ConnectSocket, msg);
    // TODO: Receive result sended back from the server side

    std::string message[2] = {"file", "content"};
    
    std::string stringNum;
    socketAPI::receiveMessage(client.ConnectSocket, stringNum);
    int numFile = std::stoi(stringNum);
    std::vector<std::string> listReceiveFile;
    std::vector<std::string> listReceiveMsg;

    for (int i = 0; i < numFile; i++) {
        std::string type;
        socketAPI::receiveMessage(client.ConnectSocket, type);
        if (type == "content") {
            std::string content;
            socketAPI::receiveMessage(client.ConnectSocket, content);
            listReceiveMsg.push_back(content);
        }
        else {
            std::string filename;
            socketAPI::receiveFile(client.ConnectSocket, filename);
            listReceiveFile.push_back(filename);
        }
    }

    mediator->Forward(new std::any(std::pair<Email, std::vector<std::string>>(FC.email, listReceiveMsg)), "content", COMPONENT::MAIL_COMPONENT);
    mediator->Forward(new std::any(std::pair<Email, std::vector<std::string>>(FC.email, listReceiveFile)), "file", COMPONENT::MAIL_COMPONENT);

    // notice successfully
    std::string newMsg = Services::getCurrentTimeString() + "|" + "Request " + FC.command.type + " - " + FC.command.action
                        + " from " + FC.email.account + " successfully!\n";
    mediator->Forward(new std::any(newMsg), "LOG", COMPONENT::UI_COMPONENT);

    //delete file
    for (auto &it : listReceiveFile) {
        Services::deleteFile(it);
    }        

    // Expect receiving a file
    
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
    // std::cout << "Receive " << ptr << '\n';
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
