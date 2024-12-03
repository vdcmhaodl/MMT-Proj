#include "UI.h"

void MainWindow::setMode(int mode) {
    this->mode = mode;
}

MainWindow::MainWindow(int WIDTH, int HEIGHT, int mode) : BaseWindow<MainWindow>(WIDTH, HEIGHT),
                                                          INFO(640, 360),
                                                          HELP(640, 360),
                                                          IP(640, 360),
                                                          LOG(640, 360),
                                                          EXTRA(640, 360, mode),
                                                          mode(mode) {}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE: {
        hwndButtonInfo = CreateWindowW(L"BUTTON", L"INFO", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                50, 50, 100, 30, m_hwnd, (HMENU)INFO_BUTTON, NULL, NULL);
        hwndButtonHelp = CreateWindowW(L"BUTTON", L"HELP", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                200, 50, 100, 30, m_hwnd, (HMENU)HELP_BUTTON, NULL, NULL);
        hwndButtonIP = CreateWindowW(L"BUTTON", L"IP LIST", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                350, 50, 100, 30, m_hwnd, (HMENU)IP_BUTTON, NULL, NULL);
        hwndButtonLog = CreateWindowW(L"BUTTON", L"LOG", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                500, 50, 100, 30, m_hwnd, (HMENU)LOG_BUTTON, NULL, NULL);
        hwndButtonExtra = CreateWindowW(L"BUTTON", L"EXTRA", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                650, 50, 100, 30, m_hwnd, (HMENU)EXTRA_BUTTON, NULL, NULL);
        break;
    }
    
    case WM_COMMAND: {
        int identifier = LOWORD(wParam);
        switch (identifier) {
        case INFO_BUTTON:
            // MessageBoxW(m_hwnd, L"Info button!", L"Child Window", MB_OK);
            if (INFO.Window() == NULL) {
                INFO.Create("Client infomation", WS_OVERLAPPEDWINDOW, WS_EX_CLIENTEDGE);
                ShowWindow(INFO.Window(), SW_SHOWDEFAULT);
                UpdateWindow(INFO.Window());
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

        case LOG_BUTTON:
            // MessageBoxW(m_hwnd, L"Log button!", L"Child Window", MB_OK);
            if (LOG.Window() == NULL) {
                LOG.Create("Log", WS_OVERLAPPEDWINDOW, WS_EX_CLIENTEDGE);
                ShowWindow(LOG.Window(), SW_SHOWDEFAULT);
                UpdateWindow(LOG.Window());
            }
            break;
        
        case EXTRA_BUTTON:
            // MessageBoxW(m_hwnd, L"Log button!", L"Child Window", MB_OK);
            if (EXTRA.Window() == NULL) {
                // MessageBoxW(m_hwnd, L"Extra button clicked!", L"Test", MB_OK);
                EXTRA.Create("Extra", WS_OVERLAPPEDWINDOW, WS_EX_CLIENTEDGE);
                ShowWindow(EXTRA.Window(), SW_SHOWDEFAULT);
                UpdateWindow(EXTRA.Window());
            }
            break;
        
        default:
            break;
        }
    } break;

    case WM_COPYDATA: {
        PCOPYDATASTRUCT pCDS = (PCOPYDATASTRUCT)lParam;
        switch (pCDS->dwData)
        {
        case EXTRA_MESSAGE:{
            std::string content((char*)pCDS->lpData);
            std::string message = content + std::string("; RECEIVED TEXT!");
            MessageBoxA(m_hwnd, (char*)message.c_str(), "Test WM_COPYDATA", MB_OK);
            break;
        }
        default:
            break;
        }
        break;
    }

    case WM_DESTROY:{
        std::string resultINFO = (INFO.Window() == NULL ? "SUCCESS" : "FAIL");
        std::string resultHELP = (HELP.Window() == NULL ? "SUCCESS" : "FAIL");
        std::string resultIP = (IP.Window() == NULL ? "SUCCESS" : "FAIL");
        std::string resultLOG = (LOG.Window() == NULL ? "SUCCESS" : "FAIL");
        std::string resultEXTRA = (EXTRA.Window() == NULL ? "SUCCESS" : "FAIL");
        MessageBoxA(INFO.Window(), (char*)resultINFO.c_str(), "WM_DESTROY", MB_OK);
        MessageBoxA(INFO.Window(), (char*)resultHELP.c_str(), "WM_DESTROY", MB_OK);
        MessageBoxA(INFO.Window(), (char*)resultIP.c_str(), "WM_DESTROY", MB_OK);
        MessageBoxA(INFO.Window(), (char*)resultLOG.c_str(), "WM_DESTROY", MB_OK);
        MessageBoxA(INFO.Window(), (char*)resultEXTRA.c_str(), "WM_DESTROY", MB_OK);
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
                SendMessageA(INFO.Window(), WM_CLOSE, 0, 0);
                SendMessageA(HELP.Window(), WM_CLOSE, 0, 0);
                SendMessageA(IP.Window(), WM_CLOSE, 0, 0);
                SendMessageA(LOG.Window(), WM_CLOSE, 0, 0);
                SendMessageA(EXTRA.Window(), WM_CLOSE, 0, 0);
                DestroyWindow(m_hwnd);
            }
            // Else: User canceled. Do nothing.
            return 0;
        }

    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return TRUE;
}