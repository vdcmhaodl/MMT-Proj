#include "input.h"

InputWindow::InputWindow(int WIDTH, int HEIGHT) : BaseWindow<InputWindow>(WIDTH, HEIGHT) {}

PCSTR InputWindow::ClassName() const { return "Input Window Class"; }

LRESULT InputWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_CREATE: {
            // Create a static control for the header
            hwndHeader = CreateWindowW(L"STATIC", L"SIGN IN",
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                50, 20, 400, 30,
                m_hwnd, NULL, NULL, NULL);

            // Create static controls for descriptions
            hwndDesc1 = CreateWindowW(L"STATIC", L"IP address:",
                WS_CHILD | WS_VISIBLE,
                50, 70, 100, 25,
                m_hwnd, NULL, NULL, NULL);

            hwndDesc2 = CreateWindowW(L"STATIC", L"Subnet mask:",
                WS_CHILD | WS_VISIBLE,
                50, 110, 100, 25,
                m_hwnd, NULL, NULL, NULL);

            hwndDesc3 = CreateWindowW(L"STATIC", L"Mail account:",
                WS_CHILD | WS_VISIBLE,
                50, 150, 100, 25,
                m_hwnd, NULL, NULL, NULL);
            
            hwndDesc4 = CreateWindowW(L"STATIC", L"Password:",
                WS_CHILD | WS_VISIBLE,
                50, 190, 100, 25,
                m_hwnd, NULL, NULL, NULL);

            // Create child windows with a distinct background
            hwndInput1 = CreateWindowExW(0,
                                        L"EDIT",
                                        L"",
                                        WS_CHILD | WS_VISIBLE | WS_BORDER,
                                        175, 70, 300, 25,
                                        m_hwnd, NULL, NULL, NULL);

            hwndInput2 = CreateWindowExW(0,
                                        L"EDIT",
                                        L"",
                                        WS_CHILD | WS_VISIBLE | WS_BORDER,
                                        175, 110, 300, 25,
                                        m_hwnd, NULL, NULL, NULL);
            
            hwndInput3 = CreateWindowExW(0,
                                        L"EDIT",
                                        L"",
                                        WS_CHILD | WS_VISIBLE | WS_BORDER,
                                        175, 150, 300, 25,
                                        m_hwnd, NULL, NULL, NULL);
            
            hwndInput4 = CreateWindowExW(0,
                                        L"EDIT",
                                        L"",
                                        WS_CHILD | WS_VISIBLE | WS_BORDER,
                                        175, 190, 300, 25,
                                        m_hwnd, NULL, NULL, NULL);
            
            // Create a combo box for selection
            hwndComboBox = CreateWindowW(L"COMBOBOX", NULL,
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER,
                175, 230, 100, 100,
                m_hwnd, (HMENU) 2, NULL, NULL);

            // Add items to the combo box
            SendMessageW(hwndComboBox, CB_ADDSTRING, 0, (LPARAM) L"Automatic");
            SendMessageW(hwndComboBox, CB_ADDSTRING, 0, (LPARAM) L"Manual");

            // Create a static control for the combo box description
            hwndComboDesc = CreateWindowW(L"STATIC", L"Mode:",
                WS_CHILD | WS_VISIBLE,
                50, 230, 100, 25,
                m_hwnd, NULL, NULL, NULL);
            
            hwndButton = CreateWindowExW(0,
                                        L"BUTTON",
                                        L"Submit",
                                        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                        150, 270, 100, 30,
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
            int itemIndex = SendMessageA(hwndComboBox, CB_GETCURSEL, 0, 0);
            char itemText[256];
            SendMessage(hwndComboBox, CB_GETLBTEXT, itemIndex, (LPARAM)itemText);
            selectedOption = itemText;

            // Clear the child windows
            DestroyWindow(hwndInput1);
            DestroyWindow(hwndInput2);
            DestroyWindow(hwndInput3);
            DestroyWindow(hwndInput4);
            DestroyWindow(hwndComboBox);
            DestroyWindow(hwndButton);
            DestroyWindow(hwndHeader);
            DestroyWindow(hwndDesc1);
            DestroyWindow(hwndDesc2);
            DestroyWindow(hwndDesc3);
            DestroyWindow(hwndDesc4);
            DestroyWindow(hwndComboDesc);
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
    return (!IPaddr.empty() && !subnetMask.empty() && !selectedOption.empty())
        && ((selectedOption == "Automatic" && !mail.empty()) || (selectedOption == "Manual" && mail.empty())); 
}