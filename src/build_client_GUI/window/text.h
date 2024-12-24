#pragma once

#include "base_window.h"

#define WM_TEXT_APPEND (WM_USER + 4)

class TextWindow : public BaseWindow<TextWindow>
{
private:
    std::string filepath;
    HWND hEdit = NULL;
    static const DWORD UPDATE_MESSAGE = 1;
    HFONT s_hFont = NULL;
public:
    void setFilePath(std::string filePath);
    TextWindow(int WIDTH = 640, int HEIGHT = 360);
    void appendTextToEdit(LPCSTR newText);
    void getNewMessage(std::string text);
    PCSTR  ClassName() const;
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};