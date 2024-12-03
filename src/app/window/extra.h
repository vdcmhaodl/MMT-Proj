#pragma once

#include "base_window.h"
#include <map>
#include <string>

class ExtraWindow : public BaseWindow<ExtraWindow>
{
private:
    HWND hwndUsername, hwndPassword, hwndComboBoxIP, hwndComboBoxCommand;
    HWND hwndUsernameDesc, hwndPasswordDesc, hwndComboIPDesc, hwndComboCommandDesc;
    HWND hwndButton;
    static const int INPUT_BUTTON = 1;
public:
    bool mode;
    std::string newUsername;
    std::string newPassword;
    std::string IP;
    std::string command;
    ExtraWindow(int WIDTH = 640, int HEIGHT = 360, int mode = AUTOMATIC);
    void createAutomatic();
    void createManual();
    std::string readAutomatic();
    std::string readManual();
    std::string query;
    std::string sendCommand();
    PCSTR  ClassName() const;
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};