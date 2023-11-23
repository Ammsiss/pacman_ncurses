//external
#include <ncurses.h>

//user
#include "z_inky.h"
#include "z_window.h"
#include "z_aggregate.h"
#include "z_obstacle.h"
#include "z_random.h"
#include "z_pacman.h"
#include "z_random.h"
#include "z_pinky.h"
#include "z_blinky.h"

//std
#include <vector>
#include <chrono>
#include <thread>
#include <queue>
#include <algorithm>

// public method

bool Inky::timeToMove(Window& win, Pacman& pacman, Pinky& pinky, Blinky& blinky, Ghost& clyde)
{
    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto currentTime{std::chrono::high_resolution_clock::now()};

    if (currentTime - m_lastTime >= m_inkyInterval)
    {
        eraseLastPosition(win);
        checkForAndPrintOverLaps(win, pinky, blinky, clyde);

        if(!setDirection(win, pacman))
            return false;

        printAndRefreshGhost(win);

        m_lastTime = currentTime;
    }

    return true;
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

bool Inky::setDirection(Window& win, Pacman& pacman)  
{
    static int targetY{1};
    static int targetX{1};

    if(m_inkyVec.y == targetY && m_inkyVec.x == targetX)
    {
        while(true)
        {
            targetY = Random::randomGhostTargetY(Random::mt);
            targetX = Random::randomGhostTargetX(Random::mt);

            if((targetY != m_inkyVec.y && targetX != m_inkyVec.x) && win.getWindowArea()[targetY][targetX] != CellName::ghostBox && win.getWindowArea()[targetY][targetX] != CellName::perimeterValue && win.getWindowArea()[targetY][targetX] != CellName::obstacleValue)
                break;
        }
    }

    std::vector<Vec> ghostPath{ createGhostPath(Vec{m_inkyVec.y, m_inkyVec.x}, win, Vec{targetY, targetX}) };

    m_inkyVec.y = ghostPath[1].y;
    m_inkyVec.x = ghostPath[1].x;

    // checks pacman collision
    if(m_inkyVec.y == pacman.getPacVec().y && m_inkyVec.x == pacman.getPacVec().x)
        return false;

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
void Inky::checkForAndPrintOverLaps(Window& win, Pinky& pinky, Blinky& blinky, Ghost& clyde)
{
    printPelletBackIfNotEaten(win);
    printOverLap(win, checkGhostOverLap(pinky, blinky, clyde));
}

void Inky::printPelletBackIfNotEaten(Window& win)
{
    if(win.getWindowArea()[m_inkyVec.y][m_inkyVec.x] != CellName::pelletEaten && win.getWindowArea()[m_inkyVec.y][m_inkyVec.x] != CellName::ghostBox &&
        win.getWindowArea()[m_inkyVec.y][m_inkyVec.x] != CellName::perimeterValue)
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::white_black));
        mvwprintw(win.getWindow(), m_inkyVec.y, m_inkyVec.x, "•");
        wattroff(win.getWindow(), COLOR_PAIR(Color::white_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));    
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

void Inky::printOverLap(Window& win, Color::ColorPair overLapColor)
{
    // prints ghost back if overlapped
    if(overLapColor != Color::null)
    {
        wattron(win.getWindow(), COLOR_PAIR(overLapColor));
        mvwprintw(win.getWindow(), m_inkyVec.y, m_inkyVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(overLapColor));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
    }
}

void Inky::printAndRefreshGhost(Window& win)
{
    wattron(win.getWindow(), COLOR_PAIR(m_inkyColor));
    mvwprintw(win.getWindow(), m_inkyVec.y, m_inkyVec.x, "ᗣ");
    wattroff(win.getWindow(), COLOR_PAIR(m_inkyColor));
    wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));

    wrefresh(win.getWindow());
}