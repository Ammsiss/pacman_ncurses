//external
#include <ncurses.h>

//user
#include "z_blinky.h"
#include "z_window.h"
#include "z_aggregate.h"
#include "z_obstacle.h"
#include "z_random.h"
#include "z_pacman.h"
#include "z_pinky.h"
#include "z_inky.h"
#include "z_ghost.h"

//std
#include <vector>
#include <chrono>
#include <thread>
#include <queue>
#include <algorithm>

using namespace std::chrono_literals;

// public method

bool Pinky::timeToMove(Window& win, Pacman& pacman, Inky& inky, Blinky& blinky, Ghost& clyde, bool powerPelletTimer, LevelState ateWhichGhost, int& score, std::chrono::time_point<std::chrono::high_resolution_clock>& lastTime, std::chrono::milliseconds& interval)
{
    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto currentTime{std::chrono::high_resolution_clock::now()};

    // if power pellet not active
    if(!powerPelletTimer)
    {
        if (currentTime - m_lastTime >= m_pinkyInterval)
        {
            eraseLastPosition(win);
            checkForAndPrintOverLaps(win, inky, blinky, clyde, powerPelletTimer);

            if(!setDirection(win, pacman, blinky, false))
                return false;

            printAndRefreshGhost(win, false);

            m_lastTime = currentTime;
        }

        return true;
    }
    else // if power pellet is active
    {
        ghostFlashing(win, lastTime, interval, currentTime);

        m_pinkyIntervalStorage = m_pinkyInterval;
        m_pinkyInterval = 350ms;

        if(ateWhichGhost == LevelState::atePinky)
        {
            m_pinkyVec.y = 15;
            m_pinkyVec.x = 15;
        }

        if(currentTime - m_lastTime >= m_pinkyInterval)
        {
            eraseLastPosition(win);
            checkForAndPrintOverLaps(win, inky, blinky, clyde, powerPelletTimer);
            
           if(!setDirection(win, pacman, blinky, true))
            {
                wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));
                mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "+");
                wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
                wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
                wrefresh(win.getWindow());
                lastTime += 1100ms;
                std::this_thread::sleep_for(1s);

                score += 20;
                m_pinkyVec.y = 15;
                m_pinkyVec.x = 15;
            }

            printAndRefreshGhost(win, true);

            m_lastTime = currentTime;
        }

        m_pinkyInterval = m_pinkyIntervalStorage;

        return true;
    }
}

void Pinky::printGhost(Window& win)
{
    wattron(win.getWindow(), COLOR_PAIR(Color::pink_black));
    mvwprintw(win.getWindow(), 15, 15, "ᗣ");
    wattron(win.getWindow(), COLOR_PAIR(Color::pink_black));
    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
}

Vec Pinky::getPinkyVec() { return m_pinkyVec; }

Color::ColorPair Pinky::getPinkyColor() { return m_pinkyColor; }

void Pinky::setSpeed() { m_pinkyInterval -= 5ms; }

void Pinky::setGhostVec() { m_pinkyVec = Vec{15, 15}; }

/********************************************************************** PRIVATE MEMBERS **********************************************************************/

/* _   _   _   _   _   _   _     _   _   _   _   _   _   _   _   _  
  / \ / \ / \ / \ / \ / \ / \   / \ / \ / \ / \ / \ / \ / \ / \ / \   Functions related to checking ghost's location and either using it for printing or
 ( S | E | T | T | I | N | G ) ( D | I | R | E | C | T | I | O | N )  reverting it and trying again until a valid input is found.
  \_/ \_/ \_/ \_/ \_/ \_/ \_/   \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/   
*/

bool Pinky::setDirection(Window& win, Pacman& pacman, Blinky& blinky, bool powerPelletActive)  
{
    std::vector<Vec> ghostPath{};

    if(!powerPelletActive)
    {
        if(m_pinkyVec.y != blinky.getBlinkyVec().y || m_pinkyVec.x != blinky.getBlinkyVec().x)
        {
            ghostPath = createGhostPath(Vec{m_pinkyVec.y, m_pinkyVec.x}, win, Vec{blinky.getBlinkyVec().y, blinky.getBlinkyVec().x});
            m_pinkyVec.y = ghostPath[1].y;
            m_pinkyVec.x = ghostPath[1].x;
        }
        else
        {
            ghostPath = createGhostPath(Vec{m_pinkyVec.y, m_pinkyVec.x}, win, Vec{1, 1});
            m_pinkyVec.y = ghostPath[1].y;
            m_pinkyVec.x = ghostPath[1].x;
        }
    }
    else
    {
        if(m_pinkyVec.y != 22 || m_pinkyVec.x != 26)
        {
            ghostPath = createGhostPath(Vec{m_pinkyVec.y, m_pinkyVec.x}, win, Vec{22, 26});
            m_pinkyVec.y = ghostPath[1].y;
            m_pinkyVec.x = ghostPath[1].x;
        }
        else
        {
            ghostPath = createGhostPath(Vec{m_pinkyVec.y, m_pinkyVec.x}, win, Vec{20, 26});
            m_pinkyVec.y = ghostPath[1].y;
            m_pinkyVec.x = ghostPath[1].x;
        }
    }


    if(m_pinkyVec.y == pacman.getPacVec().y && m_pinkyVec.x == pacman.getPacVec().x)
        return false;
    else
        return true;
}

