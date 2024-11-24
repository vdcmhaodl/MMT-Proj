#include "SystemManager.h"

bool Services::shutdown(const std::string &saveFile) {
    return system("shutdown /s /f /t 10") == 0;
}

bool Services::restart(const std::string &saveFile) {
    return system("shutdown /r /f /t 10") == 0;
}

bool Services::listFileAndFolder(const std::string &directory) {
    std::string saveFile = directory + "\\ListFileAndFolder.txt";

    std::ofstream ofs (saveFile);
    if (!ofs.is_open()) {
        std::cout << "Fail to open file!\n";
        return false;
    }
    
    WIN32_FIND_DATAA findFileData;
    HANDLE handle = FindFirstFileA((directory + "\\*").c_str(), &findFileData);
    if (handle == INVALID_HANDLE_VALUE) {
        std::cerr << "Error to access directory!\n";
        return false;
    }

    do {
        DWORD fileType = findFileData.dwFileAttributes;
        std::string fileName = findFileData.cFileName;
        
        if (fileName == "." || fileName == "..")
            continue;

        ofs << std::setw(8);
        if (fileType & FILE_ATTRIBUTE_DIRECTORY)
            ofs << "[FOLDER]";
        else 
            ofs << "[FILE]";
        ofs << std::setw(30) << (std::string)fileName 
                  << std::setw(10) << findFileData.nFileSizeLow << " Byte\n";
    } 
    while (FindNextFileA(handle, &findFileData) != 0);

    FindClose(handle);
    ofs.close();

    return true;
}

bool Services::deleteFile(const std::string &filePath)
{
    return system(("Del \"" + filePath + "\"").c_str()) == 0;
}

// bool Services::listServices(const std::string &saveFile) {
//     std::ofstream fout(saveFile.c_str());
//     if (!fout.is_open()) {
//         return false;
//     }

//     SC_HANDLE scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
//     if (scmHandle == NULL) {
//         std::cerr << "Error opening Service Control Manager: " << GetLastError() << std::endl;
//         return false;
//     }

//     DWORD bytesNeeded = 0;
//     DWORD servicesCount = 0;
//     DWORD resumeHandle = 0;

//     // Lần đầu gọi EnumServicesStatusEx để lấy kích thước bộ nhớ cần thiết
//     if (!EnumServicesStatusEx(scmHandle, SC_ENUM_PROCESS_INFO, SERVICE_TYPE_ALL, SERVICE_ACTIVE, NULL, 0, &bytesNeeded, &servicesCount, &resumeHandle, NULL)) {
//         DWORD error = GetLastError();
//         if (error != ERROR_MORE_DATA) {
//             std::cerr << "Error calling EnumServicesStatusEx for size calculation: " << error << std::endl;
//             CloseServiceHandle(scmHandle);
//             return false;
//         }
//     }

//     // Cấp phát bộ nhớ
//     LPBYTE buffer = (LPBYTE)LocalAlloc(LMEM_ZEROINIT, bytesNeeded);
//     if (buffer == NULL) {
//         std::cerr << "Error allocating memory" << std::endl;
//         CloseServiceHandle(scmHandle);
//         return false;
//     }

//     // Gọi lại EnumServicesStatusEx để lấy danh sách dịch vụ
//     if (!EnumServicesStatusEx(scmHandle, SC_ENUM_PROCESS_INFO, SERVICE_TYPE_ALL, SERVICE_ACTIVE, buffer, bytesNeeded, &bytesNeeded, &servicesCount, &resumeHandle, NULL)) {
//         std::cerr << "Error enumerating services: " << GetLastError() << std::endl;
//         LocalFree(buffer);
//         CloseServiceHandle(scmHandle);
//         return false;
//     }

//     // Chuyển đổi buffer sang ENUM_SERVICE_STATUS_PROCESS
//     LPENUM_SERVICE_STATUS_PROCESS services = (LPENUM_SERVICE_STATUS_PROCESS)buffer;

