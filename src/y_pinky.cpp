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
#include "typealiases.h"
#include "color_utils.h"
#include "ghost_utils.h"
#include "printing_utils.h"

//std
#include <vector>
#include <chrono>
#include <thread>
#include <queue>
#include <algorithm>

using namespace std::chrono_literals;

// public method

bool Pinky::timeToMove(Window& win, Pacman& pacman, Inky& inky, Blinky& blinky, Ghost& clyde, bool powerPelletTimer, LevelState ateWhichGhost, int& score, TypeAlias::timepoint& lastTime, std::chrono::milliseconds& interval)
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
                ColorUtils::colorOn(Color::yellow_black, win);
                mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "+");
                ColorUtils::colorOff(Color::yellow_black, win);
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
    ColorUtils::colorOn(Color::pink_black, win);
    mvwprintw(win.getWindow(), 15, 15, "ᗣ");
    ColorUtils::colorOff(Color::pink_black, win);
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
    return GhostUtils::createGhostPath(start, win, target);
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
            ColorUtils::colorOn(Color::yellow_black, win, true);
            mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "⬤");
            ColorUtils::colorOff(Color::yellow_black, win);
        }
        else
        {
            ColorUtils::colorOn(Color::white_black, win);
            mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "•");
            ColorUtils::colorOff(Color::white_black, win);    
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
        ColorUtils::colorOn(Color::blue_black, win);
        mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "ᗣ");
        ColorUtils::colorOff(Color::blue_black, win);
    }
    else if(overLapColor != Color::null)
    {
        ColorUtils::colorOn(overLapColor, win);
        mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "ᗣ");
        ColorUtils::colorOff(overLapColor, win);
    }
}

void Pinky::printAndRefreshGhost(Window& win, bool powerPelletActive)
{
    if(!powerPelletActive)
    {
        ColorUtils::colorOn(m_pinkyColor, win);
        mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "ᗣ");
        ColorUtils::colorOff(m_pinkyColor, win);
    }
    else
    {
        ColorUtils::colorOn(Color::blue_black, win);
        mvwprintw(win.getWindow(), m_pinkyVec.y, m_pinkyVec.x, "ᗣ");
        ColorUtils::colorOff(Color::blue_black, win);
    }

    wrefresh(win.getWindow());
}

// Every 250 ms in the last 1.5 seconds of the power pellet change ghost color to indicate time is almost out
void Pinky::ghostFlashing(Window& win, TypeAlias::timepoint& lastTime, std::chrono::milliseconds& interval, TypeAlias::timepoint& currentTime)
{
    PrintingUtils::ghostFlashing(win, lastTime, interval, currentTime, m_pinkyVec);
}
