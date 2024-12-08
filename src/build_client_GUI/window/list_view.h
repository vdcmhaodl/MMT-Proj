#pragma once

#include "base_window.h"
#include <commctrl.h>
#include <map>
#include <string>

using ListViewData = std::map<std::string, std::pair<std::string, std::string>>;
extern ListViewData list_IP_addr;

class ListViewWindow : public BaseWindow<ListViewWindow>
{
private:
    HWND hWndListView = NULL;
    // ListViewData list_IP_addr;
    
public:
    static const int UPDATE_LIST_VIEW = 5;
    ListViewWindow(int WIDTH = 640, int HEIGHT = 360);
    void InitListView();
    void UpdateListView();
    PCSTR  ClassName() const;
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};