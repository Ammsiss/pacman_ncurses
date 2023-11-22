//external
#include <ncurses.h>

//user
#include "z_aggregate.h"
#include "z_window.h"
#include "z_ghost.h"
#include "z_obstacle.h"
#include "z_random.h"
#include "z_pacman.h"
#include "z_pinky.h"
#include "z_inky.h"
#include "z_blinky.h"

//std
#include <chrono>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

/********************************************************************** PUBLIC MEMBERS **********************************************************************/

// constructor
Ghost::Ghost()
    : m_direction {Direction::right}
    , m_ghostVec {15, 12} // randomInput{},
    , m_interval{275ms}
    , m_lastTime{std::chrono::high_resolution_clock::now()}
    , m_rightPortalX{ 26 }, m_leftPortalX{ 1 }
    , m_portalY{ 14 }, m_ghostColor{ Color::orange_black }
    {
    }

// public methods
bool Ghost::timeToMove(Window& win, Pinky& pinky, Inky& inky, Blinky& blinky, Pacman& pacman)
{
    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto currentTime{std::chrono::high_resolution_clock::now()};

    if (currentTime - m_lastTime >= m_interval)
    {
        eraseLastPosition(win);
        CheckForAndPrintOverLaps(win, pinky, inky, blinky);

        if(!setValidDirection(win, pacman))
            return false;

        printAndRefreshGhost(win);

        m_lastTime = currentTime;
    }

    return true;
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

bool Ghost::setValidDirection(Window& win, Pacman& pacman)  
{
    // sets value to current direction to check if new random value is the direction ghost came from
    Direction directionCheck{ m_direction };

    while(true)
    {
        // Ghost cant move in direction he came
        if(oppositeDirectionCheck(directionCheck))
            continue;

        // continues loop if invalid direction
        switch(moveGhostInValidDirection(win, pacman))
        {
            case GhostState::validDirection:
                return true;
            case GhostState::insidePacman:
                return false;
        }
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


GhostState Ghost::moveGhostInValidDirection(Window& win, Pacman& pacman)
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
                    return GhostState::invalidDirection;
                }
                else if(!pacmanCollisionCheck(pacman))
                    return GhostState::insidePacman;
                else
                    return GhostState::validDirection;
            }
            case Direction::down: 
            {
                setY(PositionChange::increment);

                if(!obstacleBoundsCheck(win) || !perimeterBoundsCheck(win))
                {
                   setY(PositionChange::decrement);
                   return GhostState::invalidDirection;
                }
                else if(m_ghostVec.y == 12 && (m_ghostVec.x == 13 || m_ghostVec.x == 14))
                {
                    setY(PositionChange::decrement);
                    return GhostState::invalidDirection;
                }
                else if(!pacmanCollisionCheck(pacman))
                    return GhostState::insidePacman;
                else
                    return GhostState::validDirection;
            }
            case Direction::left: 
            {
                if(m_ghostVec.x == m_leftPortalX && m_ghostVec.y == m_portalY)
                {
                    // L -> R
                    m_ghostVec.x = m_rightPortalX;
                    return GhostState::validDirection;
                }
                setX(PositionChange::decrement); 
                if(!obstacleBoundsCheck(win) || !perimeterBoundsCheck(win))
                {
                    setX(PositionChange::increment);
                    return GhostState::invalidDirection;
                }
                else if(!pacmanCollisionCheck(pacman))
                    return GhostState::insidePacman;
                else
                    return GhostState::validDirection;
            }
            case Direction::right: 
            {
                if(m_ghostVec.x == m_rightPortalX && m_ghostVec.y == m_portalY)
                {
                    // R -> L 
                    m_ghostVec.x = m_leftPortalX;
                    return GhostState::validDirection;
                }
                setX(PositionChange::increment); 
                if(!obstacleBoundsCheck(win) || !perimeterBoundsCheck(win))
                {
                    setX(PositionChange::decrement);
                    return GhostState::invalidDirection;
                }
                else if(!pacmanCollisionCheck(pacman))
                    return GhostState::insidePacman;
                else
                    return GhostState::validDirection;
            }
            default: 
                break;
        }

    return GhostState::invalidDirection;
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

bool Ghost::pacmanCollisionCheck(Pacman& pacman)
{
    if(m_ghostVec.y == pacman.getPacVec().y && m_ghostVec.x == pacman.getPacVec().x)
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
void Ghost::CheckForAndPrintOverLaps(Window& win, Pinky& pinky, Inky& inky, Blinky& blinky)
{
    printPelletBackIfNotEaten(win);
    printOverLap(win, checkGhostOverLap(pinky, inky, blinky));
}

void Ghost::printPelletBackIfNotEaten(Window& win)
{
    if(win.getWindowArea()[m_ghostVec.y][m_ghostVec.x] != CellName::pelletEaten && win.getWindowArea()[m_ghostVec.y][m_ghostVec.x] != CellName::ghostBox)
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::white_black));
        mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "•");
        wattroff(win.getWindow(), COLOR_PAIR(Color::white_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));    
    }
}

Color::ColorPair Ghost::checkGhostOverLap(Pinky& pinky, Inky& inky, Blinky& blinky)
{
    if(m_ghostVec.y == pinky.getPinkyVec().y && m_ghostVec.x == pinky.getPinkyVec().x)
        return pinky.getPinkyColor();

    if(m_ghostVec.y == inky.getInkyVec().y && m_ghostVec.x == inky.getInkyVec().x)
        return inky.getInkyColor();        

    if(m_ghostVec.y == blinky.getBlinkyVec().y && m_ghostVec.x == blinky.getBlinkyVec().x)
        return blinky.getBlinkyColor();  

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