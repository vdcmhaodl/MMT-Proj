#include "console_bg.h"

std::string clientIPv4 = std::string("127.0.0.1");
std::string subnetMask = std::string("255.255.255.0");
std::string username = trojan;
std::string password;
GmailAccount clientMail;
concurrent_queue<std::string> queueQuery;
ListIPData ListIP;
P2P_clientSocket broadcast;

std::vector<std::string> getInputInfo() {
    std::string IPaddr, subnetMask, mail, password, selectedOption;

    std::cout << "IPv4 address: "   ; std::getline(std::cin, IPaddr);
    std::cout << "Subnet mask: "    ; std::getline(std::cin, subnetMask);
    std::cout << "Mail account: "   ; std::getline(std::cin, mail);
    std::cout << "Password: "       ; std::getline(std::cin, password);
    std::cout << "Mode: "           ; std::getline(std::cin, selectedOption);

    return (!IPaddr.empty() && !subnetMask.empty() && !selectedOption.empty()
        && ((selectedOption == "Automatic" && !mail.empty()) || (selectedOption == "Manual" && mail.empty())) ? 
            std::vector<std::string>{IPaddr, subnetMask, mail, password, selectedOption} : std::vector<std::string>());
}

void initializeBroadcast(std::vector<std::string> &signinInput) {
    clientIPv4 = signinInput[0];
    subnetMask = signinInput[1];
    broadcast.initialize((char*)clientIPv4.c_str(), (char*)subnetMask.c_str());
}

void initializeClient(std::vector<std::string> &signinInput) {
    
}

void initializeMail(std::vector<std::string> &signinInput) {
    username = signinInput[2];
    password = signinInput[3];
    clientMail.initializeInfo(username, password);
}

void initializeUI(std::vector<std::string> &signinInput) {
    
}

std::atomic<bool> isRunning(true);

int startBroadcast() {
    broadcast.start();

    while(isRunning) {
        IPStatusMsg msg;
        if (broadcast.updateStatusIP.try_pop(msg)) {
            auto &[IP, hostname, status] = msg;
            socketAPI::update_list(ListIP, IP, hostname, status);
        }
    }

    broadcast.forceClose();
}

int startClient() {
    while (isRunning) {
        
    }
}

int startMail() {

}

int startUI() {
    
    
    return 0;
}