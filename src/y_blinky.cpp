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

//std
#include <vector>
#include <chrono>
#include <thread>
#include <queue>
#include <algorithm>

// public method

bool Blinky::timeToMove(Window& win, Pacman& pacman, Pinky& pinky, Inky& inky, Ghost& clyde)
{
    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto currentTime{std::chrono::high_resolution_clock::now()};

    if (currentTime - m_lastTime >= m_blinkyInterval)
    {
        eraseLastPosition(win);
        checkForAndPrintOverLaps(win, pinky, inky, clyde);

        if(!setDirection(win, pacman))
            return false;

        printAndRefreshGhost(win);

        m_lastTime = currentTime;
    }

    return true;
}

Vec Blinky::getBlinkyVec() { return m_blinkyVec; }

Color::ColorPair Blinky::getBlinkyColor() { return m_blinkyColor; }

/********************************************************************** PRIVATE MEMBERS **********************************************************************/

/* _   _   _   _   _   _   _     _   _   _   _   _   _   _   _   _  
  / \ / \ / \ / \ / \ / \ / \   / \ / \ / \ / \ / \ / \ / \ / \ / \   Functions related to checking ghost's location and either using it for printing or
 ( S | E | T | T | I | N | G ) ( D | I | R | E | C | T | I | O | N )  reverting it and trying again until a valid input is found.
  \_/ \_/ \_/ \_/ \_/ \_/ \_/   \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/   
*/

bool Blinky::setDirection(Window& win, Pacman& pacman)  
{
    std::vector<Vec> ghostPath{ createGhostPath(Vec{m_blinkyVec.y, m_blinkyVec.x}, win, Vec{pacman.getPacVec().y, pacman.getPacVec().x}) };

    m_blinkyVec.y = ghostPath[1].y;
    m_blinkyVec.x = ghostPath[1].x;

    if(m_blinkyVec.y == pacman.getPacVec().y && m_blinkyVec.x == pacman.getPacVec().x)
        return false;

    return true;
}

std::vector<Vec> Blinky::createGhostPath(Vec start, Window& win, Vec target)
{
    auto rows{win.getWindowArea().size()};
    auto cols{win.getWindowArea()[0].size()};

    std::vector<std::vector<int>> visited(rows, std::vector<int>(cols, 0));
    std::vector<std::vector<Vec>> parentCell(rows, std::vector<Vec>(cols));

    std::queue<Vec> q{};
    q.push(start);                  // push in start point to q (0, 0)
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
            if(next.y >= 0 && next.y < rows && next.x >= 0 && next.x < cols && win.getWindowArea()[next.y][next.x] != CellName::obstacleValue && 
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

void Blinky::eraseLastPosition(Window& win)
{
    mvwprintw(win.getWindow(), m_blinkyVec.y, m_blinkyVec.x, " ");
}

// deals with overlaps (pellets && ghost)
void Blinky::checkForAndPrintOverLaps(Window& win, Pinky& pinky, Inky& inky, Ghost& clyde)
{
    printPelletBackIfNotEaten(win);
    printOverLap(win, checkGhostOverLap(pinky, inky, clyde));
}

void Blinky::printPelletBackIfNotEaten(Window& win)
{
    if(win.getWindowArea()[m_blinkyVec.y][m_blinkyVec.x] != CellName::pelletEaten && win.getWindowArea()[m_blinkyVec.y][m_blinkyVec.x] != CellName::ghostBox)
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::white_black));
        mvwprintw(win.getWindow(), m_blinkyVec.y, m_blinkyVec.x, "•");
        wattroff(win.getWindow(), COLOR_PAIR(Color::white_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));    
    }
}

Color::ColorPair Blinky::checkGhostOverLap(Pinky& pinky, Inky& inky, Ghost& clyde)
{
    if(m_blinkyVec.y == pinky.getPinkyVec().y && m_blinkyVec.x == pinky.getPinkyVec().x)
        return pinky.getPinkyColor();

    if(m_blinkyVec.y == inky.getInkyVec().y && m_blinkyVec.x == inky.getInkyVec().x)
        return inky.getInkyColor();
    
    if(m_blinkyVec.y == clyde.getGhostVec().y && m_blinkyVec.x == clyde.getGhostVec().x)
        return clyde.getGhostColor();

    return Color::null;
}

void Blinky::printOverLap(Window& win, Color::ColorPair overLapColor)
{
    // prints ghost back if overlapped
    if(overLapColor != Color::null)
    {
        wattron(win.getWindow(), COLOR_PAIR(overLapColor));
        mvwprintw(win.getWindow(), m_blinkyVec.y, m_blinkyVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(overLapColor));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
    }
}

void Blinky::printAndRefreshGhost(Window& win)
{
    wattron(win.getWindow(), COLOR_PAIR(m_blinkyColor));
    mvwprintw(win.getWindow(), m_blinkyVec.y, m_blinkyVec.x, "ᗣ");
    wattroff(win.getWindow(), COLOR_PAIR(m_blinkyColor));
    wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));

    wrefresh(win.getWindow());
}