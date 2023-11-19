#include <ncurses.h>
#include <vector>
#include <chrono>

#include "z_aggregate.h"
#include "z_pellet.h"
#include "z_window.h"
#include "z_pacman.h"
#include "z_obstacle.h"

#include <thread>

using namespace std::chrono_literals;

// private members:

void Pacman::setY(PositionChange direction) 
{
    if (direction == PositionChange::decrement) --m_pacVec.y;
    else if (direction == PositionChange::increment) ++m_pacVec.y;
}

void Pacman::setX(PositionChange direction)
{
    if (direction == PositionChange::decrement) --m_pacVec.x;
    else if (direction == PositionChange::increment) ++m_pacVec.x;
}

void Pacman::setDirection() 
{
    switch(m_userInput)
    {
        case 'w': m_direction = Direction::up; break;
        case 'a': m_direction = Direction::left; break;
        case 's': m_direction = Direction::down; break;
        case 'd': m_direction = Direction::right; break;
        default: m_direction = Direction::null; break;
    }
}

bool Pacman::obstacleBoundsCheck(std::vector<Obstacle>& obstacleList)
{
    for(std::size_t iOuter{0}; iOuter < obstacleList.size(); ++iOuter)
    {
        for(std::size_t iInner{0}; iInner < obstacleList[iOuter].getObsVec().size(); ++iInner)
        {
            if(m_pacVec.y == obstacleList[iOuter].getObsVec()[iInner].y 
                && m_pacVec.x == obstacleList[iOuter].getObsVec()[iInner].x)
                {
                    return true;
                }
        }
    }

    return false;
}

void Pacman::getUserInputAndSetDirection(Window& win)
{
    m_userInput = wgetch(win.getWindow());
    if (m_userInput != ERR)
    {
        setDirection();
    }

    // clears extra input as to not cause input buffer
    while (wgetch(win.getWindow()) != ERR) {}
}

void Pacman::erase(Window& win)
{
    mvwprintw(win.getWindow(), m_pacVec.y, m_pacVec.x, " ");
}

void Pacman::printPacmanBasedOnDirectionAndRefresh(Window& win)
{
    if(m_direction == Direction::up)
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));    
        mvwprintw(win.getWindow(), m_pacVec.y, m_pacVec.x, "v");
        wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
    }
    else if(m_direction == Direction::down)
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));  
        mvwprintw(win.getWindow(), m_pacVec.y, m_pacVec.x, "ʌ");
        wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
    }
    else if(m_direction == Direction::left)
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));  
        mvwprintw(win.getWindow(), m_pacVec.y, m_pacVec.x, ">");
        wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
    }
    else if(m_direction == Direction::right)
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));  
        mvwprintw(win.getWindow(), m_pacVec.y, m_pacVec.x, "<");
        wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
    }
    wrefresh(win.getWindow());
}

void Pacman::movePacmanBasedOnDirection(Window& win, std::vector<Obstacle>& obstacleList)
{
    switch(m_direction)
        {
            case Direction::up: 
            {
                if(m_pacVec.y != 1)
                {
                    setY(PositionChange::decrement);
                    if(obstacleBoundsCheck(obstacleList))
                        setY(PositionChange::increment);
                }
                break;
            }
            case Direction::down: 
            {
                if(m_pacVec.y != (win.getScreenY() - 2))
                {
                    setY(PositionChange::increment);
                    if(obstacleBoundsCheck(obstacleList))
                        setY(PositionChange::decrement);
                }
                break;
            }
            case Direction::left: 
            {
                if(m_pacVec.x == m_leftPortalX && m_pacVec.y == m_portalY)
                {
                    // L -> R
                    m_pacVec.x = m_rightPortalX;
                }
                else if(m_pacVec.x != 1)
                {
                    setX(PositionChange::decrement); 
                    if(obstacleBoundsCheck(obstacleList))
                        setX(PositionChange::increment);
                }
                break;
            }
            case Direction::right: 
            {
                if(m_pacVec.x == m_rightPortalX && m_pacVec.y == m_portalY)
                {
                    // R -> L 
                    m_pacVec.x = m_leftPortalX;
                }
                else if(m_pacVec.x != (win.getScreenX() - 2))
                {
                    setX(PositionChange::increment); 
                    if(obstacleBoundsCheck(obstacleList))
                        setX(PositionChange::decrement);
                }
                break;
            }
            default: 
                break;
        }
}

void Pacman::setGarbageValue(Pellet& pellets)
{
    const int garbage{10000};

    for(int y{0}; y < pellets.getPelletVec().size(); ++y)
    {
        for(int x{0}; x < pellets.getPelletVec()[y].size(); ++x)
        {
            if(m_pacVec.y == y && m_pacVec.x == x)
            {
                pellets.getPelletVec()[y][x] = garbage;
            }
        }
    }
}

// public members:

Pacman::Pacman()
    : m_direction {Direction::right}
    , m_pacVec {1, 1}, m_userInput{}
    , m_interval{175ms}
    , m_lastTime{std::chrono::high_resolution_clock::now()}
    , m_rightPortalX{ 26 }, m_leftPortalX{ 1 }
    , m_portalY{ 14 }
    {
    }

void Pacman::timeToMove(Window& win, std::vector<Obstacle>& obstacleList, Pellet& pellets)
{
    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto currentTime{std::chrono::high_resolution_clock::now()};

    getUserInputAndSetDirection(win);
    if (currentTime - m_lastTime >= m_interval)
    {
        erase(win);
        setGarbageValue(pellets);
        movePacmanBasedOnDirection(win, obstacleList);
        printPacmanBasedOnDirectionAndRefresh(win);

        m_lastTime = currentTime;
    }
}
