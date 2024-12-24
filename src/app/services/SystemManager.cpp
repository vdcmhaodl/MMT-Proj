#include "SystemManager.h"

std::vector<std::string> Services::shutdown(Command command) {
    std::string filename;

    if (!Services::shutdown())
        filename = "Cannot shutdown computer";
    else
        filename = "Command execute successfully, computer will shutdown in 5 seconds";

    return std::vector<std::string>({filename});
}

std::vector<std::string> Services::restart(Command command) {
    std::string filename;

    if (!Services::restart()) 
        filename = "Cannot restart computer";
    else
        filename = "Command execute successfully, computer will restart in 5 seconds";

    return std::vector<std::string>({filename});
}

std::vector<std::string> Services::listFileAndFolder(Command command) {
    std::vector<std::string> result;
    for (auto &it : command.listName) {
        std::string filename = "";
        Services::listFileAndFolder(it, filename);
        result.push_back(filename);
    }

    return result;
}

std::vector<std::string> Services::deleteFile(Command command) {
    std::string filename;

    for (auto &it : command.listName) {
        if (!Services::deleteFile(it))
            filename += "Cannot delete file " + it + "\r\n";
        else
            filename += "Delete file " + it + " successfully\r\n";
    }
    return std::vector<std::string>({filename});
}

std::vector<std::string> Services::getFile(Command command) {
    std::string response;
    std::vector<std::string> listFile;
    for (auto &it : command.listName) {
        if (!Services::getFile(it))
            response += "Cannot get file " + it + "\r\n";
        else {
            response += "Get file " + it + " successfully\r\n";

            std::string filename = std::filesystem::path(it).filename().string();
            std::string newFilename = Command::generateFilepathWithFolder(filename);
            CopyFileA(it.c_str(), newFilename.c_str(), TRUE);

            listFile.push_back(newFilename);
        }
    }
    listFile.push_back(response);
    return listFile;
}

std::vector<std::string> Services::listServices(Command command) {
    std::string filename = Command::generateFilepath(10, ".txt");
    Services::listServices(filename);
    return std::vector<std::string>({filename});
}

std::vector<std::string> Services::startService(Command command) {
    std::string filename;

    for (auto &it : command.listName) {
        if (!Services::startService(it))
            filename += "Cannot start service " + it + "\r\n";
        else
            filename += "Start service " + it + " successfully\r\n";
    }
    return std::vector<std::string> ({filename});
}

std::vector<std::string> Services::stopService(Command command) {
    std::string filename;

    for (auto& it : command.listName) {
        if (!Services::stopService(it))
            filename += "Cannot stop service " + it + "\r\n";
        else
            filename += "Stop service " + it + " successfully\r\n";
    }
    return std::vector<std::string> ({filename});
}

bool Services::shutdown() {
    return system("shutdown /s /f /t 5") == 0;
}

bool Services::restart() {
    return system("shutdown /r /f /t 5") == 0;
}

