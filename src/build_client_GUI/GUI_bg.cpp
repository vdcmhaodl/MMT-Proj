#include "GUI_bg.h"
#include <atomic>

std::string clientIPv4;
std::string subnetMask;
P2P_clientSocket broadcast;

std::string username;
std::string password;
GmailAccount clientMail;
concurrent_queue<Email> queueMail;

concurrent_queue<std::string> queueQuery;
concurrent_queue<std::string> queueLog;

std::atomic<bool> isRunning(true);

MainWindow win(1280, 720);

std::vector<std::string> getInputInfo() {
    InputWindow inp_win(1280, 720);
    if (!inp_win.Create("Sign in", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX, WS_EX_CLIENTEDGE)) {
        return std::vector<std::string>();
    }

    ShowWindow(inp_win.Window(), SW_SHOWDEFAULT);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    std::cerr << inp_win.IPaddr << '\n';
    std::cerr << inp_win.subnetMask << '\n';
    std::cerr << inp_win.mail << '\n';
    std::cerr << inp_win.password << '\n';
    std::cerr << inp_win.selectedOption << '\n';

    std::vector<std::string> result{inp_win.IPaddr, inp_win.subnetMask, inp_win.mail, inp_win.password, inp_win.selectedOption};
    return (inp_win.validinput() ? result : std::vector<std::string>());
}

void initializeBroadcast(std::vector<std::string> &signinInput) {
    clientIPv4 = signinInput[0];
    subnetMask = signinInput[1];
    broadcast.initialize((char*)clientIPv4.c_str(), (char*)subnetMask.c_str());
}

void initializeClient(std::vector<std::string> &signinInput) {
    std::ofstream fout("Info.txt");
    if (!fout.is_open()) {
        return;
    }

    fout << "IP address: " << clientIPv4 << '\n';
    fout << "Subnet mask: " << subnetMask << '\n';
    fout << "Mail account: " << username << '\n';

    fout.close();
}

void initializeMail(std::vector<std::string> &signinInput) {
    username = signinInput[2];
    password = signinInput[3];
    clientMail.initializeInfo(username, password);
}

void initializeUI(std::vector<std::string> &signinInput) {
    win.setMode(signinInput[4] == "AUTOMATIC" ? AUTOMATIC : MANUAL);
    win.INFO.setFilePath("INFO.txt");
    win.HELP.setFilePath("HELP.txt");
}

std::thread::id idThreadBroadcast;
std::thread::id idThreadClient;
std::thread::id idThreadMail;
std::thread::id idThreadUI;

int startBroadcast() {
    broadcast.start();

    while(isRunning) {
        IPStatusMsg msg;
        if (broadcast.updateStatusIP.try_pop(msg)) {
            auto &[IP, hostname, status] = msg;
            std::osyncstream(std::cout) << "Receive new IP update!\n";
            socketAPI::update_list(list_IP_addr, IP, hostname, status);
            // COPYDATASTRUCT cds = wrapperData(socketAPI::createServerMessage(IP, hostname, status), MainWindow::IP_MESSAGE);
            SendMessageA(win.Window(), WM_COMMAND, MAKEWORD(MainWindow::IP_UPDATE_MESSAGE, 0), 0);
        }
    }

    broadcast.forceClose();
}

int startClient() {
    while (isRunning) {
        
    }
}

int startMail() {
    while(isRunning) {
        std::queue<Email> newEmail = clientMail.getEmailQueue();
        while(!newEmail.empty()) {
            queueMail.push(newEmail.front());
            newEmail.pop();
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

int startUI() {
    if (!win.Create("Learn to Program Windows", 
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX, WS_EX_CLIENTEDGE)) {
        return 0;
    }

    win.INFO.update_hwnd_parent(win.Window());
    win.HELP.update_hwnd_parent(win.Window());
    win.IP.update_hwnd_parent(win.Window());
    win.LOG.update_hwnd_parent(win.Window());
    win.EXTRA.update_hwnd_parent(win.Window());

    ShowWindow(win.Window(), SW_SHOWDEFAULT);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    isRunning = false;
    
    return 0;
}