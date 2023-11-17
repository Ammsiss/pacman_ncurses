#include <ncurses.h>

#include <vector>

#include "z_window.h"
#include "z_aggregate.h"

// private

const int Window::defaultGameX{28};
const int Window::defaultGameY{31};

void Window::drawBoxAndRefresh()
{
    wborder(m_window, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD);
    wrefresh(m_window);
}

void Window::initWindowPerimeter()
{
    // X: 0 - 27
    // Y: 0 - 30

    for (int top{0}; top <= (m_screenX - 1); ++top)
    {
        m_windowPerimeter.emplace_back(Vec{0, top});
    }

    for (int bottom{0}; bottom <= (m_screenX - 1); ++bottom)
    {
        m_windowPerimeter.emplace_back(Vec{(m_screenY - 1), bottom});
    }

    for (int left{1}; left <= (m_screenY - 2); ++left)
    {
        m_windowPerimeter.emplace_back(Vec{left, 0});
    }

    for (int right{1}; right <= (m_screenY - 2); ++right)
    {
        m_windowPerimeter.emplace_back(Vec{right, m_screenX - 1});
    }
}

// public

Window::Window(int screenY, int screenX)
: m_screenY {screenY}
, m_screenX {screenX}
{
    int maxY{};
    int maxX{};
    getmaxyx(stdscr, maxY, maxX);
    m_window = newwin(m_screenY, m_screenX, (maxY - m_screenY) / 2, (maxX - m_screenX) / 2);
    drawBoxAndRefresh();
    initWindowPerimeter();
}


Window::Window(int screenY, int screenX, int startY, int startX)
: m_screenY {screenY}
, m_screenX {screenX}
{
    m_window = newwin(screenY, screenX, startY, startX);
    drawBoxAndRefresh();
    initWindowPerimeter();
}


Window::~Window() { delwin(m_window); }

WINDOW* Window::getWindow() { return m_window; }
const int& Window::getScreenY() { return m_screenY; }
const int& Window::getScreenX() { return m_screenX; }
std::vector<Vec> Window::getWindowPerimeter() { return m_windowPerimeter; }
