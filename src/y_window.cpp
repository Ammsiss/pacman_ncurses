//external
#include <ncurses.h>

//user
#include "z_window.h"
#include "z_aggregate.h"
#include "z_obstacle.h"

//std
#include <vector>
#include <chrono>
#include <thread>
#include <string>
#include <fstream>

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

void Window::printLives(int lives)
{
    std::vector<std::string> livesList{};

    for(int i{0}; i < lives; ++i)
        livesList.emplace_back("<");

    wattron(m_window, COLOR_PAIR(Color::red_black));
    mvwprintw(m_window, 10, 1, "♥♥♥♥");
    mvwprintw(m_window, 11, 1, "    ");
    mvwprintw(m_window, 12, 1, "♥♥♥♥");
    wattroff(m_window, COLOR_PAIR(Color::red_black));

    wattron(m_window, COLOR_PAIR(Color::yellow_black));
    
    for(std::size_t i{0}; i < livesList.size(); ++i)
        mvwprintw(m_window, 11, (i + 1), "%s", livesList[i].c_str());

    wattroff(m_window, COLOR_PAIR(Color::yellow_black));

    wattron(m_window, COLOR_PAIR(Color::default_color));



    wrefresh(m_window);
}

void Window::printScoreOutline(int score)
{
    mvwprintw(m_window, 11, 23, "    ");
    mvwprintw(m_window, 10, 23, "    ");
    mvwprintw(m_window, 12, 23, "    ");

    wattron(m_window, COLOR_PAIR(Color::yellow_black));
    mvwprintw(m_window, 10, 23, "PNTS");
    mvwprintw(m_window, 12, 23, "%d", score);
    wattroff(m_window, COLOR_PAIR(Color::yellow_black));
    wattron(m_window, COLOR_PAIR(Color::default_color));

    // cyan
    wattron(m_window, COLOR_PAIR(Color::cyan_black));
    mvwprintw(m_window, 11, 23, "ᗣ");
    wattroff(m_window, COLOR_PAIR(Color::cyan_black));
    wattron(m_window, COLOR_PAIR(Color::default_color));

    // orange
    wattron(m_window, COLOR_PAIR(Color::orange_black));
    mvwprintw(m_window, 11, 24, "ᗣ");
    wattroff(m_window, COLOR_PAIR(Color::orange_black));
    wattron(m_window, COLOR_PAIR(Color::default_color));

    // red
    wattron(m_window, COLOR_PAIR(Color::red_black));
    mvwprintw(m_window, 11, 25, "ᗣ");
    wattroff(m_window, COLOR_PAIR(Color::red_black));
    wattron(m_window, COLOR_PAIR(Color::default_color));

    // pink
    wattron(m_window, COLOR_PAIR(Color::pink_black));
    mvwprintw(m_window, 11, 26, "ᗣ");
    wattroff(m_window, COLOR_PAIR(Color::pink_black));
    wattron(m_window, COLOR_PAIR(Color::default_color));
}

void Window::printHighScore()
{
    std::ifstream inputFile(m_highscore);
    int highscore{};
    inputFile >> highscore;

    wattron(m_window, COLOR_PAIR(Color::yellow_black));
    mvwprintw(m_window, 16, 23, "HgSc");
    mvwprintw(m_window, 18, 23, "    ");
    mvwprintw(m_window, 18, 23, "%d", highscore);
    wattroff(m_window, COLOR_PAIR(Color::yellow_black));
    wattron(m_window, COLOR_PAIR(Color::default_color));

    // cyan
    wattron(m_window, COLOR_PAIR(Color::cyan_black));
    mvwprintw(m_window, 17, 23, "ᗣ");
    wattroff(m_window, COLOR_PAIR(Color::cyan_black));
    wattron(m_window, COLOR_PAIR(Color::default_color));

    // orange
    wattron(m_window, COLOR_PAIR(Color::orange_black));
    mvwprintw(m_window, 17, 24, "ᗣ");
    wattroff(m_window, COLOR_PAIR(Color::orange_black));
    wattron(m_window, COLOR_PAIR(Color::default_color));

    // red
    wattron(m_window, COLOR_PAIR(Color::red_black));
    mvwprintw(m_window, 17, 25, "ᗣ");
    wattroff(m_window, COLOR_PAIR(Color::red_black));
    wattron(m_window, COLOR_PAIR(Color::default_color));

    // pink
    wattron(m_window, COLOR_PAIR(Color::pink_black));
    mvwprintw(m_window, 17, 26, "ᗣ");
    wattroff(m_window, COLOR_PAIR(Color::pink_black));
    wattron(m_window, COLOR_PAIR(Color::default_color));
}

void Window::setHighScore(int score)
{
    std::ifstream inputFile(m_highscore);
    int highscore{};
    inputFile >> highscore;

    if(score > highscore)
    {
    std::ofstream outputFile(m_highscore);
    outputFile << score;
    }
}

void Window::printGameOverMessage()
{
    wattron(m_window, COLOR_PAIR(Color::yellow_black));
    mvwprintw(m_window, 14, 11, "LOSER!");
    wattroff(m_window, COLOR_PAIR(Color::yellow_black));
    wattron(m_window, COLOR_PAIR(Color::default_color));

    wrefresh(m_window);

    char clickPastMessage{};
    while(true)
    {
        clickPastMessage = getch();

        if(clickPastMessage == '\n')
            break;
    }
}

void Window::flashingObstacles()
{
    auto interval{ 300ms };
    auto lastTime{ std::chrono::high_resolution_clock::now() };

    int colorChange{0};

    while(true)
    {
        auto currentTime{ std::chrono::high_resolution_clock::now() };

        if(currentTime - lastTime >= interval)
        {
            for(std::size_t y{0}; y < m_windowArea.size(); ++y)
            {
                for(std::size_t x{0}; x < m_windowArea[y].size(); ++x)
                {
                    if(colorChange % 2 == 0 && (m_windowArea[y][x] == CellName::obstacleValue || m_windowArea[y][x] == CellName::perimeterValue))
                    {
                        wattron(m_window, COLOR_PAIR(Color::white_black));
                        mvwaddch(m_window, y, x, ACS_CKBOARD);
                        wattroff(m_window, COLOR_PAIR(Color::white_black));
                        wattron(m_window, COLOR_PAIR(Color::default_color));

                        wrefresh(m_window);
                    }
                    else if(colorChange % 2 != 0 && (m_windowArea[y][x] == CellName::obstacleValue || m_windowArea[y][x] == CellName::perimeterValue))
                    {
                        wattron(m_window, COLOR_PAIR(Color::blue_black));
                        mvwaddch(m_window, y, x, ACS_CKBOARD);
                        wattroff(m_window, COLOR_PAIR(Color::blue_black));
                        wattron(m_window, COLOR_PAIR(Color::default_color));

                        wrefresh(m_window);
                    }
                }
            }

            if(colorChange == 5)
                break;

            ++colorChange;

            lastTime = currentTime;
        }

        std::this_thread::sleep_for(5ms);
    }
}
                                                                                                                                                                 
// hardcoded assignment
void Window::assignGhostBox()
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