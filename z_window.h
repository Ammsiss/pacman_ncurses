#pragma once

#include "z_aggregate.h"

#include <vector>
#include <ncurses.h>

class Window
{
private:
    static const int defaultGameX;
    static const int defaultGameY;
    int m_screenY;
    int m_screenX;
    WINDOW* m_window;
    std::vector<Vec> m_windowPerimeter{};
    std::vector<std::vector<int>> m_windowArea{};

    void drawBoxAndRefresh();
    void initWindowPerimeter();
    void initWindowArea();

public:
    Window(int screenY = defaultGameY, int screenX = defaultGameX);
    Window(int screenY, int screenX, int startY, int startX);
    ~Window();

    // getters
    WINDOW* getWindow();
    const int& getScreenY(); 
    const int& getScreenX();
    std::vector<Vec> getWindowPerimeter();
    std::vector<std::vector<int>> getWindowArea();
    void gameCountDown();
};