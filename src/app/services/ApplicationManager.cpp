#include "ApplicationManager.h"


std::vector<std::string> Services::startApplication(Command command) {
    std::string filename;

    for (auto it : command.listName) {
        if (!Services::startApplication(it))
            filename += "Cannot start " + it + "\r\n";
        else
            filename += "Start app " + it + " successfully\r\n";
    }

    return std::vector<std::string> ({filename});
}

std::vector<std::string> Services::stopApplication(Command command) {
    std::string filename;

    for (auto it : command.listName) {
        if (!Services::stopApplication(it)) 
            filename += "Cannot stop " + it + "\n";
        else
            filename += "Stop app " + it + " successfully\n";
    }
    return std::vector<std::string> ({filename});
}

std::vector<std::string> Services::listApplications(Command command) {
    std::string filename = Command::generateFilepath(10, ".txt");

    std::wstring wFileName (filename.begin(), filename.end());
    if (!Services::listApplications(wFileName)) {
        return std::vector<std::string> ({"Failed to list applications"});
    }
    std::cerr << "listApp successfully\n";
    
    return std::vector<std::string> ({filename});
}

bool Services::startApplication(const std::string &appName) {
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
    std::string appPath = "'shell:AppsFolder\\" + appID + "'";

    // open app
    return system(("powershell -command \"Start-Process " + appPath + "\"").c_str()) == 0;
}

bool Services::stopApplication(const std::string &appName) {
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

bool Services::listApplications(const std::wstring &filename) {
    std::set<RunningApp> runningApps;
    
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&runningApps));
    std::wofstream fout(filename.c_str());
    fout.imbue(std::locale(fout.getloc(), new std::codecvt_utf8<wchar_t>));

    for (const auto &app : runningApps) {
        fout << L"Process ID: " << app.processID << L"\n";
        fout << L"Window title: " << app.windowTitle << L"\n";
        fout << L"Executable name: " << extractAppName(app.executableName) << L"\n";
        fout << L"Path: " << app.executableName << L"\n\n";
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

bool isSystemApp(const std::wstring &windowTitle, const std::wstring &executableName) {
    std::vector<std::wstring> excludedTitles = {L"Settings", L"Program Manager", L"Windows Input Experience"};
    std::vector<std::wstring> excludedExecutables = {L"SystemSettings.exe", L"explorer.exe", L"TextInputHost.exe", L"ApplicationFrameHost.exe"};
    for (const std::wstring &title : excludedTitles) {
        if (windowTitle.find(title) != std::wstring::npos)
            return true;
    }
    for (const std::wstring &exe : excludedExecutables) {
        if (executableName.find(exe) != std::wstring::npos)
            return true;
    }
    return false;
}

std::wstring extractAppName(const std::wstring &path) {
    return std::filesystem::path(path).filename().wstring();
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD processID;
    GetWindowThreadProcessId(hwnd, &processID);

    if (IsWindowVisible(hwnd) && GetWindowTextLengthW(hwnd) > 0)
    {
        RunningApp app;
        app.processID = processID;

        int length = GetWindowTextLengthW(hwnd) + 1;
        std::vector<wchar_t> title(length);
        GetWindowTextW(hwnd, &title[0], length);

        app.windowTitle.assign(title.begin(), title.end());

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
        if (hProcess)
        {
            std::vector<wchar_t> exeName(MAX_PATH);
            if (GetModuleFileNameExW(hProcess, NULL, &exeName[0], MAX_PATH))
            {
                app.executableName.assign(exeName.begin(), exeName.end());
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