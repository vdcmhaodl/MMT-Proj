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

struct RunningApp {
    DWORD processID;
    std::string windowTitle;
    std::string executableName;

    bool operator<(const RunningApp &other) const {
        return tie(processID, windowTitle) < tie(other.processID, other.windowTitle);
    }
};

bool StartApp(const std::string &appName);
bool StopApp(const std::string &appName);
bool ListApps(const std::string &saveFile);

std::string ReadCMD(const char* cmd);
bool isSystemApp(const std::string &windowTitle, const std::string &executableName);
std::string extractAppName(const std::string &path);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);