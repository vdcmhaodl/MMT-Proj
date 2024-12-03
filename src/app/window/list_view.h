#pragma once

#include "base_window.h"
#include <map>
#include <string>

using ListViewData = std::map<std::string, std::pair<std::string, std::string>>;

class ListViewWindow : public BaseWindow<ListViewWindow>
{
private:
    HWND hWndListView = NULL;
    ListViewData list_IP_addr;
public:
    ListViewWindow(int WIDTH = 640, int HEIGHT = 360);
    void InitListView();
    void UpdateListView();
    PCSTR  ClassName() const;
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};