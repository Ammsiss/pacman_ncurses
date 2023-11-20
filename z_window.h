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
    int m_perimeterValue{10001};
    
    WINDOW* m_window;

    std::vector<Vec> m_windowPerimeter{};
    std::vector<std::vector<int>> m_windowArea{};

    void initWindowPerimeter();
    void initWindowArea();
    void assignPelletEatenToGhostBox();
    void removeGhostBoxPelletAndAssignEaten();

public:
    Window(int screenY = defaultGameY, int screenX = defaultGameX);
    Window(int screenY, int screenX, int startY, int startX);
    ~Window();

    // getters
    WINDOW* getWindow();
    int& getScreenY(); 
    int& getScreenX();
    std::vector<Vec>& getWindowPerimeter();
    std::vector<std::vector<int>>& getWindowArea();

    void drawBoxAndRefresh();
    void gameCountDown();
};