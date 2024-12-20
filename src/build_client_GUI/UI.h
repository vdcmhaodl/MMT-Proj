#pragma once

#include "../socket/socket.h"
#include "window/base_window.h"
#include "window/input.h"
#include "window/list_view.h"
#include "window/text.h"
#include "window/extra.h"
#include "mediator.h"
#include <any>

#define WM_FRONTEND_NOTIFY (WM_USER + 1)

class UI : public BaseWindow<UI>, public Participant
{
public:
    static const int MAIL_BUTTON = 1;
    static const int HELP_BUTTON = 2;
    static const int IP_BUTTON = 3;
    static const int ABOUT_BUTTON = 4;
    static const int IP_UPDATE_MESSAGE = 6;
    static const int LOG_UPDATE_MESSAGE = 7;
    static const int INFO_UPDATE_MESSAGE = 8;

    static const DWORD INFO_MESSAGE = 1;
    static const DWORD IP_MESSAGE = 2;
    static const DWORD LOG_MESSAGE = 3;
    static const DWORD EXTRA_MESSAGE = 4;
    HWND hwndButtonMail, hwndButtonHelp, hwndButtonIP, hwndButtonLog, hwndButtonExtra, hwndButtonAbout;

    TextWindow MAIL;
    TextWindow HELP;
    TextWindow ABOUT;
    ListViewWindow IP;
    // TextWindow LOG;

    HWND hEdit;
    HWND hInfo;

    HFONT s_hFont;

    int mode;
    void setMode(int mode);
    UI(int WIDTH, int HEIGHT, int mode, Mediator* mediator, std::string name);
    PCSTR  ClassName() const { return "Client Window Class"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void initialize(std::vector<std::string> &signinInput);
    void start();

    void Send(std::string msg);
    void Send(std::string msg, std::string receiver);
    void Receive(std::string msg);

    void Send(std::any *ptr);
    void Send(std::any *ptr, std::string dest);
    void Send(std::any *ptr, std::string type, std::string dest);
    void Receive(std::any *ptr);
    void Receive(std::any *ptr, std::string type);
};