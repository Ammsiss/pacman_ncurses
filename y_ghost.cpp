#include <ncurses.h>

#include "z_aggregate.h"
#include "z_window.h"
#include "z_ghost.h"
#include "z_obstacle.h"
#include "z_random.h"

#include <chrono>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

// private members:

void Ghost::setY(PositionChange direction) 
{
    if (direction == PositionChange::decrement) --m_ghostVec.y;
    else if (direction == PositionChange::increment) ++m_ghostVec.y;
}

void Ghost::setX(PositionChange direction)
{
    if (direction == PositionChange::decrement) --m_ghostVec.x;
    else if (direction == PositionChange::increment) ++m_ghostVec.x;
}

void Ghost::setValidDirection(std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter)  // SHOULD CHECK 3 CONDITIONS: 1) ghost not in perimeter 2) ghost not in obstacle 3) ghost not move opposite direction
{
    Direction directionCheck{ m_direction };

    while(true)
    {
        m_direction = static_cast<Direction>(Random::randomDirection(Random::mt));


        // Possible refactor
        if (directionCheck == Direction::up && m_direction == Direction::down)
            continue;
        else if (directionCheck == Direction::left && m_direction == Direction::right)
            continue;
        else if (directionCheck == Direction::right && m_direction == Direction:: left)
            continue;
        else if (directionCheck == Direction::down && m_direction == Direction::up)
            continue;


        // breaks from loop if valid direction to use in movement and printing
        if( !moveGhostBasedOnDirection(obstacleList, windowPerimeter) )
            continue;
        else
            break;
    }
}

bool Ghost::perimeterBoundsCheck(std::vector<Vec>& windowPerimeter)
{
    for(std::size_t i{0}; i < windowPerimeter.size(); ++i)
    {
        if(m_ghostVec.y == windowPerimeter[i].y && m_ghostVec.x == windowPerimeter[i].x)
            return false;
    }

    return true;
}

bool Ghost::obstacleBoundsCheck(std::vector<Obstacle>& obstacleList)
{
    for(std::size_t iOuter{0}; iOuter < obstacleList.size(); ++iOuter)
    {
        for(std::size_t iInner{0}; iInner < obstacleList[iOuter].getObsVec().size(); ++iInner)
        {
            if(m_ghostVec.y == obstacleList[iOuter].getObsVec()[iInner].y 
                && m_ghostVec.x == obstacleList[iOuter].getObsVec()[iInner].x)
                {
                    return false;
                }
        }
    }

    return true;
}

void Ghost::erase(Window& win)
{
    mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, " ");
}

void Ghost::printAndRefreshGhost(Window& win)
{
    mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "ᗣ");
    wrefresh(win.getWindow());
}

bool Ghost::moveGhostBasedOnDirection(std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter)
{
    switch(m_direction)
        {
            case Direction::up: 
            {
                setY(PositionChange::decrement);
                if(!obstacleBoundsCheck(obstacleList) || !perimeterBoundsCheck(windowPerimeter))
                {
                    setY(PositionChange::increment);
                    return false;
                }
                return true;
            }
            case Direction::down: 
            {

                setY(PositionChange::increment);
                if(!obstacleBoundsCheck(obstacleList) || !perimeterBoundsCheck(windowPerimeter))
                {
                   setY(PositionChange::decrement);
                   return false;
                }
                return true;
            }
            case Direction::left: 
            {
                if(m_ghostVec.x == m_leftPortalX && m_ghostVec.y == m_portalY)
                {
                    // L -> R
                    m_ghostVec.x = m_rightPortalX;
                    return true;
                }
                setX(PositionChange::decrement); 
                if(!obstacleBoundsCheck(obstacleList) || !perimeterBoundsCheck(windowPerimeter))
                {
                    setX(PositionChange::increment);
                    return false;
                }
                return true;
            }
            case Direction::right: 
            {
                if(m_ghostVec.x == m_rightPortalX && m_ghostVec.y == m_portalY)
                {
                    // R -> L 
                    m_ghostVec.x = m_leftPortalX;
                    return true;
                }
                setX(PositionChange::increment); 
                if(!obstacleBoundsCheck(obstacleList) || !perimeterBoundsCheck(windowPerimeter))
                {
                    setX(PositionChange::decrement);
                    return false;
                }
                return true;
            }
            default: 
                break;
        }

    return false;
}

// public members:

Ghost::Ghost(std::chrono::milliseconds speed)
    : m_direction {Direction::right}
    , m_ghostVec {15, 12} // randomInput{},
    , m_interval{speed}
    , m_lastTime{std::chrono::high_resolution_clock::now()}
    , m_rightPortalX{ 26 }, m_leftPortalX{ 1 }
    , m_portalY{ 14 }
    {
    }

void Ghost::timeToMove(Window& win, std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter)
{
    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto currentTime{std::chrono::high_resolution_clock::now()};

    if (currentTime - m_lastTime >= m_interval)
    {
        erase(win);
        setValidDirection(obstacleList, windowPerimeter);
        printAndRefreshGhost(win);

        m_lastTime = currentTime;
    }
}
