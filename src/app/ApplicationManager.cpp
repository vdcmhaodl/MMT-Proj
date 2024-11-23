#include "ApplicationManager.h"

bool StartApp(const std::string &appName) {
    // get appID
    std::string cmd = "powershell -command \"Get-StartApps | Where-Object { $_.Name -like '*" + appName + "*' }\"";
    std::string appID = ReadCMD(cmd.c_str());
    if (appID == "") 
        return false;
    size_t pos1 = 0,
           pos2 = 0;
    
    pos1 = appID.find("AppID");
    for (int i = 0; i < 3; ++i) pos2 = appID.find("\n", pos2) + 1;
    pos1 += pos2 - 1;
    pos2 = appID.find("\n", pos1);
    appID = appID.substr(pos1, pos2 - pos1);
    std::string appPath = "shell:AppsFolder\\" + appID;

    // open app
    return system(("powershell -command \"Start-Process " + appPath + "\"").c_str()) == 0;
}

bool StopApp(const std::string &appName) {
    // get ProcessName
    std::string cmd = "powershell -command \"get-process | where-object Name -like '*" 
                      + appName 
                      + "*' | select-object ProcessName\"";
    std::string processName = ReadCMD(cmd.c_str());
    if (processName == "")
        return false;
    size_t pos1 = 0,
           pos2 = 0;
    pos1 = processName.find("ProcessName", pos1);
    pos1 = processName.find("\n", pos1) + 1;
    pos1 = processName.find("\n", pos1) + 1;
    pos2 = processName.find("\n", pos1);
    processName = processName.substr(pos1, pos2 - pos1);

    // stop application
    cmd = "powershell -command \" Stop-Process -Name " + processName + "-Force\"";
    return system(cmd.c_str()) == 0;
}

bool ListApps(const std::string &saveFile) {
    std::set<RunningApp> runningApps;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&runningApps));

    std::ofstream fout(saveFile.c_str());
    if (!fout.is_open()) {
        return false;
    }

    fout << "Cac ung dung dang chay:\n\n";
    for (const auto &app : runningApps) {
        fout << "Process ID: " << app.processID
             << "\nTieu de cua so: " << app.windowTitle
             << "\nTen executable: " << extractAppName(app.executableName)
             << "\nPath: " << app.executableName << "\n\n";
    }

    fout.close();
    return true;
}


std::string ReadCMD(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr)
        result += buffer.data();
    return result;
}

bool isSystemApp(const std::string &windowTitle, const std::string &executableName) {
    std::vector<std::string> excludedTitles = {"Settings", "Program Manager", "Windows Input Experience"};
    std::vector<std::string> excludedExecutables = {"SystemSettings.exe", "explorer.exe", "TextInputHost.exe", "ApplicationFrameHost.exe"};
    for (const std::string &title : excludedTitles) {
        if (windowTitle.find(title) != std::string::npos)
            return true;
    }
    for (const std::string &exe : excludedExecutables) {
        if (executableName.find(exe) != std::string::npos)
            return true;
    }

    return false;
}

std::string extractAppName(const std::string &path) {
    return std::filesystem::path(path).filename().string();
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD processID;
    GetWindowThreadProcessId(hwnd, &processID);

    if (IsWindowVisible(hwnd) && GetWindowTextLength(hwnd) > 0)
    {
        RunningApp app;
        app.processID = processID;

        int length = GetWindowTextLength(hwnd) + 1;
        std::vector<char> title(length);
        GetWindowTextA(hwnd, &title[0], length);
        app.windowTitle = &title[0];

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
        if (hProcess)
        {
            std::vector<char> exeName(MAX_PATH);
            if (GetModuleFileNameExA(hProcess, NULL, &exeName[0], MAX_PATH))
            {
                app.executableName = &exeName[0];
            }
            CloseHandle(hProcess);
        }

        if (!isSystemApp(app.windowTitle, app.executableName))
        {
            std::set<RunningApp> *apps = reinterpret_cast<std::set<RunningApp> *>(lParam);
            apps->insert(app);
        }
    }
    return TRUE;
}