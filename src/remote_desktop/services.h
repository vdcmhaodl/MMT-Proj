#pragma once

// #define _WIN32_WINNT 0x0A00
#include <Windows.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <shellscalingapi.h>
#include <tlhelp32.h>
#include <tchar.h>
// #include <TlHelp32.h>
#include <psapi.h>
#include <set>
#include <map>
#include <filesystem>
#include <mutex>
#include <array>
#include <memory>
#include <stdexcept>

struct RunningApp {
    DWORD processID;
    std::string windowTitle;
    std::string executableName;

    bool operator<(const RunningApp &other) const {
        return tie(processID, windowTitle) < tie(other.processID, other.windowTitle);
    }
};

namespace Services {
    std::string extractAppName(const std::string &path);
    BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
    bool getApplicationPathFromSearch(const std::string &appName, std::string &appPath);
    bool isSystemApp(const std::string &windowTitle, const std::string &executableName);

    bool shutdownComputer(const std::string &saveFile);
    bool restartComputer(const std::string &saveFile);
    
    std::string exec(const char* cmd);
    bool listApplications(const std::string &saveFile);
    bool startApplication(const std::string &appName);
    bool isApplicationRunning(const std::string &appName);
    bool stopApplication(const std::string &appPath);

    bool listServices(const std::string &saveFile);
    bool startService(const std::string &serviceName);
    bool stopService(const std::string &serviceName);

    bool CaptureScreen(const std::string &filename);
    bool takeScreenShot(const std::string &filename);

    // bool copyFile(const std::string &fileSrc, const std::string &fileDes);
    bool deleteFile(const std::string &fileName);

    bool SpecialKeys(int S_Key);
    LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    bool keyLogger(const std::string &saveFile);

    bool startWebcam(const std::string &saveFile);
    bool stopWebcam(const std::string &saveFile);

    extern std::map<std::string, bool(*)(const std::string&)> servicesMap;

    bool processCommand(std::string &command);
}