//     // In danh sách các dịch vụ ra màn hình
//     fout << std::setw(30) << std::left << "Service name" << "Process ID" << "\n\n";
//     for (DWORD i = 0; i < servicesCount; i++) {
//         std::string serviceName = services[i].lpServiceName;
//         DWORD processId = services[i].ServiceStatusProcess.dwProcessId;

//         fout << std::setw(30) << std::left << serviceName << processId << std::endl;
//     }

//     // Giải phóng bộ nhớ và đóng handle của SCM
//     LocalFree(buffer);
//     CloseServiceHandle(scmHandle);
//     fout.close();

//     return true;
// }

bool Services::startService(const std::string &serviceName) {
    SC_HANDLE scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (scmHandle == NULL) {
        std::cerr << "Error opening Service Control Manager: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE serviceHandle = OpenServiceA(scmHandle, serviceName.c_str(), SERVICE_START | SERVICE_QUERY_STATUS);
    if (serviceHandle == NULL) {
        DWORD dwError = GetLastError();
        std::cerr << "Error opening service: " << dwError << std::endl;
        CloseServiceHandle(scmHandle);
        return false;
    }

    if (!StartService(serviceHandle, 0, NULL)) {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_SERVICE_ALREADY_RUNNING) {
            std::cout << "Service " << serviceName << " is already running." << std::endl;
        } else {
            std::cerr << "Error starting service: " << dwError << std::endl;
        }
        CloseServiceHandle(serviceHandle);
        CloseServiceHandle(scmHandle);
        return false;
    }

    // Kiểm tra trạng thái dịch vụ
    SERVICE_STATUS_PROCESS ssp;
    DWORD bytesNeeded;
    do {
        if (!QueryServiceStatusEx(serviceHandle, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded)) {
            std::cerr << "Error querying service status: " << GetLastError() << std::endl;
            CloseServiceHandle(serviceHandle);
            CloseServiceHandle(scmHandle);
            return false;
        }
    } while (ssp.dwCurrentState == SERVICE_START_PENDING);

    if (ssp.dwCurrentState == SERVICE_RUNNING) {
        std::cout << "Service " << serviceName << " started successfully!" << std::endl;
    } else {
        std::cerr << "Service failed to start." << std::endl;
    }

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

    SC_HANDLE serviceHandle = OpenServiceA(scmHandle, serviceName.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (serviceHandle == NULL) {
        DWORD dwError = GetLastError();
        std::cerr << "Error opening service: " << dwError << std::endl;
        CloseServiceHandle(scmHandle);
        return false;
    }

    SERVICE_STATUS status;
    if (!ControlService(serviceHandle, SERVICE_CONTROL_STOP, &status)) {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_SERVICE_NOT_ACTIVE) {
            std::cout << "Service " << serviceName << " is not running." << std::endl;
        } else {
            std::cerr << "Error stopping service: " << dwError << std::endl;
        }
        CloseServiceHandle(serviceHandle);
        CloseServiceHandle(scmHandle);
        return false;
    }

    // Kiểm tra trạng thái dịch vụ
    SERVICE_STATUS_PROCESS ssp;
    DWORD bytesNeeded;
    do {
        if (!QueryServiceStatusEx(serviceHandle, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded)) {
            std::cerr << "Error querying service status: " << GetLastError() << std::endl;
            CloseServiceHandle(serviceHandle);
            CloseServiceHandle(scmHandle);
            return false;
        }
    } while (ssp.dwCurrentState == SERVICE_STOP_PENDING);

    if (ssp.dwCurrentState == SERVICE_STOPPED) {
        std::cout << "Service " << serviceName << " stopped successfully!" << std::endl;
    } else {
        std::cerr << "Service failed to stop." << std::endl;
    }

    CloseServiceHandle(serviceHandle);
    CloseServiceHandle(scmHandle);
    return true;
}