#include "UI.h"

void UI::setMode(int mode) {
    this->mode = mode;
    // EXTRA.setMode(mode);
}

UI::UI(int WIDTH, int HEIGHT, int mode, Mediator* mediator, std::string name) : BaseWindow<UI>(WIDTH, HEIGHT),
                                                          MAIL(640, 360),
                                                          HELP(1280, 720),
                                                          IP(640, 360),
                                                          ABOUT(640, 360),
                                                          mode(mode),
                                                          Participant(mediator, name) {}

LRESULT UI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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

        hwndButtonMail = CreateWindowW(L"BUTTON", L"ACCOUNT", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                25, 25, 75, 75, m_hwnd, (HMENU)MAIL_BUTTON, NULL, NULL);
        SendMessage(hwndButtonMail, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
        hwndButtonHelp = CreateWindowW(L"BUTTON", L"HELP", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                25, 125, 75, 75, m_hwnd, (HMENU)HELP_BUTTON, NULL, NULL);
        SendMessage(hwndButtonHelp, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
        hwndButtonIP = CreateWindowW(L"BUTTON", L"IP", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                25, 225, 75, 75, m_hwnd, (HMENU)IP_BUTTON, NULL, NULL);
        SendMessage(hwndButtonIP, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
        hwndButtonAbout = CreateWindowW(L"BUTTON", L"ABOUT", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                25, 325, 75, 75, m_hwnd, (HMENU)ABOUT_BUTTON, NULL, NULL);
        SendMessage(hwndButtonAbout, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));

        std::ifstream logFile("Info.txt"); 
        std::string line;
        std::string logContent; 

        std::getline(logFile, line);
        logContent += line;
        while (std::getline(logFile, line)) { 
            logContent += "\r\n" + line; 
        }

        // MessageBoxA(NULL, logContent.c_str(), "LOG", MB_OK);
        logFile.close(); 

        logFont.lfWeight = FW_NORMAL;
        // printf("%s", (char*)logFont.lfFaceName);

        s_hFont = CreateFontIndirect(&logFont);

        hInfo = CreateWindowExA(0, "EDIT", logContent.c_str(), 
            WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 
            125, 25, 775, 75, m_hwnd, (HMENU)1, GetModuleHandle(NULL), NULL ); 
        SendMessage(hInfo, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));

        hEdit = CreateWindowExA(0, "EDIT", NULL, 
            WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 
            125, 125, 775, 275, m_hwnd, (HMENU)0, GetModuleHandle(NULL), NULL ); 
        SendMessage(hEdit, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));

        ReleaseDC(m_hwnd, hdc);
        break;
    }
    
    case WM_COMMAND: {
        int identifier = LOWORD(wParam);
        // std::osyncstream(std::cout) << identifier << '\n';
        switch (identifier) {
        case MAIL_BUTTON:
            // MessageBoxW(m_hwnd, L"Info button!", L"Child Window", MB_OK);
            if (MAIL.Window() == NULL) {
                MAIL.Create("VALID MAIL ACCOUNT", WS_OVERLAPPEDWINDOW, WS_EX_CLIENTEDGE);
                ShowWindow(MAIL.Window(), SW_SHOWDEFAULT);
                UpdateWindow(MAIL.Window());
            }
            break;
        
        case HELP_BUTTON:
            // MessageBoxW(m_hwnd, L"Help button!", L"Child Window", MB_OK);
            if (HELP.Window() == NULL) {
                HELP.Create("HELP", WS_OVERLAPPEDWINDOW, WS_EX_CLIENTEDGE);
                ShowWindow(HELP.Window(), SW_SHOWDEFAULT);
                UpdateWindow(HELP.Window());
            }
            break;

        case IP_BUTTON:
            // MessageBoxW(m_hwnd, L"IP button!", L"Child Window", MB_OK);
            if (IP.Window() == NULL) {
                IP.Create("LIST OF SERVER IP", WS_OVERLAPPEDWINDOW, WS_EX_CLIENTEDGE);
                ShowWindow(IP.Window(), SW_SHOWDEFAULT);
            }
            break;

        case ABOUT_BUTTON:
            // MessageBoxW(m_hwnd, L"Log button!", L"Child Window", MB_OK);
            if (ABOUT.Window() == NULL) {
                ABOUT.Create("ABOUT", WS_OVERLAPPEDWINDOW, WS_EX_CLIENTEDGE);
                ShowWindow(ABOUT.Window(), SW_SHOWDEFAULT);
                UpdateWindow(ABOUT.Window());
            }
            break;
        
        case IP_UPDATE_MESSAGE: {
            // MessageBoxA(INFO.Window(), "Receive new status!", "WM_COMMAND", MB_OK);
            // if (IP.Window() != NULL) {
            //     SendMessageA(IP.Window(), WM_COMMAND, MAKEWORD(ListViewWindow::UPDATE_LIST_VIEW, 0), 0);
            // }
        } break;
        
        default:
            break;
        }
    } break;

    case WM_IP_REQUEST: {
        mediator->Forward(COMPONENT::UI_COMPONENT, COMPONENT::BROADCAST_COMPONENT, "request");
        break;
    }

    case WM_COPYDATA: {
        PCOPYDATASTRUCT pCDS = (PCOPYDATASTRUCT)lParam;
        std::string content((char*)pCDS->lpData);
        switch (pCDS->dwData) {
        case IP_MESSAGE: {
            std::string IP_addr, name, status;
            socketAPI::decipherServerMessage(content, IP_addr, name, status);
            break;
        }
        case EXTRA_MESSAGE:{
            std::string message = content + std::string("; RECEIVED TEXT!");
            break;
        }
        default:
            break;
        }
        break;
    }

    case WM_FRONTEND_NOTIFY: {
        switch(wParam) {
            case INFO_UPDATE_MESSAGE: {
                if (MAIL.Window() != NULL) {
                    SendMessageA(IP.Window(), WM_TEXT_APPEND, 0, lParam);
                }
                break;
            }
            case IP_UPDATE_MESSAGE: {
                // std::osyncstream(std::cout) << "Recieve frontend message IP\n";
                if (IP.Window() != NULL) {
                    SendMessageA(IP.Window(), WM_IP_NOTIFY, 0, lParam);
                }
                break;
            }
            case LOG_UPDATE_MESSAGE: {
                std::any* any_ptr = reinterpret_cast<std::any*>(lParam);
                std::string text = std::any_cast<std::string>(*any_ptr);
                
                auto appendTextToEdit = [&](LPCSTR newText)
                {
                    int TextLen = SendMessageA(hEdit, WM_GETTEXTLENGTH, 0, 0);
                    SendMessageA(hEdit, EM_SETSEL, (WPARAM)TextLen, (LPARAM)TextLen);
                    SendMessageA(hEdit, EM_REPLACESEL, FALSE, (LPARAM)(newText));
                };

                std::ofstream logFile("LOG.txt", std::ios::app);
                logFile << text + "\n";
                
                appendTextToEdit(text.c_str());
                appendTextToEdit("\r\n");
                break;
            }
        }
    } break;

    case WM_DESTROY:{
        PostQuitMessage(0);
        return 0;
    }
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

    case WM_CLOSE: {
            if (MessageBoxW(m_hwnd, L"Really quit?", L"EXIT", MB_OKCANCEL) == IDOK) {
                SendMessageA(MAIL.Window(), WM_CLOSE, 0, 0);
                SendMessageA(HELP.Window(), WM_CLOSE, 0, 0);
                SendMessageA(IP.Window(), WM_CLOSE, 0, 0);
                DestroyWindow(m_hwnd);
                DeleteObject(s_hFont);

                mediator->Broadcast(this, "exit");
            }
            // Else: User canceled. Do nothing.
            return 0;
        }

    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return TRUE;
}

