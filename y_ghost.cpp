//external
#include <ncurses.h>

//user
#include "z_aggregate.h"
#include "z_window.h"
#include "z_ghost.h"
#include "z_obstacle.h"
#include "z_random.h"
#include "z_pellet.h"

//std
#include <chrono>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

/********************************************************************** PRIVATE MEMBERS **********************************************************************/

// constructor
Ghost::Ghost(std::chrono::milliseconds speed, Color::ColorPair ghostColor)
    : m_direction {Direction::right}
    , m_ghostVec {15, 12} // randomInput{},
    , m_interval{speed}
    , m_lastTime{std::chrono::high_resolution_clock::now()}
    , m_rightPortalX{ 26 }, m_leftPortalX{ 1 }
    , m_portalY{ 14 }, m_ghostColor{ ghostColor }
    {
    }

// public methods
void Ghost::timeToMove(Window& win, std::vector<Obstacle>& obstacleList, Ghost& g1, Ghost& g2, Ghost& g3)
{
    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto currentTime{std::chrono::high_resolution_clock::now()};

    if (currentTime - m_lastTime >= m_interval)
    {
        eraseLastPosition(win);
        CheckForAndPrintOverLaps(win, g1, g2, g3);
        setValidDirection(obstacleList, win);
        printAndRefreshGhost(win);

        m_lastTime = currentTime;
    }
}

// getters
Color::ColorPair Ghost::getGhostColor() { return m_ghostColor; }
Vec Ghost::getGhostVec() { return m_ghostVec; }


/********************************************************************** PRIVATE MEMBERS **********************************************************************/

/* _   _   _   _   _   _   _     _   _   _   _   _   _   _   _   _  
  / \ / \ / \ / \ / \ / \ / \   / \ / \ / \ / \ / \ / \ / \ / \ / \   Functions related to checking ghost's location and either using it for printing or
 ( S | E | T | T | I | N | G ) ( D | I | R | E | C | T | I | O | N )  reverting it and trying again until a valid input is found.
  \_/ \_/ \_/ \_/ \_/ \_/ \_/   \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/   
*/

void Ghost::setValidDirection(std::vector<Obstacle>& obstacleList, Window& win)  
{
    // sets value to current direction to check if new random value is the direction ghost came from
    Direction directionCheck{ m_direction };

    while(true)
    {
        // Ghost cant move in direction he came
        if(oppositeDirectionCheck(directionCheck))
            continue;

        // breaks from loop if valid direction to use in movement and printing
        if(moveGhostInValidDirection(obstacleList, win))
            break;
    }
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
}

bool Ghost::moveGhostInValidDirection(std::vector<Obstacle>& obstacleList, Window& win)
{
    // attempt a move, check if valid, and revert the move if its not
    // delegates checks to obstacle and perimeter bounds check
    switch(m_direction)
        {
            case Direction::up: 
            {
                setY(PositionChange::decrement);
                if(!obstacleBoundsCheck(win) || !perimeterBoundsCheck(win))
                {
                    setY(PositionChange::increment);
                    return false;
                }
                return true;
            }
            case Direction::down: 
            {

                setY(PositionChange::increment);
                if(!obstacleBoundsCheck(win) || !perimeterBoundsCheck(win))
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
                if(!obstacleBoundsCheck(win) || !perimeterBoundsCheck(win))
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
                if(!obstacleBoundsCheck(win) || !perimeterBoundsCheck(win))
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

void Ghost::setY(PositionChange direction) 
{
    if (direction == PositionChange::decrement) 
        --m_ghostVec.y;
    else
        ++m_ghostVec.y;
}

void Ghost::setX(PositionChange direction)
{
    if (direction == PositionChange::decrement)
        --m_ghostVec.x;
    else
        ++m_ghostVec.x;
}

bool Ghost::obstacleBoundsCheck(Window& win)
{
    if(win.getWindowArea()[m_ghostVec.y][m_ghostVec.x] == CellName::obstacleValue)
        return false;

    return true;
}

bool Ghost::perimeterBoundsCheck(Window& win)
{
    if(win.getWindowArea()[m_ghostVec.y][m_ghostVec.x] == CellName::perimeterValue)
        return false;

    return true;
}

/* _   _   _   _   _   _   _   _  
  / \ / \ / \ / \ / \ / \ / \ / \  Functions related to erasing old positions, and printing new positions. Also deals with ghost 
 ( P | R | I | N | T | I | N | G ) overlaps which cause print overs which is where a ghost will overlap with another and when it moves it will 
  \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/  erase the other ghost causing the ghost to dissapear. Also deals with spawning pellets back after a ghost clears them                              
*/

void Ghost::eraseLastPosition(Window& win)
{
    mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, " "); 
}

// deals with overlaps (pellets && ghost)
void Ghost::CheckForAndPrintOverLaps(Window& win, Ghost& g1, Ghost& g2, Ghost& g3)
{
    printPelletBackIfNotEaten(win);
    printOverLap(win, checkGhostOverLap(g1, g2, g3));
}

void Ghost::printPelletBackIfNotEaten(Window& win)
{
    if(win.getWindowArea()[m_ghostVec.y][m_ghostVec.x] != CellName::pelletEaten)
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::white_black));
        mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "•");
        wattroff(win.getWindow(), COLOR_PAIR(Color::white_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));    
    }
}

Color::ColorPair Ghost::checkGhostOverLap(Ghost& g1, Ghost& g2, Ghost& g3)
{
    // checks to see if implicit ghost is in the same coordinate as a explicit ghost
    std::vector<Ghost> ghostList{g1, g2, g3};

    for(std::size_t i{0}; i < ghostList.size(); ++i)
    {
        if(m_ghostVec.y == ghostList[i].getGhostVec().y && m_ghostVec.x == ghostList[i].getGhostVec().x)
                return ghostList[i].getGhostColor();
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

void Ghost::printAndRefreshGhost(Window& win)
{
    wattron(win.getWindow(), COLOR_PAIR(m_ghostColor));
    mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "ᗣ");
    wattroff(win.getWindow(), COLOR_PAIR(m_ghostColor));
    wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));

    wrefresh(win.getWindow());
}