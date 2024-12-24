#include "text.h"

void TextWindow::setFilePath(std::string filePath) {
    this->filepath = filePath;
}

TextWindow::TextWindow(int WIDTH, int HEIGHT) : BaseWindow<TextWindow>(WIDTH, HEIGHT) {}

void TextWindow::appendTextToEdit(LPCSTR newText)
{
    int TextLen = SendMessageW(hEdit, WM_GETTEXTLENGTH, 0, 0);
    SendMessageW(hEdit, EM_SETSEL, (WPARAM)TextLen, (LPARAM)TextLen);
    SendMessageW(hEdit, EM_REPLACESEL, FALSE, (LPARAM)(newText));
}

void TextWindow::getNewMessage(std::string text) {
    std::ofstream logFile(filepath.c_str(), std::ios::app);
    logFile << text + "\n";
    if (hEdit != NULL) {
        appendTextToEdit("\r\n");
        appendTextToEdit(text.c_str());
    }
}

PCSTR TextWindow::ClassName() const { return "Text Window Class"; }

LRESULT TextWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_CREATE: {
        const char* fontName = "Consolas";
        const long nFontSize = 13;

        HDC hdc = GetDC(m_hwnd);
        LOGFONT logFont = {0};
        logFont.lfHeight = -MulDiv(nFontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
        logFont.lfWeight = FW_NORMAL;
        strcpy((char*)logFont.lfFaceName, fontName);

        // printf("%s", (char*)logFont.lfFaceName);

        s_hFont = CreateFontIndirect(&logFont);
        
        std::ifstream logFile(filepath.c_str()); 
        // std::osyncstream(std::cerr) << "filepath: " << filepath << '\n';
        std::string line;
        std::string logContent; 

        std::getline(logFile, line);
        logContent += line;
        while (std::getline(logFile, line)) { 
            logContent += "\r\n" + line; 
        }

        RECT clientRect;
        GetClientRect(m_hwnd, &clientRect);
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;

        // MessageBoxA(NULL, logContent.c_str(), "LOG", MB_OK);
        logFile.close(); 
        hEdit = CreateWindowExA(0, "EDIT", logContent.c_str(), 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, clientRect.left, clientRect.top, width, height, m_hwnd, (HMENU)1, GetModuleHandle(NULL), NULL ); 
        // HFONT hFont = (HFONT)GetStockObject(ANSI_FIXED_FONT); 
        SendMessage(hEdit, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
        ReleaseDC(m_hwnd, hdc);
    } break;

    case WM_TEXT_APPEND: {
        std::any* any_ptr = reinterpret_cast<std::any*>(lParam);
        std::string text = std::any_cast<std::string>(*any_ptr);
        // std::osyncstream(std::cout) << "Receive text: " << text << '\n';
        getNewMessage(text);
        delete any_ptr;
        break;
    }

    case WM_COPYDATA: {
        PCOPYDATASTRUCT pCDS = (PCOPYDATASTRUCT)lParam;
        std::string content((char*)pCDS->lpData);
        switch (pCDS->dwData) {
        case UPDATE_MESSAGE: {
            getNewMessage(content);
            break;
        }
        default:
            break;
        }
        break;
    }

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