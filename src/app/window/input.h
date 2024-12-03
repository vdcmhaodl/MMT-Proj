#pragma once
#include "base_window.h"

class InputWindow : public BaseWindow<InputWindow>
{
private:
    HWND hwndInput1, hwndInput2, hwndInput3, hwndComboBox;
    HWND hwndButton;
    HWND hwndHeader; 
    HWND hwndDesc1, hwndDesc2, hwndDesc3, hwndComboDesc; 
    static const int INPUT_BUTTON = 1;
    bool inputCollected = false;
    std::vector<std::wstring> inputs = std::vector<std::wstring>(3);
public:
    InputWindow(int WIDTH = 1280, int HEIGHT = 720);
    std::string IPaddr;
    std::string subnetMask;
    std::string mail;
    std::string password;
    std::string selectedOption = "";
    PCSTR  ClassName() const;
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool validinput();
};

