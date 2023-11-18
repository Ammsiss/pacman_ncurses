#include <ncurses.h>

#include <vector>

#include "z_window.h"
#include "z_aggregate.h"

// private

const int Window::defaultGameX{28};
const int Window::defaultGameY{31};

void Window::drawBoxAndRefresh()
{
    wattron(m_window, COLOR_PAIR(6));   
    wborder(m_window, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD);
    wattroff(m_window, COLOR_PAIR(6));
    wattron(m_window, COLOR_PAIR(0));
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

void Window::initWindowArea()
{
    for(std::size_t rows{0}; rows < m_screenY; ++rows)
    {
        m_windowArea.emplace_back();

        for(std::size_t columns{0}; columns < m_screenX; ++columns)
        {
            m_windowArea[rows].emplace_back();
        }
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
    initWindowArea();
}


Window::Window(int screenY, int screenX, int startY, int startX)
: m_screenY {screenY}
, m_screenX {screenX}
{
    m_window = newwin(screenY, screenX, startY, startX);
    drawBoxAndRefresh();
    initWindowPerimeter();
    initWindowArea();
}


Window::~Window() { delwin(m_window); }

WINDOW* Window::getWindow() { return m_window; }
const int& Window::getScreenY() { return m_screenY; }
const int& Window::getScreenX() { return m_screenX; }
std::vector<Vec> Window::getWindowPerimeter() { return m_windowPerimeter; }
std::vector<std::vector<int>> Window::getWindowArea() { return m_windowArea; }