std::vector<Vec> Pinky::createGhostPath(Vec start, Window& win, Vec target)
{
    auto rows{win.getWindowArea().size()};
    auto cols{win.getWindowArea()[0].size()};

    std::vector<std::vector<int>> visited(rows, std::vector<int>(cols, 0));
    std::vector<std::vector<Vec>> parentCell(rows, std::vector<Vec>(cols));

    std::queue<Vec> q{};
    q.push(start);                  // push in start point to q
    visited[start.y][start.x] = 1;  // assign start point visited

    while(!q.empty())
    {
        Vec current{ q.front() };  // assigns new variable "current" to element first out.
        q.pop();                          // removes element at front of q

        // Directions: up, down, left, right
        std::vector<Vec> directions = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};

        for(const auto& dir : directions)
        {
            Vec next{current.y + dir.y, current.x + dir.x};

            //if(next.y >= 0 && next.y < rows && next.x >= 0 && next.x < cols && !visited[next.y][next.x]) 
            if(next.y > 0 && next.y < (rows - 1) && next.x > 0 && next.x < (cols - 1) && win[{next.y, next.x}] != CellName::obstacleValue && 
                win[{next.y, next.x}] != CellName::perimeterValue && !visited[next.y][next.x])
            {
                q.push(next);
                visited[next.y][next.x] = 1;
                parentCell[next.y][next.x] = current;
            }
        }
    }

    std::vector<Vec> path{};
    Vec current{target};
    while(current.y != start.y || current.x != start.x)
    {
        path.push_back(current);
        current = parentCell[current.y][current.x];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    
    return path;
}

/* _   _   _   _   _   _   _   _  
  / \ / \ / \ / \ / \ / \ / \ / \  Functions related to erasing old positions, and printing new positions. Also deals with ghost 
 ( P | R | I | N | T | I | N | G ) overlaps which cause print overs which is where a ghost will overlap with another and when it moves it will 
  \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/  erase the other ghost causing the ghost to dissapear. Also deals with spawning pellets back after a ghost clears them                              
*/

void Pinky::eraseLastPosition(Window& win)
{
    mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, " ");
}

// deals with overlaps (pellets && ghost)
void Pinky::checkForAndPrintOverLaps(Window& win, Inky& inky, Blinky& blinky, Ghost& clyde, bool powerPelletActive)
{
    printPelletBackIfNotEaten(win);
    printOverLap(win, checkGhostOverLap(inky, blinky, clyde), powerPelletActive);
}

void Pinky::printPelletBackIfNotEaten(Window& win)
{
    if(win[{m_pinkyVec.y, m_pinkyVec.x}] != CellName::pelletEaten && win[{m_pinkyVec.y, m_pinkyVec.x}] != CellName::ghostBox &&
        win[{m_pinkyVec.y, m_pinkyVec.x}] != CellName::perimeterValue && win[{m_pinkyVec.y, m_pinkyVec.x}] != CellName::powerPelletEaten)
    {
        if(win[{m_pinkyVec.y, m_pinkyVec.x}] == CellName::powerPellet)
        {
            wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));
            wattron(win.getWindow(), A_BLINK);
            mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "⬤");
            wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
            wattroff(win.getWindow(), A_BLINK);
            wattron(win.getWindow(), COLOR_PAIR(Color::default_color));  
        }
        else
        {
        wattron(win.getWindow(), COLOR_PAIR(Color::white_black));
        mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "•");
        wattroff(win.getWindow(), COLOR_PAIR(Color::white_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));    
        }
    }
}

Color::ColorPair Pinky::checkGhostOverLap(Inky& inky, Blinky& blinky, Ghost& clyde)
{
    if(m_pinkyVec.y == inky.getInkyVec().y && m_pinkyVec.x == inky.getInkyVec().x)
        return inky.getInkyColor();

    if(m_pinkyVec.y == blinky.getBlinkyVec().y && m_pinkyVec.x == blinky.getBlinkyVec().x)
        return blinky.getBlinkyColor();
    
    if(m_pinkyVec.y == clyde.getGhostVec().y && m_pinkyVec.x == clyde.getGhostVec().x)
        return clyde.getGhostColor();

    return Color::null;
}

void Pinky::printOverLap(Window& win, Color::ColorPair overLapColor, bool powerPelletActive)
{
    // prints ghost back if overlapped
    if(powerPelletActive && overLapColor != Color::null)
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::blue_black));
        mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(Color::blue_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color)); 
    }
    else if(overLapColor != Color::null)
    {
        wattron(win.getWindow(), COLOR_PAIR(overLapColor));
        mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(overLapColor));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
    }
}

void Pinky::printAndRefreshGhost(Window& win, bool powerPelletActive)
{
    if(!powerPelletActive)
    {
        wattron(win.getWindow(), COLOR_PAIR(m_pinkyColor));
        mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(m_pinkyColor));
        wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));
    }
    else
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::blue_black));
        mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(Color::blue_black));
        wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));
    }

    wrefresh(win.getWindow());
}

// Every 250 ms in the last 1.5 seconds of the power pellet change ghost color to indicate time is almost out
void Pinky::ghostFlashing(Window& win, std::chrono::time_point<std::chrono::high_resolution_clock>& lastTime, std::chrono::milliseconds& interval, std::chrono::time_point<std::chrono::high_resolution_clock>& currentTime)
{
    auto elapsedTime{ currentTime - lastTime };
    auto remainingTime{ interval - elapsedTime };

    Color::ColorPair color{};
    std::array<std::chrono::milliseconds, 6> ghostIntervals{250ms, 500ms, 750ms, 1000ms, 1250ms, 1500ms};

    for(std::size_t i{0}; i < ghostIntervals.size(); ++i)
    {
        if(remainingTime <= ghostIntervals[i])
        {
            if(i % 2 == 0)
                color = Color::white_black;
            else
                color = Color::blue_black;

            wattron(win.getWindow(), COLOR_PAIR(color));
            mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "ᗣ");
            wattroff(win.getWindow(), COLOR_PAIR(color));
            wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));
            wrefresh(win.getWindow());

            break;
        }
    }
}
