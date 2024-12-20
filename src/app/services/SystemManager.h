#pragma once

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include "serviceCommand.h"

namespace Services{
    bool shutdown();
    bool restart();

    bool listFileAndFolder(const std::string &directory, std::string &fileSave);
    bool deleteFile(const std::string &filePath);
    bool getFile(const std::string &filePath);

    bool listServices(const std::string &saveFile);
    bool startService(const std::string &serviceName);
    bool stopService(const std::string &serviceName);
    
    std::vector<std::string> shutdown(Command command);
    std::vector<std::string> restart(Command command);

    std::vector<std::string> listFileAndFolder(Command command);
    std::vector<std::string> deleteFile(Command command);
    std::vector<std::string> getFile(Command command);

    std::vector<std::string> listServices(Command command);
    std::vector<std::string> startService(Command command);
    std::vector<std::string> stopService(Command command);
}