bool Services::listFileAndFolder(const std::string &directory, std::string &fileSave) {
    std::string filename = Command::generateFilepath(10, ".txt");
    fileSave = filename;

    std::ofstream ofs (filename.c_str());
    if (!ofs.is_open()) {
        std::osyncstream(std::cout) << "Fail to open file!\n";
        return false;
    }
    
    WIN32_FIND_DATAA findFileData;
    HANDLE handle = FindFirstFileA((directory + "\\*").c_str(), &findFileData);
    if (handle == INVALID_HANDLE_VALUE) {
        ofs << "Cannot execute command";
        ofs.close();

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

bool Services::getFile(const std::string &filePath) {
    std::ifstream fin(filePath.c_str());
    if (!fin.is_open()) {
        return false;
    }
    else {
        fin.close();
        return true;
    }
}

bool Services::listServices(const std::string &saveFile) {
    std::ofstream fout(saveFile.c_str());
    if (!fout.is_open()) {
        return false;
    }

    SC_HANDLE scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (scmHandle == NULL) {
        std::osyncstream(std::cerr) << "Error opening Service Control Manager: " << GetLastError() << std::endl;
        return false;
    }

    DWORD bytesNeeded = 0;
    DWORD servicesCount = 0;
    DWORD resumeHandle = 0;

    if (!EnumServicesStatusEx(scmHandle, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_ACTIVE, NULL, 0, &bytesNeeded, &servicesCount, &resumeHandle, NULL)) {
        DWORD error = GetLastError();
        if (error != ERROR_MORE_DATA) {
            std::osyncstream(std::cerr) << "Error calling EnumServicesStatusEx for size calculation: " << error << std::endl;
            CloseServiceHandle(scmHandle);
            return false;
        }
    }

    LPBYTE buffer = (LPBYTE)LocalAlloc(LMEM_ZEROINIT, bytesNeeded);
    if (buffer == NULL) {
        std::osyncstream(std::cerr) << "Error allocating memory" << std::endl;
        CloseServiceHandle(scmHandle);
        return false;
    }

    if (!EnumServicesStatusEx(scmHandle, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_ACTIVE, buffer, bytesNeeded, &bytesNeeded, &servicesCount, &resumeHandle, NULL)) {
        std::osyncstream(std::cerr) << "Error enumerating services: " << GetLastError() << std::endl;
        LocalFree(buffer);
        CloseServiceHandle(scmHandle);
        return false;
    }

    LPENUM_SERVICE_STATUS_PROCESS services = (LPENUM_SERVICE_STATUS_PROCESS)buffer;

    fout << std::setw(35) << std::left << "Service name" << std::setw(65) << std::left << "Display name" << "PID" << "\n";
    fout << std::string(150, '-') << "\n";
    for (DWORD i = 0; i < servicesCount; i++) {
        fout << std::setw(35) << std::left;
        fout << (char*)services[i].lpServiceName;
        fout << std::setw(65) << std::left;
        fout << (char*)services[i].lpDisplayName;
        fout << services[i].ServiceStatusProcess.dwProcessId << std::endl;
    }

    LocalFree(buffer);
    CloseServiceHandle(scmHandle);
    fout.close();

    return true;
}

bool Services::startService(const std::string &serviceName)
{
    SC_HANDLE scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (scmHandle == NULL) {
        std::osyncstream(std::cerr) << "Error opening Service Control Manager: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE serviceHandle = OpenServiceA(scmHandle, serviceName.c_str(), SERVICE_START | SERVICE_QUERY_STATUS);
    if (serviceHandle == NULL) {
        std::osyncstream(std::cerr) << "Error opening service: " << GetLastError() << std::endl;
        CloseServiceHandle(scmHandle);
        return false;
    }

    if (!StartService(serviceHandle, 0, NULL)) {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_SERVICE_ALREADY_RUNNING) {
            std::osyncstream(std::cout) << "Service " << serviceName << " is already running." << std::endl;
        } else {
            std::osyncstream(std::cerr) << "Error starting service: " << dwError << std::endl;
        }
        CloseServiceHandle(serviceHandle);
        CloseServiceHandle(scmHandle);
        return false;
    }

    SERVICE_STATUS_PROCESS ssp;
    DWORD bytesNeeded;
    do {
        if (!QueryServiceStatusEx(serviceHandle, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded)) {
            std::osyncstream(std::cerr) << "Error querying service status: " << GetLastError() << std::endl;
            CloseServiceHandle(serviceHandle);
            CloseServiceHandle(scmHandle);
            return false;
        }
    } while (ssp.dwCurrentState == SERVICE_START_PENDING);

    if (ssp.dwCurrentState == SERVICE_RUNNING) {
        std::osyncstream(std::cout) << "Service " << serviceName << " started successfully!" << std::endl;
    } else {
        std::osyncstream(std::cerr) << "Service failed to start." << std::endl;
    }

    CloseServiceHandle(serviceHandle);
    CloseServiceHandle(scmHandle);
    return true;
}

bool Services::stopService(const std::string &serviceName) {
    SC_HANDLE scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (scmHandle == NULL) {
        std::osyncstream(std::cerr) << "Error opening Service Control Manager: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE serviceHandle = OpenServiceA(scmHandle, serviceName.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (serviceHandle == NULL) {
        std::osyncstream(std::cerr) << "Error opening service: " << GetLastError() << std::endl;
        CloseServiceHandle(scmHandle);
        return false;
    }

    SERVICE_STATUS status;
    if (!ControlService(serviceHandle, SERVICE_CONTROL_STOP, &status)) {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_SERVICE_NOT_ACTIVE) {
            std::osyncstream(std::cout) << "Service " << serviceName << " is not running." << std::endl;
        } else {
            std::osyncstream(std::cerr) << "Error stopping service: " << dwError << std::endl;
        }
        CloseServiceHandle(serviceHandle);
        CloseServiceHandle(scmHandle);
        return false;
    }

    SERVICE_STATUS_PROCESS ssp;
    DWORD bytesNeeded;
    do {
        if (!QueryServiceStatusEx(serviceHandle, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded)) {
            std::osyncstream(std::cerr) << "Error querying service status: " << GetLastError() << std::endl;
            CloseServiceHandle(serviceHandle);
            CloseServiceHandle(scmHandle);
            return false;
        }
    } while (ssp.dwCurrentState == SERVICE_STOP_PENDING);

    if (ssp.dwCurrentState == SERVICE_STOPPED) {
        std::osyncstream(std::cout) << "Service " << serviceName << " stopped successfully!" << std::endl;
    } else {
        std::osyncstream(std::cerr) << "Service failed to stop." << std::endl;
    }

    CloseServiceHandle(serviceHandle);
    CloseServiceHandle(scmHandle);
    return true;
}