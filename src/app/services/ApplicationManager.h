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
#include "SystemManager.h"

struct RunningApp {
    DWORD processID;
    std::wstring windowTitle;
    std::wstring executableName;

    bool operator<(const RunningApp &other) const {
        return std::tie(processID, windowTitle) < std::tie(other.processID, other.windowTitle);
    }
};

namespace Services {
    bool startApplication(const std::string &appName);
    bool stopApplication(const std::string &appName);
    bool listApplications(const std::wstring &filename);
    
    std::vector<std::string> startApplication(Command command);
    std::vector<std::string> stopApplication(Command command);
    std::vector<std::string> listApplications(Command command);
}

std::string ReadCMD(const char* cmd);

bool isSystemApp(const std::wstring &windowTitle, const std::wstring &executableName);
std::wstring extractAppName(const std::wstring &path);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);