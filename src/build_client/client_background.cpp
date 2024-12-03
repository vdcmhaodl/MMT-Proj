#include "client_background.h"
#include <atomic>

std::string clientIPv4 = std::string("127.0.0.1");
std::string subnetMask = std::string("255.255.255.0");
std::string username = trojan;
std::string password;
Gmail clientMail;
std::queue<std::string> queueQuery;

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

    std::vector<std::string> result{inp_win.IPaddr, inp_win.subnetMask, inp_win.mail, inp_win.password, inp_win.selectedOption};
    return (inp_win.validinput() ? result : std::vector<std::string>());
}

void initializeBroadcast(std::vector<std::string> &signinInput) {
    clientIPv4 = signinInput[0];
    subnetMask = signinInput[1];
}

void initializeClient(std::vector<std::string> &signinInput) {
    username = signinInput[2];
    password = signinInput[3];
    clientMail = Gmail(username, password);
}

void initializeUI(std::vector<std::string> &signinInput) {
    win.setMode(signinInput[4] == "AUTOMATIC" ? AUTOMATIC : MANUAL);
}

int startBroadcast() {
    broadcast.start();

    while(isRunning) {
        
    }
}

int startClient() {
    while (isRunning) {
        
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