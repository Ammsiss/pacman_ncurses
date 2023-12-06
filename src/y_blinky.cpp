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
#include <utility>
#include <array>

using namespace std::chrono_literals;

// public method

bool Blinky::timeToMove(Window& win, Pacman& pacman, Pinky& pinky, Inky& inky, Ghost& clyde, bool powerPelletTimer, LevelState ateWhichGhost, int& score, TypeAlias::timepoint& lastTime, std::chrono::milliseconds& interval)
{
    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto currentTime{std::chrono::high_resolution_clock::now()};

    // if power pellet not active
    if(!powerPelletTimer)
    {
        if (currentTime - m_lastTime >= m_blinkyInterval)
        {
            eraseLastPosition(win);
            checkForAndPrintOverLaps(win, pinky, inky, clyde, powerPelletTimer);

            if(!setDirection(win, pacman, false))
                return false;

            printAndRefreshGhost(win, false);

            m_lastTime = currentTime;
        }

        return true;
    }
    else // if power pellet is active
    {
        ghostFlashing(win, lastTime, interval, currentTime);

        m_blinkyIntervalStorage = m_blinkyInterval;
        m_blinkyInterval = 350ms;

        if(ateWhichGhost == LevelState::ateBlinky)
        {
            m_blinkyVec.y = 15;
            m_blinkyVec.x = 14;
        }

        if(currentTime - m_lastTime >= m_blinkyInterval)
        {
            eraseLastPosition(win);
            checkForAndPrintOverLaps(win, pinky, inky, clyde, powerPelletTimer);
            
           if(!setDirection(win, pacman, true))
            {
                ColorUtils::colorOn(Color::yellow_black, win);
                mvwprintw(win.getWindow(), m_blinkyVec.y, m_blinkyVec.x, "+");
                ColorUtils::colorOff(Color::yellow_black, win);
                wrefresh(win.getWindow());
                lastTime += 1100ms;
                std::this_thread::sleep_for(1s);

                score += 20;
                m_blinkyVec.y = 15;
                m_blinkyVec.x = 14;
            }

            printAndRefreshGhost(win, true);

            m_lastTime = currentTime;
        }

        m_blinkyInterval = m_blinkyIntervalStorage;

        return true;
    }
}

void Blinky::printGhost(Window& win)
{
    ColorUtils::colorOn(Color::red_black, win);
    mvwprintw(win.getWindow(), 15, 14, "ᗣ");
    ColorUtils::colorOff(Color::red_black, win);
}

Vec Blinky::getBlinkyVec() { return m_blinkyVec; }

Color::ColorPair Blinky::getBlinkyColor() { return m_blinkyColor; }

void Blinky::setSpeed() { m_blinkyInterval -= 5ms; }

void Blinky::setGhostVec() { m_blinkyVec = Vec{15, 14}; }

/********************************************************************** PRIVATE MEMBERS **********************************************************************/

/* _   _   _   _   _   _   _     _   _   _   _   _   _   _   _   _  
  / \ / \ / \ / \ / \ / \ / \   / \ / \ / \ / \ / \ / \ / \ / \ / \   Functions related to checking ghost's location and either using it for printing or
 ( S | E | T | T | I | N | G ) ( D | I | R | E | C | T | I | O | N )  reverting it and trying again until a valid input is found.
  \_/ \_/ \_/ \_/ \_/ \_/ \_/   \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/   
*/

