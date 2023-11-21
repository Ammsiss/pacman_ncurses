#pragma once

// external
#include <ncurses.h>

// user
#include "z_aggregate.h"

// std
#include <vector>

class Window
{
public:
    // constructors
    Window(int screenY = defaultGameY, int screenX = defaultGameX);
    Window(int screenY, int screenX, int startY, int startX);
    ~Window();

    // public methods
    void drawBoxAndRefresh();
    void gameCountDown();
    void assignGhostBox();
    void printLives(int lives);
    void printScoreOutline(int score);

    // getters
    WINDOW* getWindow();
    int& getScreenY(); 
    int& getScreenX();
    std::vector<Vec>& getWindowPerimeter();
    std::vector<std::vector<int>>& getWindowArea();

private:

    // private variables
    static const int defaultGameX;
    static const int defaultGameY;
    int m_screenY;
    int m_screenX;
    int m_perimeterValue{10001};
    WINDOW* m_window;
    // to assign for vector space
    std::vector<Vec> m_windowPerimeter{};
    // main vector space
    std::vector<std::vector<int>> m_windowArea{};

    // private methods
    void initWindowPerimeter();
    void initWindowAreaSize();
};