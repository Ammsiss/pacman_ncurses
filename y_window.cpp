//external
#include <ncurses.h>

//user
#include <vector>
#include <chrono>
#include <thread>

//std
#include "z_window.h"
#include "z_aggregate.h"
#include "z_obstacle.h"

using namespace std::chrono_literals;

/********************************************************************** PUBLIC MEMBERS **********************************************************************/

// constructs centered window with optional dimensions
Window::Window(int screenY, int screenX)
: m_screenY {screenY}
, m_screenX {screenX}
{
    int maxY{};
    int maxX{};
    getmaxyx(stdscr, maxY, maxX);
    m_window = newwin(m_screenY, m_screenX, (maxY - m_screenY) / 2, (maxX - m_screenX) / 2);
    initWindowPerimeter();
    initWindowAreaSize();
}

// Constructs custum window with custum start point and dimensions based on stdscr
Window::Window(int screenY, int screenX, int startY, int startX)
: m_screenY {screenY}
, m_screenX {screenX}
{
    m_window = newwin(screenY, screenX, startY, startX);
    initWindowPerimeter();
    initWindowAreaSize();
}

// deconstructor
Window::~Window() { delwin(m_window); }

// -------------v
void Window::drawBoxAndRefresh()
{
    wattron(m_window, COLOR_PAIR(Color::blue_black));   
    wborder(m_window, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD);
    wattroff(m_window, COLOR_PAIR(Color::blue_black));
    wattron(m_window, COLOR_PAIR(Color::default_color));
    wrefresh(m_window);
}

// Count down for main game window
void Window::gameCountDown()
{
    int count{3};

    auto interval{1000ms};
    auto lastTime{std::chrono::high_resolution_clock::now()};

    wattron(m_window, COLOR_PAIR(Color::yellow_black));  

    while(true)
    {
        auto currentTime{std::chrono::high_resolution_clock::now()};

        if(currentTime - lastTime >= interval)
        {
            if(count == 3)
            {
                mvwprintw(m_window, 14, 11, "3");
                wrefresh(m_window);
            }

            if(count == 2)
            {
                mvwprintw(m_window, 14, 12, "2");
                wrefresh(m_window);
            }

            if(count == 1)
            {
                mvwprintw(m_window, 14, 13, "1");
                wrefresh(m_window);
            }

            if(count == 0)
            {
                mvwprintw(m_window, 14, 14, "GO!");
                wrefresh(m_window);
            }

            lastTime = currentTime;
            --count;

            if(count == -2)
            {
                mvwprintw(m_window, 14, 11, "      ");
                wrefresh(m_window);
                wattroff(m_window, COLOR_PAIR(Color::yellow_black));
                wattron(m_window, COLOR_PAIR(Color::default_color));
                break;
            }
        }

        std::this_thread::sleep_for(5ms);
    }
}
                                                                                                                                                                 
// hardcoded assignment
void Window::assignGhostBox(Window& win)
{
    // HARDCODED CHANGE MAYBE
    
    for(int y{13}; y < 16; ++y)
    {
        for(int x{11}; x < 17; ++x)
        {
            m_windowArea[y][x] = CellName::ghostBox;
        }
    }

    m_windowArea[12][13] = CellName::ghostBox;
    m_windowArea[12][14] = CellName::ghostBox;
}

// getters
WINDOW* Window::getWindow() { return m_window; }
int& Window::getScreenY() { return m_screenY; }
int& Window::getScreenX() { return m_screenX; }
std::vector<Vec>& Window::getWindowPerimeter() { return m_windowPerimeter; }
std::vector<std::vector<int>>& Window::getWindowArea() { return m_windowArea; }

/********************************************************************** PRIVATE MEMBERS **********************************************************************/

// constant default window dimensions
const int Window::defaultGameX{28};
const int Window::defaultGameY{31};

// ------------------v
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

// -----------------v
void Window::initWindowAreaSize()
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