bool Blinky::setDirection(Window& win, Pacman& pacman, bool powerPelletActive)  
{
    std::vector<Vec> ghostPath{};

    if(!powerPelletActive)
    {
        if(m_blinkyVec.y != pacman.getPacVec().y || m_blinkyVec.x != pacman.getPacVec().x)
        {
            ghostPath = createGhostPath(Vec{m_blinkyVec.y, m_blinkyVec.x}, win, Vec{pacman.getPacVec().y, pacman.getPacVec().x});
            m_blinkyVec.y = ghostPath[1].y;
            m_blinkyVec.x = ghostPath[1].x;
        }
        else
        {
            ghostPath = createGhostPath(Vec{m_blinkyVec.y, m_blinkyVec.x}, win, Vec{1, 1});
            m_blinkyVec.y = ghostPath[1].y;
            m_blinkyVec.x = ghostPath[1].x;
        }
    }
    else
    {
        if(m_blinkyVec.y != 22 || m_blinkyVec.x != 1)
        {
            ghostPath = createGhostPath(Vec{m_blinkyVec.y, m_blinkyVec.x}, win, Vec{22, 1});
            m_blinkyVec.y = ghostPath[1].y;
            m_blinkyVec.x = ghostPath[1].x;
        }
        else
        {
            ghostPath = createGhostPath(Vec{m_blinkyVec.y, m_blinkyVec.x}, win, Vec{20, 1});
            m_blinkyVec.y = ghostPath[1].y;
            m_blinkyVec.x = ghostPath[1].x;
        }
    }


    if(m_blinkyVec.y == pacman.getPacVec().y && m_blinkyVec.x == pacman.getPacVec().x)
        return false;
    else
        return true;
}

std::vector<Vec> Blinky::createGhostPath(Vec start, Window& win, Vec target)
{
    return GhostUtils::createGhostPath(start, win, target);
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
void Blinky::checkForAndPrintOverLaps(Window& win, Pinky& pinky, Inky& inky, Ghost& clyde, bool powerPelletActive)
{
    printPelletBackIfNotEaten(win);
    printOverLap(win, checkGhostOverLap(pinky, inky, clyde), powerPelletActive);
}

void Blinky::printPelletBackIfNotEaten(Window& win)
{
    if(win[{m_blinkyVec.y, m_blinkyVec.x}] != CellName::pelletEaten && win[{m_blinkyVec.y, m_blinkyVec.x}] != CellName::ghostBox &&
        win[{m_blinkyVec.y, m_blinkyVec.x}] != CellName::perimeterValue && win[{m_blinkyVec.y, m_blinkyVec.x}] != CellName::powerPelletEaten)
    {
        if(win[{m_blinkyVec.y, m_blinkyVec.x}] == CellName::powerPellet)
        {
            ColorUtils::colorOn(Color::yellow_black, win, true);
            mvwprintw(win.getWindow(), m_blinkyVec.y, m_blinkyVec.x, "⬤");
            ColorUtils::colorOff(Color::yellow_black, win);
        }
        else
        {
            ColorUtils::colorOn(Color::white_black, win);
            mvwprintw(win.getWindow(), m_blinkyVec.y, m_blinkyVec.x, "•");
            ColorUtils::colorOff(Color::white_black, win);
        }
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

void Blinky::printOverLap(Window& win, Color::ColorPair overLapColor, bool powerPelletActive)
{
    // prints ghost back if overlapped
    if(powerPelletActive && overLapColor != Color::null)
    {
        ColorUtils::colorOn(Color::blue_black, win);
        mvwprintw(win.getWindow(), m_blinkyVec.y, m_blinkyVec.x, "ᗣ");
        ColorUtils::colorOff(Color::blue_black, win);
    }
    else if(overLapColor != Color::null)
    {
        ColorUtils::colorOn(overLapColor, win);
        mvwprintw(win.getWindow(), m_blinkyVec.y, m_blinkyVec.x, "ᗣ");
        ColorUtils::colorOff(overLapColor, win);
    }
}

void Blinky::printAndRefreshGhost(Window& win, bool powerPelletActive)
{
    PrintingUtils::printAndRefreshGhost(win, powerPelletActive, m_blinkyColor, m_blinkyVec);
}

// Every 250 ms in the last 1.5 seconds of the power pellet change ghost color to indicate time is almost out
void Blinky::ghostFlashing(Window& win, TypeAlias::timepoint& lastTime, std::chrono::milliseconds& interval, TypeAlias::timepoint& currentTime)
{
    PrintingUtils::ghostFlashing(win, lastTime, interval, currentTime, m_blinkyVec);
}
