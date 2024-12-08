#include "extra.h"

void ExtraWindow::setMode(bool mode) {
    this->mode = mode;
}

ExtraWindow::ExtraWindow(int WIDTH, int HEIGHT, int mode) : BaseWindow<ExtraWindow>(WIDTH, HEIGHT), mode(mode) {}

void ExtraWindow::createAutomatic() {
    // Create static controls for descriptions
    hwndUsernameDesc = CreateWindowW(L"STATIC", L"Username:",
        WS_CHILD | WS_VISIBLE,
        50, 70, 100, 25,
        m_hwnd, NULL, NULL, NULL);

    hwndPasswordDesc = CreateWindowW(L"STATIC", L"Password:",
        WS_CHILD | WS_VISIBLE,
        50, 110, 100, 25,
        m_hwnd, NULL, NULL, NULL);
    
    // Create child windows with a distinct background
    hwndUsername = CreateWindowExW(0,
                                L"EDIT",
                                L"",
                                WS_CHILD | WS_VISIBLE | WS_BORDER,
                                175, 70, 300, 25,
                                m_hwnd, NULL, NULL, NULL);

    hwndPassword = CreateWindowExW(0,
                                L"EDIT",
                                L"",
                                WS_CHILD | WS_VISIBLE | WS_BORDER,
                                175, 110, 300, 25,
                                m_hwnd, NULL, NULL, NULL);
}

void ExtraWindow::createManual() {
    hwndComboIPDesc = CreateWindowW(L"STATIC", L"IP:",
    WS_CHILD | WS_VISIBLE,
    50, 150, 100, 25,
    m_hwnd, NULL, NULL, NULL);

// Create a static control for the combo box description
    hwndComboCommandDesc = CreateWindowW(L"STATIC", L"Command:",
        WS_CHILD | WS_VISIBLE,
        50, 190, 100, 25,
        m_hwnd, NULL, NULL, NULL);

    // Create a combo box for selection
    hwndComboBoxIP = CreateWindowW(L"COMBOBOX", NULL,
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER,
        175, 150, 100, 100,
        m_hwnd, (HMENU) 2, NULL, NULL);

    // Add items to the combo box
    SendMessageW(hwndComboBoxIP, CB_ADDSTRING, 0, (LPARAM) L"0.0.0.0");
    SendMessageW(hwndComboBoxIP, CB_ADDSTRING, 0, (LPARAM) L"192.168.1.1");
    SendMessageW(hwndComboBoxIP, CB_ADDSTRING, 0, (LPARAM) L"127.0.0.1");

    // Create a combo box for selection
    hwndComboBoxCommand = CreateWindowW(L"COMBOBOX", NULL,
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER,
        175, 190, 100, 100,
        m_hwnd, (HMENU) 2, NULL, NULL);

    // Add items to the combo box
    SendMessageW(hwndComboBoxCommand, CB_ADDSTRING, 0, (LPARAM) L"Application-List");
    SendMessageW(hwndComboBoxCommand, CB_ADDSTRING, 0, (LPARAM) L"Application-Start");
    SendMessageW(hwndComboBoxCommand, CB_ADDSTRING, 0, (LPARAM) L"Application-Stop");
}

std::string ExtraWindow::readAutomatic() {
    char buffer[256];

    GetWindowTextA(hwndUsername, buffer, 256);
    newUsername = std::string(buffer);

    GetWindowTextA(hwndPassword, buffer, 256);
    newPassword = std::string(buffer);

    query = std::string("mail-change;") + newUsername + std::string(1, ';') + newPassword; 

    DestroyWindow(hwndUsername);
    DestroyWindow(hwndPassword);
    DestroyWindow(hwndUsernameDesc);
    DestroyWindow(hwndPasswordDesc);

    return std::string();
}

std::string ExtraWindow::readManual() {
    // Get the selected option from the combo box
    int itemIndex = SendMessageA(hwndComboBoxIP, CB_GETCURSEL, 0, 0);
    char itemText[256];
    SendMessage(hwndComboBoxIP, CB_GETLBTEXT, itemIndex, (LPARAM)itemText);
    IP = std::string(itemText);

    // Get the selected option from the combo box
    itemIndex = SendMessageA(hwndComboBoxCommand, CB_GETCURSEL, 0, 0);
    SendMessage(hwndComboBoxCommand, CB_GETLBTEXT, itemIndex, (LPARAM)itemText);
    command = std::string(itemText);

    query = std::string("query-local;") + IP + std::string(1, ';') + command; 

    DestroyWindow(hwndComboBoxIP);
    DestroyWindow(hwndComboBoxCommand);
    DestroyWindow(hwndComboIPDesc);
    DestroyWindow(hwndComboCommandDesc);

    return std::string();
}

std::string ExtraWindow::sendCommand() {
    COPYDATASTRUCT cds = wrapperData(query, 4);
    SendMessage(m_hwnd_parent, WM_COPYDATA, (WPARAM)m_hwnd, (LPARAM)&cds);
    return std::string();
}

PCSTR ExtraWindow::ClassName() const { return "Extra Window Class"; }

LRESULT ExtraWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_CREATE: {
            if (mode == AUTOMATIC)
                createAutomatic();
            else 
                createManual();
            hwndButton = CreateWindowExW(0,
                                        L"BUTTON",
                                        L"Submit",
                                        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                        150, 230, 100, 30,
                                        m_hwnd, (HMENU)INPUT_BUTTON, NULL, NULL);
            
            // // Set background color to make areas stand out
            HBRUSH hBrush = CreateSolidBrush(RGB(173, 216, 230)); // Light blue
            // SetClassLongPtrW(hwndInput1, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
            // SetClassLongPtrW(hwndInput2, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
            // SetClassLongPtrW(hwndInput3, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
            DeleteObject(hBrush);
            break;
        }
    
    case WM_COMMAND: {
        int identifier = LOWORD(wParam);
        switch (identifier)
        {
        case INPUT_BUTTON: {
            if (mode == AUTOMATIC) 
                readAutomatic();
            else
                readManual();
            sendCommand();
            DestroyWindow(hwndButton);
            // Trigger a repaint to display the collected inputs
            InvalidateRect(m_hwnd, NULL, TRUE);

            DestroyWindow(m_hwnd);
        } break;
        
        default:
            break;
        }
    } break;

    case WM_DESTROY:
        m_hwnd = NULL;
        // PostQuitMessage(0);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
        EndPaint(m_hwnd, &ps);
    } return 0;
    
    case WM_NCHITTEST: { // Prevent resizing with the mouse 
        LRESULT hit = DefWindowProc(m_hwnd, uMsg, wParam, lParam); 
        if (hit == HTBOTTOM || 
            hit == HTRIGHT || 
            hit == HTBOTTOMRIGHT || 
            hit == HTLEFT || 
            hit == HTTOP || 
            hit == HTTOPLEFT || 
            hit == HTTOPRIGHT || 
            hit == HTBOTTOMLEFT) { 
            return HTCLIENT; 
        } 
        return hit;
    } 

    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return TRUE;
}