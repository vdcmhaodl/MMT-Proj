#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <windows.h> 
#include <filesystem>
#include <psapi.h>
#include <set>
#include <string>
#include <array>
#include <memory>
#include <vector>
#include "serviceCommand.h"

struct RunningApp {
    DWORD processID;
    std::string windowTitle;
    std::string executableName;

    bool operator<(const RunningApp &other) const {
        return tie(processID, windowTitle) < tie(other.processID, other.windowTitle);
    }
};

namespace Services {
    bool startApplication(const std::string &appName);
    bool stopApplication(const std::string &appName);
    bool listApplications(const std::string &saveFile);
    
    std::vector<std::string> startApplication(Command command);
    std::vector<std::string> stopApplication(Command command);
    std::vector<std::string> listApplications(Command command);
}

std::string ReadCMD(const char* cmd);

bool isSystemApp(const std::string &windowTitle, const std::string &executableName);
std::string extractAppName(const std::string &path);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);