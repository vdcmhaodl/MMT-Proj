#pragma once

#include "base_window.h"
#include <commctrl.h>
#include <map>
#include <string>
#include <any>
#include <syncstream>
#include <iostream>

using ListViewData = std::map<std::string, std::pair<std::string, std::string>>;

#define WM_IP_NOTIFY (WM_USER + 2)
#define WM_IP_REQUEST (WM_USER + 3)

class ListViewWindow : public BaseWindow<ListViewWindow>
{
private:
    HWND hWndListView = NULL;
    ListViewData list_IP_addr;
    
public:
    static const int UPDATE_LIST_VIEW = 5;
    ListViewWindow(int WIDTH = 640, int HEIGHT = 360);
    void InitListView();
    void UpdateListView();
    PCSTR  ClassName() const;
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};