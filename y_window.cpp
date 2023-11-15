#include <ncurses.h>

#include "z_window.h"

// private

const int Window::defaultGameX{28};
const int Window::defaultGameY{31};

void Window::drawBoxAndRefresh()
{
    wborder(m_window, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD);
    wrefresh(m_window);
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
}


Window::Window(int screenY, int screenX, int startY, int startX)
: m_screenY {screenY}
, m_screenX {screenX}
{
    m_window = newwin(screenY, screenX, startY, startX);
    drawBoxAndRefresh();
}


Window::~Window() { delwin(m_window); }

WINDOW* Window::getWindow() { return m_window; }
const int& Window::getScreenY() { return m_screenY; }
const int& Window::getScreenX() { return m_screenX; }
