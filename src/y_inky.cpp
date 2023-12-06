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

bool Inky::timeToMove(Window& win, Pacman& pacman, Pinky& pinky, Blinky& blinky, Ghost& clyde, bool powerPelletTimer, LevelState ateWhichGhost, int& score, TypeAlias::timepoint& lastTime, std::chrono::milliseconds& interval)
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
        ghostFlashing(win, lastTime, interval, currentTime);

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
                ColorUtils::colorOn(Color::yellow_black, win);
                mvwprintw(win.getWindow(), m_inkyVec.y, m_inkyVec.x, "+");
                ColorUtils::colorOff(Color::yellow_black, win);
                wrefresh(win.getWindow());
                lastTime += 1100ms;
                std::this_thread::sleep_for(1s);

                score += 20;
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
    ColorUtils::colorOn(Color::cyan_black, win);
    mvwprintw(win.getWindow(), 15, 12, "ᗣ");
    ColorUtils::colorOn(Color::cyan_black, win);
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

            if((targetY != m_inkyVec.y || targetX != m_inkyVec.x) && win[{targetY, targetX}] != CellName::ghostBox && win[{targetY, targetX}] != CellName::perimeterValue && win[{targetY, targetX}] != CellName::obstacleValue)
                break;
        }
    }

    auto currentTime{ std::chrono::high_resolution_clock::now() };    

    std::vector<Vec> ghostPath{};
    if(!powerPelletActive)
        {
            if(currentTime - m_pathLastTime >= m_inkyPathInterval)
            {
                if(m_inkyVec.y != pacman.getPacVec().y || m_inkyVec.x != pacman.getPacVec().x)
                {
                    ghostPath = createGhostPath(Vec{m_inkyVec.y, m_inkyVec.x}, win, Vec{pacman.getPacVec().y, pacman.getPacVec().x});
                    m_inkyVec.y = ghostPath[1].y;
                    m_inkyVec.x = ghostPath[1].x;
                }
                else
                {
                    ghostPath = createGhostPath(Vec{m_inkyVec.y, m_inkyVec.x}, win, Vec{1, 1});
                    m_inkyVec.y = ghostPath[1].y;
                    m_inkyVec.x = ghostPath[1].x;
                }
                
                if(currentTime - m_pathLastTime >= 16s)
                    m_pathLastTime = currentTime;
            }
            else
            {
                std::vector<Vec> ghostPath{ createGhostPath(Vec{m_inkyVec.y, m_inkyVec.x}, win, Vec{targetY, targetX}) };
                m_inkyVec.y = ghostPath[1].y;
                m_inkyVec.x = ghostPath[1].x;
            }
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
    return GhostUtils::createGhostPath(start, win, target);
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
    if(win[{m_inkyVec.y, m_inkyVec.x}] != CellName::pelletEaten && win[{m_inkyVec.y, m_inkyVec.x}] != CellName::ghostBox &&
        win[{m_inkyVec.y, m_inkyVec.x}] != CellName::perimeterValue && win[{m_inkyVec.y, m_inkyVec.x}] != CellName::powerPelletEaten)
    {
        if(win[{m_inkyVec.y, m_inkyVec.x}] == CellName::powerPellet)
        {
            ColorUtils::colorOn(Color::yellow_black, win, true);
            mvwprintw(win.getWindow(), m_inkyVec.y, m_inkyVec.x, "⬤");
            ColorUtils::colorOff(Color::yellow_black, win);
        }
        else
        {
            ColorUtils::colorOn(Color::white_black, win);
            mvwprintw(win.getWindow(), m_inkyVec.y, m_inkyVec.x, "•");
            ColorUtils::colorOff(Color::white_black, win);
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
    PrintingUtils::printOverLap(win, overLapColor, powerPelletActive, m_inkyVec);
}

void Inky::printAndRefreshGhost(Window& win, bool powerPelletActive)
{
    PrintingUtils::printAndRefreshGhost(win, powerPelletActive, m_inkyColor, m_inkyVec);
}

// Every 250 ms in the last 1.5 seconds of the power pellet change ghost color to indicate time is almost out
void Inky::ghostFlashing(Window& win, TypeAlias::timepoint& lastTime, std::chrono::milliseconds& interval, TypeAlias::timepoint& currentTime)
{
    PrintingUtils::ghostFlashing(win, lastTime, interval, currentTime, m_inkyVec);
}