void UI::initialize(std::vector<std::string> &signinInput) {
    setMode(0);
    MAIL.setFilePath("AdminAccount.txt");
    HELP.setFilePath("HELP.txt");
    ABOUT.setFilePath("ABOUT.txt");
}

void UI::start() {
    if (!Create("CLIENT", 
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX, WS_EX_CLIENTEDGE)) {
        return;
    }

    MAIL.update_hwnd_parent(Window());
    HELP.update_hwnd_parent(Window());
    IP.update_hwnd_parent(Window());

    ShowWindow(Window(), SW_SHOWDEFAULT);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return;
}

void UI::Receive(std::string msg) {
    // std::osyncstream(std::cout) << "String: " << msg;
}

void UI::Receive(std::any *ptr, std::string type) {
    // std::osyncstream(std::cout) << "Receive ptr " << ptr << ' ' << type << '\n';
    if (type == "IP") {
        SendMessageA(m_hwnd, WM_FRONTEND_NOTIFY, IP_UPDATE_MESSAGE, (LPARAM)ptr);
    }
    else if (type == "INFO") {
        SendMessageA(m_hwnd, WM_FRONTEND_NOTIFY, INFO_UPDATE_MESSAGE, (LPARAM)ptr);
    }
    else if (type == "LOG") {
        std::string text = std::any_cast<std::string>(*ptr);
        SendMessageA(m_hwnd, WM_FRONTEND_NOTIFY, LOG_UPDATE_MESSAGE, (LPARAM)ptr);
    }
}
