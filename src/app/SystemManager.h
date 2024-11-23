#pragma once

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

bool ShutdownComputer();
bool RestartComputer();
bool DelFile(const std::string &filePath);

bool ListSers(const std::string &saveFile);
bool StartSer(const std::string &serviceName);
bool StopSer(const std::string &serviceName);