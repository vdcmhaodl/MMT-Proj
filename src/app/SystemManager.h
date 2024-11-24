#pragma once

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

namespace Services{
    bool shutdown(const std::string &saveFile);
    bool restart(const std::string &saveFile);

    bool listFileAndFolder(const std::string &directory);
    bool deleteFile(const std::string &filePath);

    bool listServices(const std::string &saveFile);
    bool startService(const std::string &serviceName);
    bool stopService(const std::string &serviceName);
}