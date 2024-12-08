#pragma once

#include "../socket/socket.h"
#include "window/base_window.h"
#include "window/input.h"
#include "window/list_view.h"
#include "window/text.h"
#include "window/extra.h"

class MainWindow : public BaseWindow<MainWindow>
{
public:
    static const int INFO_BUTTON = 1;
    static const int HELP_BUTTON = 2;
    static const int IP_BUTTON = 3;
    static const int LOG_BUTTON = 4;
    static const int EXTRA_BUTTON = 5;
    static const int IP_UPDATE_MESSAGE = 6;

    static const DWORD INFO_MESSAGE = 1;
    static const DWORD IP_MESSAGE = 2;
    static const DWORD LOG_MESSAGE = 3;
    static const DWORD EXTRA_MESSAGE = 4;
    HWND hwndButtonInfo, hwndButtonHelp, hwndButtonIP, hwndButtonLog, hwndButtonExtra;

    TextWindow INFO;
    TextWindow HELP;
    ListViewWindow IP;
    TextWindow LOG;
    ExtraWindow EXTRA;

    int mode;
    void setMode(int mode);
    MainWindow(int WIDTH = CW_USEDEFAULT, int HEIGHT = CW_USEDEFAULT, int mode = AUTOMATIC);
    PCSTR  ClassName() const { return "Client Window Class"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

extern concurrent_queue<std::string> queueExtra;