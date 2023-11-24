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

bool Inky::timeToMove(Window& win, Pacman& pacman, Pinky& pinky, Blinky& blinky, Ghost& clyde, bool powerPelletTimer, LevelState ateWhichGhost, int& score, std::chrono::time_point<std::chrono::high_resolution_clock>& lastTime)
{
    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto currentTime{std::chrono::high_resolution_clock::now()};

    // if power pellet not active
    if(!powerPelletTimer)
    {
        if (currentTime - m_lastTime >= m_inkyInterval)
        {
            eraseLastPosition(win);
            checkForAndPrintOverLaps(win, pinky, blinky, clyde, powerPelletTimer);

            if(!setDirection(win, pacman, false))
                return false;

            printAndRefreshGhost(win, false);

            m_lastTime = currentTime;
        }

        return true;
    }
    else // if power pellet is active
    {
        m_inkyIntervalStorage = m_inkyInterval;
        m_inkyInterval = 350ms;

        if(ateWhichGhost == LevelState::ateInky)
        {
            m_inkyVec.y = 15;
            m_inkyVec.x = 14;
        }

        if(currentTime - m_lastTime >= m_inkyInterval)
        {
            eraseLastPosition(win);
            checkForAndPrintOverLaps(win, pinky, blinky, clyde, powerPelletTimer);
            
           if(!setDirection(win, pacman, true))
            {
                wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));
                mvwprintw(win.getWindow(), m_inkyVec.y, m_inkyVec.x, "+");
                wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
                wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
                wrefresh(win.getWindow());
                lastTime += 1100ms;
                std::this_thread::sleep_for(1s);

                score += 200;
                m_inkyVec.y = 15;
                m_inkyVec.x = 12;
            }

            printAndRefreshGhost(win, true);

            m_lastTime = currentTime;
        }

        m_inkyInterval = m_inkyIntervalStorage;

        return true;
    }
}

void Inky::printGhost(Window& win)
{
    wattron(win.getWindow(), COLOR_PAIR(Color::cyan_black));
    mvwprintw(win.getWindow(), 15, 12, "ᗣ");
    wattron(win.getWindow(), COLOR_PAIR(Color::cyan_black));
    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
}

Vec Inky::getInkyVec() { return m_inkyVec; }

Color::ColorPair Inky::getInkyColor() { return m_inkyColor; }

void Inky::setSpeed() { m_inkyInterval -= 5ms; }

void Inky::setGhostVec() { m_inkyVec = Vec{15, 12}; }

/********************************************************************** PRIVATE MEMBERS **********************************************************************/

/* _   _   _   _   _   _   _     _   _   _   _   _   _   _   _   _  
  / \ / \ / \ / \ / \ / \ / \   / \ / \ / \ / \ / \ / \ / \ / \ / \   Functions related to checking ghost's location and either using it for printing or
 ( S | E | T | T | I | N | G ) ( D | I | R | E | C | T | I | O | N )  reverting it and trying again until a valid input is found.
  \_/ \_/ \_/ \_/ \_/ \_/ \_/   \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/   
*/

bool Inky::setDirection(Window& win, Pacman& pacman, bool powerPelletActive)  
{
    static int targetY{1};
    static int targetX{1};

    if(m_inkyVec.y == targetY && m_inkyVec.x == targetX)
    {
        while(true)
        {
            targetY = Random::randomGhostTargetY(Random::mt);
            targetX = Random::randomGhostTargetX(Random::mt);

            if((targetY != m_inkyVec.y || targetX != m_inkyVec.x) && win.getWindowArea()[targetY][targetX] != CellName::ghostBox && win.getWindowArea()[targetY][targetX] != CellName::perimeterValue && win.getWindowArea()[targetY][targetX] != CellName::obstacleValue)
                break;
        }
    }

    std::vector<Vec> ghostPath{};

    if(!powerPelletActive)
    {
        std::vector<Vec> ghostPath{ createGhostPath(Vec{m_inkyVec.y, m_inkyVec.x}, win, Vec{targetY, targetX}) };
        m_inkyVec.y = ghostPath[1].y;
        m_inkyVec.x = ghostPath[1].x;
    }
    else
    {
        if(m_inkyVec.y != 3 || m_inkyVec.x != 1)
        {
            ghostPath = createGhostPath(Vec{m_inkyVec.y, m_inkyVec.x}, win, Vec{3, 1});
            m_inkyVec.y = ghostPath[1].y;
            m_inkyVec.x = ghostPath[1].x;
        }
        else
        {
            ghostPath = createGhostPath(Vec{m_inkyVec.y, m_inkyVec.x}, win, Vec{5, 1});
            m_inkyVec.y = ghostPath[1].y;
            m_inkyVec.x = ghostPath[1].x;
        }
    }


    if(m_inkyVec.y == pacman.getPacVec().y && m_inkyVec.x == pacman.getPacVec().x)
        return false;
    else
        return true;
}

