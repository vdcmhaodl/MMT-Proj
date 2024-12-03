#pragma once

#include <windows.h>
#include <shobjidl.h> 
#include <string>
#include <vector>

#define AUTOMATIC 0
#define MANUAL 1

template <class DERIVED_TYPE> 
class BaseWindow
{
public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        DERIVED_TYPE *pThis = NULL;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

            pThis->m_hwnd = hwnd;
        }
        else
        {
            pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }
        if (pThis)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    BaseWindow(int WIDTH = CW_USEDEFAULT, int HEIGHT = CW_USEDEFAULT, HWND m_hwnd_parent = NULL) : 
        m_hwnd(NULL), WIDTH(WIDTH), HEIGHT(HEIGHT), m_hwnd_parent(m_hwnd_parent) { }

    BOOL Create(
        PCSTR lpWindowName,
        DWORD dwStyle,
        DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        HMENU hMenu = 0
        )
    {
        WNDCLASS wc = {0};

        wc.lpfnWndProc   = DERIVED_TYPE::WindowProc;
        wc.hInstance     = GetModuleHandle(NULL);
        wc.lpszClassName = ClassName();

        RegisterClass(&wc);

        m_hwnd = CreateWindowExA(
            dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
            WIDTH, HEIGHT, m_hwnd_parent, hMenu, GetModuleHandle(NULL), this
            );

        return (m_hwnd ? TRUE : FALSE);
    }

    HWND Window() const { return m_hwnd; }

    void update_hwnd_parent(HWND hWndParent) {
        m_hwnd_parent = hWndParent;
    }

protected:

    virtual PCSTR  ClassName() const = 0;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
    
    HWND m_hwnd;
    int WIDTH;
    int HEIGHT;
    HWND m_hwnd_parent;
};



