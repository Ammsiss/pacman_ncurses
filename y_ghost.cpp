#include <ncurses.h>

#include "z_aggregate.h"
#include "z_window.h"
#include "z_ghost.h"
#include "z_obstacle.h"
#include "z_random.h"
#include "z_pellet.h"

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

bool Ghost::oppositeDirectionCheck(Direction directionCheck)
{
    m_direction = static_cast<Direction>(Random::randomDirection(Random::mt));

    switch(directionCheck)
    {
        case Direction::down:
            return m_direction == Direction::up;
        case Direction::up:
            return m_direction == Direction::down;
        case Direction::left:
            return m_direction == Direction::right;
        case Direction::right:
            return m_direction == Direction::left;
        default:
            return false;
    }

    return false;
}

// SHOULD CHECK 3 CONDITIONS: 1) ghost not in perimeter 2) ghost not in obstacle 3) ghost not move opposite direction
// delegates checks to oppositeDirectionCheck for the direction check and moveGhostInValidDirection for the obstacle and perimeter check
void Ghost::setValidDirection(std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter, std::vector<std::vector<int>>& windowArea)  
{
    Direction directionCheck{ m_direction };

    while(true)
    {
        // Ghost cant move in direction he came
        if(oppositeDirectionCheck(directionCheck))
            continue;

        // breaks from loop if valid direction to use in movement and printing
        if(moveGhostInValidDirection(obstacleList, windowPerimeter, windowArea))
            break;
    }
}

bool Ghost::perimeterBoundsCheck(std::vector<Vec>& windowPerimeter, std::vector<std::vector<int>>& windowArea)
{
    const int perimeterValue{10001};

    if(windowArea[m_ghostVec.y][m_ghostVec.x] == perimeterValue)
        return false;

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
    wattron(win.getWindow(), COLOR_PAIR(m_ghostColor));
    mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "ᗣ");
    wattroff(win.getWindow(), COLOR_PAIR(m_ghostColor));
    wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));

    wrefresh(win.getWindow());
}


bool Ghost::moveGhostInValidDirection(std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter, std::vector<std::vector<int>>& windowArea)
{
    // attempt a move, check if valid, and revert the move if its not
    // delegates checks to obstacle and perimeter bounds check
    switch(m_direction)
        {
            case Direction::up: 
            {
                setY(PositionChange::decrement);
                if(!obstacleBoundsCheck(obstacleList) || !perimeterBoundsCheck(windowPerimeter, windowArea))
                {
                    setY(PositionChange::increment);
                    return false;
                }
                return true;
            }
            case Direction::down: 
            {

                setY(PositionChange::increment);
                if(!obstacleBoundsCheck(obstacleList) || !perimeterBoundsCheck(windowPerimeter, windowArea))
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
                if(!obstacleBoundsCheck(obstacleList) || !perimeterBoundsCheck(windowPerimeter, windowArea))
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
                if(!obstacleBoundsCheck(obstacleList) || !perimeterBoundsCheck(windowPerimeter, windowArea))
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

Color::ColorPair Ghost::checkGhostOverLap(Ghost& pinky, Ghost& inky, Ghost& blinky, Ghost& clyde)
{
    // checks to see if implicit ghost is in the same coordinate as a explicit ghost
    std::vector<Ghost> ghostList{pinky, inky, blinky, clyde};

    for(std::size_t i{0}; i < ghostList.size(); ++i)
    {
        if(m_ghostVec.y == ghostList[i].getGhostVec().y && m_ghostVec.x == ghostList[i].getGhostVec().x && 
            m_ghostColor != ghostList[i].getGhostColor())
            {
                return ghostList[i].getGhostColor();
            }
    }

    return Color::null;
}

void Ghost::printOverLap(Window& win, Color::ColorPair overLapColor)
{
    // prints ghost back if overlapped
    if(overLapColor != Color::null)
    {
        wattron(win.getWindow(), COLOR_PAIR(overLapColor));
        mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(overLapColor));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
    }
}

void Ghost::printPelletBack(Pellet& pellets, Window& win)
{
    const int garbage{10000};

    if(pellets.getPelletVec()[m_ghostVec.y][m_ghostVec.x] != garbage)
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::white_black));
        mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "•");
        wattroff(win.getWindow(), COLOR_PAIR(Color::white_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));    
    }
}

// public members:

Ghost::Ghost(std::chrono::milliseconds speed, Color::ColorPair ghostColor)
    : m_direction {Direction::right}
    , m_ghostVec {15, 12} // randomInput{},
    , m_interval{speed}
    , m_lastTime{std::chrono::high_resolution_clock::now()}
    , m_rightPortalX{ 26 }, m_leftPortalX{ 1 }
    , m_portalY{ 14 }, m_ghostColor{ ghostColor }
    {
    }

// refactor this ****
void Ghost::timeToMove(Window& win, std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter, std::vector<std::vector<int>>& windowArea, Ghost& pinky, Ghost& inky, Ghost& blinky, Ghost& clyde, Pellet& pellets)
{
    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto currentTime{std::chrono::high_resolution_clock::now()};

    if (currentTime - m_lastTime >= m_interval)
    {
        erase(win);
        printPelletBack(pellets, win);
        printOverLap(win, checkGhostOverLap(pinky, inky, blinky, clyde));
        setValidDirection(obstacleList, windowPerimeter, windowArea);
        printAndRefreshGhost(win);

        m_lastTime = currentTime;
    }
}

Color::ColorPair Ghost::getGhostColor() { return m_ghostColor; }

Vec Ghost::getGhostVec() { return m_ghostVec; }