std::vector<Vec> Inky::createGhostPath(Vec start, Window& win, Vec target)
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
            if(next.y > 0 && next.y < (rows - 1) && next.x > 0 && next.x < (cols - 1) && win.getWindowArea()[next.y][next.x] != CellName::obstacleValue && 
                win.getWindowArea()[next.y][next.x] != CellName::perimeterValue && !visited[next.y][next.x])
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

void Inky::eraseLastPosition(Window& win)
{
    mvwprintw(win.getWindow(), m_inkyVec.y, m_inkyVec.x, " ");
}

// deals with overlaps (pellets && ghost)
void Inky::checkForAndPrintOverLaps(Window& win, Pinky& pinky, Blinky& blinky, Ghost& clyde, bool powerPelletActive)
{
    printPelletBackIfNotEaten(win);
    printOverLap(win, checkGhostOverLap(pinky, blinky, clyde), powerPelletActive);
}

void Inky::printPelletBackIfNotEaten(Window& win)
{
    if(win.getWindowArea()[m_inkyVec.y][m_inkyVec.x] != CellName::pelletEaten && win.getWindowArea()[m_inkyVec.y][m_inkyVec.x] != CellName::ghostBox &&
        win.getWindowArea()[m_inkyVec.y][m_inkyVec.x] != CellName::perimeterValue && win.getWindowArea()[m_inkyVec.y][m_inkyVec.x] != CellName::powerPelletEaten)
    {
        if(win.getWindowArea()[m_inkyVec.y][m_inkyVec.x] == CellName::powerPellet)
        {
            wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));
            wattron(win.getWindow(), A_BLINK);
            mvwprintw(win.getWindow(), m_inkyVec.y, m_inkyVec.x, "⬤");
            wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
            wattroff(win.getWindow(), A_BLINK);
            wattron(win.getWindow(), COLOR_PAIR(Color::default_color));  
        }
        else
        {
        wattron(win.getWindow(), COLOR_PAIR(Color::white_black));
        mvwprintw(win.getWindow(), m_inkyVec.y, m_inkyVec.x, "•");
        wattroff(win.getWindow(), COLOR_PAIR(Color::white_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));    
        }
    }
}

Color::ColorPair Inky::checkGhostOverLap(Pinky& pinky, Blinky& blinky, Ghost& clyde)
{
    if(m_inkyVec.y == pinky.getPinkyVec().y && m_inkyVec.x == pinky.getPinkyVec().x)
        return pinky.getPinkyColor();

    if(m_inkyVec.y == blinky.getBlinkyVec().y && m_inkyVec.x == blinky.getBlinkyVec().x)
        return blinky.getBlinkyColor();
    
    if(m_inkyVec.y == clyde.getGhostVec().y && m_inkyVec.x == clyde.getGhostVec().x)
        return clyde.getGhostColor();

    return Color::null;
}

void Inky::printOverLap(Window& win, Color::ColorPair overLapColor, bool powerPelletActive)
{
    // prints ghost back if overlapped
    if(powerPelletActive && overLapColor != Color::null)
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::blue_black));
        mvwprintw(win.getWindow(), m_inkyVec.y, m_inkyVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(Color::blue_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color)); 
    }
    else if(overLapColor != Color::null)
    {
        wattron(win.getWindow(), COLOR_PAIR(overLapColor));
        mvwprintw(win.getWindow(), m_inkyVec.y, m_inkyVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(overLapColor));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
    }
}

void Inky::printAndRefreshGhost(Window& win, bool powerPelletActive)
{
    if(!powerPelletActive)
    {
        wattron(win.getWindow(), COLOR_PAIR(m_inkyColor));
        mvwprintw(win.getWindow(), m_inkyVec.y, m_inkyVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(m_inkyColor));
        wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));
    }
    else
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::blue_black));
        mvwprintw(win.getWindow(), m_inkyVec.y, m_inkyVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(Color::blue_black));
        wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));
    }

    wrefresh(win.getWindow());
}