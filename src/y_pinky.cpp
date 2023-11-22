//external
#include <ncurses.h>

//user
#include "z_pinky.h"
#include "z_window.h"
#include "z_aggregate.h"
#include "z_obstacle.h"
#include "z_random.h"
#include "z_pacman.h"
#include "z_blinky.h"

//std
#include <vector>
#include <chrono>
#include <thread>
#include <queue>
#include <algorithm>

// public method

bool Pinky::timeToMove(Window& win, Pacman& pacman, Inky& inky, Blinky& blinky, Ghost& clyde)
{
    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto currentTime{std::chrono::high_resolution_clock::now()};

    if (currentTime - m_lastTime >= m_pinkyInterval)
    {
        eraseLastPosition(win);
        checkForAndPrintOverLaps(win, inky, blinky, clyde);

        if(!setDirection(win, pacman, blinky))
            return false;

        printAndRefreshGhost(win);

        m_lastTime = currentTime;
    }

    return true;
}

Vec Pinky::getPinkyVec() { return m_pinkyVec; }

Color::ColorPair Pinky::getPinkyColor() { return m_pinkyColor; }

/********************************************************************** PRIVATE MEMBERS **********************************************************************/

/* _   _   _   _   _   _   _     _   _   _   _   _   _   _   _   _  
  / \ / \ / \ / \ / \ / \ / \   / \ / \ / \ / \ / \ / \ / \ / \ / \   Functions related to checking ghost's location and either using it for printing or
 ( S | E | T | T | I | N | G ) ( D | I | R | E | C | T | I | O | N )  reverting it and trying again until a valid input is found.
  \_/ \_/ \_/ \_/ \_/ \_/ \_/   \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/   
*/

bool Pinky::setDirection(Window& win, Pacman& pacman, Blinky& blinky)  
{
    std::vector<Vec> ghostPath{};

    if(m_pinkyVec.y != blinky.getBlinkyVec().y || m_pinkyVec.x != blinky.getBlinkyVec().x)
    {
        ghostPath = createGhostPath(Vec{m_pinkyVec.y, m_pinkyVec.x}, win, Vec{blinky.getBlinkyVec().y, blinky.getBlinkyVec().x});
        m_pinkyVec.y = ghostPath[1].y;
        m_pinkyVec.x = ghostPath[1].x;
    }

    if(m_pinkyVec.y == pacman.getPacVec().y && m_pinkyVec.x == pacman.getPacVec().x)
        return false;

    return true;
}

std::vector<Vec> Pinky::createGhostPath(Vec start, Window& win, Vec target)
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

void Pinky::eraseLastPosition(Window& win)
{
    mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, " ");
}

// deals with overlaps (pellets && ghost)
void Pinky::checkForAndPrintOverLaps(Window& win, Inky& inky, Blinky& blinky, Ghost& clyde)
{
    printPelletBackIfNotEaten(win);
    printOverLap(win, checkGhostOverLap(inky, blinky, clyde));
}

void Pinky::printPelletBackIfNotEaten(Window& win)
{
    if(win.getWindowArea()[m_pinkyVec.y][m_pinkyVec.x] != CellName::pelletEaten && win.getWindowArea()[m_pinkyVec.y][m_pinkyVec.x] != CellName::ghostBox &&
        win.getWindowArea()[m_pinkyVec.y][m_pinkyVec.x] != CellName::perimeterValue)
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::white_black));
        mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "•");
        wattroff(win.getWindow(), COLOR_PAIR(Color::white_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));    
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

void Pinky::printOverLap(Window& win, Color::ColorPair overLapColor)
{
    // prints ghost back if overlapped
    if(overLapColor != Color::null)
    {
        wattron(win.getWindow(), COLOR_PAIR(overLapColor));
        mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(overLapColor));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
    }
}

void Pinky::printAndRefreshGhost(Window& win)
{
    wattron(win.getWindow(), COLOR_PAIR(m_pinkyColor));
    mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "ᗣ");
    wattroff(win.getWindow(), COLOR_PAIR(m_pinkyColor));
    wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));

    wrefresh(win.getWindow());
}