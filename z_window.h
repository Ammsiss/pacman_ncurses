#pragma once

#include <ncurses.h>

class Window
{
private:
    static const int defaultGameX;
    static const int defaultGameY;
    int m_screenY;
    int m_screenX;
    WINDOW* m_window;

    void drawBoxAndRefresh();

public:
    Window(int screenY = defaultGameY, int screenX = defaultGameX);
    Window(int screenY, int screenX, int startY, int startX);
    ~Window();

    // getters
    WINDOW* getWindow();
    const int& getScreenY(); 
    const int& getScreenX();
};