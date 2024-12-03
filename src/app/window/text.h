#pragma once

#include "base_window.h"
#include <fstream>

class TextWindow : public BaseWindow<TextWindow>
{
private:
    std::string filepath;
    HWND hEdit = NULL;
public:
    TextWindow(int WIDTH = 640, int HEIGHT = 360);
    void appendTextToEdit(LPCSTR newText);
    void getNewMessage(std::string text);
    PCSTR  ClassName() const;
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};