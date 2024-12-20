#include "UI.h"

void UI::setMode(int mode) {
    this->mode = mode;
    // EXTRA.setMode(mode);
}

UI::UI(int WIDTH, int HEIGHT, int mode, Mediator* mediator, std::string name) : BaseWindow<UI>(WIDTH, HEIGHT),
                                                          MAIL(640, 360),
                                                          HELP(960, 480),
                                                          IP(640, 360),
                                                          ABOUT(640, 360),
                                                        //   LOG(640, 360),
                                                        //   EXTRA(640, 360, mode),
                                                          mode(mode),
                                                          Participant(mediator, name) {}

LRESULT UI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE: {
        hwndButtonMail = CreateWindowW(L"BUTTON", L"MAIL", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                50, 25, 75, 75, m_hwnd, (HMENU)MAIL_BUTTON, NULL, NULL);
        hwndButtonHelp = CreateWindowW(L"BUTTON", L"HELP", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                50, 125, 75, 75, m_hwnd, (HMENU)HELP_BUTTON, NULL, NULL);
        hwndButtonIP = CreateWindowW(L"BUTTON", L"IP LIST", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                50, 225, 75, 75, m_hwnd, (HMENU)IP_BUTTON, NULL, NULL);
        hwndButtonAbout = CreateWindowW(L"BUTTON", L"ABOUT", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                50, 325, 75, 75, m_hwnd, (HMENU)ABOUT_BUTTON, NULL, NULL);

        std::ifstream logFile("Info.txt"); 
        // std::cerr << "filepath: " << filepath << '\n';
        std::string line;
        std::string logContent; 

        std::getline(logFile, line);
        logContent += line;
        while (std::getline(logFile, line)) { 
            logContent += "\r\n" + line; 
        }

        // MessageBoxA(NULL, logContent.c_str(), "LOG", MB_OK);
        logFile.close(); 

        hInfo = CreateWindowExA(0, "EDIT", logContent.c_str(), 
            WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 
            175, 25, 625, 75, m_hwnd, (HMENU)1, GetModuleHandle(NULL), NULL ); 
        
        HFONT hFont = (HFONT)GetStockObject(ANSI_FIXED_FONT); 
        SendMessageA(hInfo, WM_SETFONT, (WPARAM)hFont, TRUE);

        hEdit = CreateWindowExA(0, "EDIT", NULL, 
            WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 
            175, 125, 625, 275, m_hwnd, (HMENU)0, GetModuleHandle(NULL), NULL ); 
        // hwndButtonLog = CreateWindowW(L"BUTTON", L"LOG", WS_VISIBLE | WS_CHILD | WS_BORDER,
        //                         50, 350, 100, 30, m_hwnd, (HMENU)LOG_BUTTON, NULL, NULL);
        // hwndButtonExtra = CreateWindowW(L"BUTTON", L"EXTRA", WS_VISIBLE | WS_CHILD | WS_BORDER,
        //                         50, 450, 100, 30, m_hwnd, (HMENU)EXTRA_BUTTON, NULL, NULL);
        break;
    }
    
    case WM_COMMAND: {
        int identifier = LOWORD(wParam);
        std::osyncstream(std::cout) << identifier << '\n';
        switch (identifier) {
        case MAIL_BUTTON:
            // MessageBoxW(m_hwnd, L"Info button!", L"Child Window", MB_OK);
            if (MAIL.Window() == NULL) {
                MAIL.Create("Mail", WS_OVERLAPPEDWINDOW, WS_EX_CLIENTEDGE);
                ShowWindow(MAIL.Window(), SW_SHOWDEFAULT);
                UpdateWindow(MAIL.Window());
            }
            break;
        
        case HELP_BUTTON:
            // MessageBoxW(m_hwnd, L"Help button!", L"Child Window", MB_OK);
            if (HELP.Window() == NULL) {
                HELP.Create("Help", WS_OVERLAPPEDWINDOW, WS_EX_CLIENTEDGE);
                ShowWindow(HELP.Window(), SW_SHOWDEFAULT);
                UpdateWindow(HELP.Window());
            }
            break;

        case IP_BUTTON:
            // MessageBoxW(m_hwnd, L"IP button!", L"Child Window", MB_OK);
            if (IP.Window() == NULL) {
                IP.Create("Ip adresses", WS_OVERLAPPEDWINDOW, WS_EX_CLIENTEDGE);
                ShowWindow(IP.Window(), SW_SHOWDEFAULT);
                UpdateWindow(IP.Window());
            }
            break;

        case ABOUT_BUTTON:
            // MessageBoxW(m_hwnd, L"Log button!", L"Child Window", MB_OK);
            if (ABOUT.Window() == NULL) {
                ABOUT.Create("About", WS_OVERLAPPEDWINDOW, WS_EX_CLIENTEDGE);
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
            MessageBoxA(m_hwnd, (char*)content.c_str(), "IP message", MB_OK);
            
            break;
        }
        case EXTRA_MESSAGE:{
            
            std::string message = content + std::string("; RECEIVED TEXT!");
            MessageBoxA(m_hwnd, (char*)message.c_str(), "Extra message", MB_OK);
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
                std::osyncstream(std::cout) << "Recieve frontend message IP\n";
                if (IP.Window() != NULL) {
                    SendMessageA(IP.Window(), WM_IP_NOTIFY, 0, lParam);
                }
                break;
            }
            case LOG_UPDATE_MESSAGE: {
                std::osyncstream(std::cout) << "Recieve frontend message LOG\n";
                std::any* any_ptr = reinterpret_cast<std::any*>(lParam);
                std::string text = std::any_cast<std::string>(*any_ptr);
                std::osyncstream(std::cout) << "TEXT: " << text << '\n';
                
                auto appendTextToEdit = [&](LPCSTR newText)
                {
                    int TextLen = SendMessageA(hEdit, WM_GETTEXTLENGTH, 0, 0);
                    SendMessageA(hEdit, EM_SETSEL, (WPARAM)TextLen, (LPARAM)TextLen);
                    SendMessageA(hEdit, EM_REPLACESEL, FALSE, (LPARAM)(newText));
                };

                std::ofstream logFile("LOG.txt", std::ios::app);
                logFile << text + "\n";
                appendTextToEdit("\r\n");
                appendTextToEdit(text.c_str());

                // std::osyncstream(std::cout) << "LOG text: " << text << '\n';
                // LOG.getNewMessage(text);
                // if (LOG.Window() != NULL) {
                //     SendMessageA(LOG.Window(), WM_TEXT_APPEND, 0, lParam);
                // }
                break;
            }
        }
    } break;

    case WM_DESTROY:{
        std::string resultINFO = (MAIL.Window() == NULL ? "SUCCESS" : "FAIL");
        std::string resultHELP = (HELP.Window() == NULL ? "SUCCESS" : "FAIL");
        std::string resultIP = (IP.Window() == NULL ? "SUCCESS" : "FAIL");
        // std::string resultLOG = (LOG.Window() == NULL ? "SUCCESS" : "FAIL");
        // std::string resultEXTRA = (EXTRA.Window() == NULL ? "SUCCESS" : "FAIL");
        // MessageBoxA(INFO.Window(), (char*)resultINFO.c_str(), "WM_DESTROY", MB_OK);
        // MessageBoxA(INFO.Window(), (char*)resultHELP.c_str(), "WM_DESTROY", MB_OK);
        // MessageBoxA(INFO.Window(), (char*)resultIP.c_str(), "WM_DESTROY", MB_OK);
        // MessageBoxA(INFO.Window(), (char*)resultLOG.c_str(), "WM_DESTROY", MB_OK);
        // MessageBoxA(INFO.Window(), (char*)resultEXTRA.c_str(), "WM_DESTROY", MB_OK);
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
            if (MessageBoxW(m_hwnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK) {
                SendMessageA(MAIL.Window(), WM_CLOSE, 0, 0);
                SendMessageA(HELP.Window(), WM_CLOSE, 0, 0);
                SendMessageA(IP.Window(), WM_CLOSE, 0, 0);
                // SendMessageA(LOG.Window(), WM_CLOSE, 0, 0);
                // SendMessageA(EXTRA.Window(), WM_CLOSE, 0, 0);
                DestroyWindow(m_hwnd);

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
    setMode(signinInput[4] == "AUTOMATIC" ? AUTOMATIC : MANUAL);
    MAIL.setFilePath("AdminAccount");
    HELP.setFilePath("HELP.txt");
    // LOG.setFilePath("LOG.txt");
}

void UI::start() {
    if (!Create("Learn to Program Windows", 
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX, WS_EX_CLIENTEDGE)) {
        return;
    }

    MAIL.update_hwnd_parent(Window());
    HELP.update_hwnd_parent(Window());
    IP.update_hwnd_parent(Window());
    // LOG.update_hwnd_parent(Window());
    // EXTRA.update_hwnd_parent(Window());

    ShowWindow(Window(), SW_SHOWDEFAULT);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // isRunning = false;
    
    return;
}

void UI::Send(std::string msg) {
}

void UI::Send(std::string msg, std::string receiver) {
}

void UI::Receive(std::string msg) {
    std::cout << "String: " << msg;
}

void UI::Send(std::any *ptr) {
}

void UI::Send(std::any *ptr, std::string dest) {
}

void UI::Send(std::any *ptr, std::string type, std::string dest) {
}

void UI::Receive(std::any *ptr) {
}

void UI::Receive(std::any *ptr, std::string type) {
    std::osyncstream(std::cout) << "Receive ptr " << ptr << ' ' << type << '\n';
    if (type == "IP") {
        SendMessageA(m_hwnd, WM_FRONTEND_NOTIFY, IP_UPDATE_MESSAGE, (LPARAM)ptr);
    }
    else if (type == "INFO") {
        SendMessageA(m_hwnd, WM_FRONTEND_NOTIFY, INFO_UPDATE_MESSAGE, (LPARAM)ptr);
    }
    else if (type == "LOG") {
        std::string text = std::any_cast<std::string>(*ptr);
        std::osyncstream(std::cout) << "Append " << text << '\n';
        SendMessageA(m_hwnd, WM_FRONTEND_NOTIFY, LOG_UPDATE_MESSAGE, (LPARAM)ptr);
    }
}
