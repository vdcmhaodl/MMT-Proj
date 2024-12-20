#include "input.h"

InputWindow::InputWindow(int WIDTH, int HEIGHT) : BaseWindow<InputWindow>(WIDTH, HEIGHT) {}

PCSTR InputWindow::ClassName() const { return "Input Window Class"; }

LRESULT InputWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_CREATE: {
            const char* fontName = "Consolas";
            const long nFontSize = 13;

            HDC hdc = GetDC(m_hwnd);
            LOGFONT logFont = {0};
            logFont.lfHeight = -MulDiv(nFontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
            logFont.lfWeight = FW_BOLD;
            strcpy((char*)logFont.lfFaceName, fontName);

            // printf("%s", (char*)logFont.lfFaceName);

            s_hFont = CreateFontIndirect(&logFont);

            // Create a static control for the header
            hwndHeader = CreateWindowW(L"STATIC", L"SIGN IN",
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                50, 20, 500, 25,
                m_hwnd, NULL, NULL, NULL);
            SendMessage(hwndHeader, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));

            // Create static controls for descriptions
            hwndDesc1 = CreateWindowW(L"STATIC", L"IP address",
                WS_CHILD | WS_VISIBLE,
                50, 80, 150, 25,
                m_hwnd, NULL, NULL, NULL);
            SendMessage(hwndDesc1, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));

            hwndDesc2 = CreateWindowW(L"STATIC", L"Subnet mask",
                WS_CHILD | WS_VISIBLE,
                50, 120, 150, 25,
                m_hwnd, NULL, NULL, NULL);
            SendMessage(hwndDesc2, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));

            hwndDesc3 = CreateWindowW(L"STATIC", L"Username",
                WS_CHILD | WS_VISIBLE,
                50, 160, 150, 25,
                m_hwnd, NULL, NULL, NULL);
            SendMessage(hwndDesc3, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
            
            hwndDesc4 = CreateWindowW(L"STATIC", L"Password",
                WS_CHILD | WS_VISIBLE,
                50, 200, 150, 25,
                m_hwnd, NULL, NULL, NULL);
            SendMessage(hwndDesc4, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));

            logFont.lfWeight = FW_NORMAL;
            s_hFont = CreateFontIndirect(&logFont);
            
            // Create child windows with a distinct background
            hwndInput1 = CreateWindowExW(0,
                                        L"EDIT",
                                        L"",
                                        WS_CHILD | WS_VISIBLE | WS_BORDER,
                                        215, 80, 335, 25,
                                        m_hwnd, NULL, NULL, NULL);
            SendMessage(hwndInput1, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));

            hwndInput2 = CreateWindowExW(0,
                                        L"EDIT",
                                        L"",
                                        WS_CHILD | WS_VISIBLE | WS_BORDER,
                                        215, 120, 335, 25,
                                        m_hwnd, NULL, NULL, NULL);
            SendMessage(hwndInput2, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));

            hwndInput3 = CreateWindowExW(0,
                                        L"EDIT",
                                        L"",
                                        WS_CHILD | WS_VISIBLE | WS_BORDER,
                                        215, 160, 335, 25,
                                        m_hwnd, NULL, NULL, NULL);
            SendMessage(hwndInput3, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));

            hwndInput4 = CreateWindowExW(0,
                                        L"EDIT",
                                        L"",
                                        WS_CHILD | WS_VISIBLE | WS_BORDER,
                                        215, 200, 335, 25,
                                        m_hwnd, NULL, NULL, NULL);
            SendMessage(hwndInput4, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));

            ReleaseDC(m_hwnd, hdc);

            // Create a combo box for selection
            // hwndComboBox = CreateWindowW(L"COMBOBOX", NULL,
            //     WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER,
            //     175, 230, 100, 100,
            //     m_hwnd, (HMENU) 2, NULL, NULL);

            // Add items to the combo box
            // SendMessageW(hwndComboBox, CB_ADDSTRING, 0, (LPARAM) L"Automatic");
            // SendMessageW(hwndComboBox, CB_ADDSTRING, 0, (LPARAM) L"Manual");

            // Create a static control for the combo box description
            // hwndComboDesc = CreateWindowW(L"STATIC", L"Mode:",
            //     WS_CHILD | WS_VISIBLE,
            //     50, 230, 100, 25,
            //     m_hwnd, NULL, NULL, NULL);
            
            hwndButton = CreateWindowExW(0,
                                        L"BUTTON",
                                        L"Submit",
                                        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                        250, 250, 100, 30,
                                        m_hwnd, (HMENU)INPUT_BUTTON, NULL, NULL);
            SendMessage(hwndButton, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));

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
            char buffer[256];

            GetWindowTextA(hwndInput1, buffer, 256);
            IPaddr = std::string(buffer);

            GetWindowTextA(hwndInput2, buffer, 256);
            subnetMask = std::string(buffer);

            GetWindowTextA(hwndInput3, buffer, 256);
            mail = std::string(buffer);

            GetWindowTextA(hwndInput4, buffer, 256);
            password = std::string(buffer);

            // Get the selected option from the combo box
            // int itemIndex = SendMessageA(hwndComboBox, CB_GETCURSEL, 0, 0);
            // char itemText[256];
            // SendMessage(hwndComboBox, CB_GETLBTEXT, itemIndex, (LPARAM)itemText);
            // selectedOption = itemText;

            // Clear the child windows
            DestroyWindow(hwndInput1);
            DestroyWindow(hwndInput2);
            DestroyWindow(hwndInput3);
            DestroyWindow(hwndInput4);
            // DestroyWindow(hwndComboBox);
            DestroyWindow(hwndButton);
            DestroyWindow(hwndHeader);
            DestroyWindow(hwndDesc1);
            DestroyWindow(hwndDesc2);
            DestroyWindow(hwndDesc3);
            DestroyWindow(hwndDesc4);
            // DestroyWindow(hwndComboDesc);
            inputCollected = true;

            // Trigger a repaint to display the collected inputs
            InvalidateRect(m_hwnd, NULL, TRUE);

            DestroyWindow(m_hwnd);
        } break;
        
        default:
            break;
        }
    } break;

    case WM_DESTROY:
        DeleteObject(s_hFont);
        PostQuitMessage(0);
        return 0;

    case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(m_hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
            EndPaint(m_hwnd, &ps);
        }
        return 0;
    
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

bool InputWindow::validinput() {
    return true;
}