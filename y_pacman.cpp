#include <ncurses.h>
#include <vector>

#include "z_aggregate.h"
#include "z_window.h"
#include "z_pacman.h"
#include "z_obstacle.h"

// private members:

void Pacman::setY(DirectionEval direction) 
{
    if (direction == DirectionEval::decrement) --m_pacVec.y;
    else if (direction == DirectionEval::increment) ++m_pacVec.y;
}

void Pacman::setX(DirectionEval direction)
{
    if (direction == DirectionEval::decrement) --m_pacVec.x;
    else if (direction == DirectionEval::increment) ++m_pacVec.x;
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

// public members:

Pacman::Pacman()
    : m_direction {Direction::right}
    , m_pacVec {1, 1}, m_pacman{'@'}
    , m_userInput{}
    {
    }

void Pacman::printAndRefresh(Window& win)
{
    mvwaddch(win.getWindow(), m_pacVec.y, m_pacVec.x, m_pacman);
    wrefresh(win.getWindow());
}

void Pacman::erase(Window& win)
{
    mvwprintw(win.getWindow(), m_pacVec.y, m_pacVec.x, " ");
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

void Pacman::movePacmanBasedOnDirection(Window& win, std::vector<Obstacle>& obstacleList)
{
    switch(m_direction)
        {
            case Direction::up: 
            {
                if(m_pacVec.y != 1)
                {
                    setY(DirectionEval::decrement);
                    if(obstacleBoundsCheck(obstacleList))
                        setY(DirectionEval::increment);
                }
                break;
            }
            case Direction::down: 
            {
                if(m_pacVec.y != (win.getScreenY() - 2))
                {
                    setY(DirectionEval::increment);
                    if(obstacleBoundsCheck(obstacleList))
                        setY(DirectionEval::decrement);
                }
                break;
            }
            case Direction::left: 
            {
                if(m_pacVec.x != 1)
                {
                    setX(DirectionEval::decrement); 
                    if(obstacleBoundsCheck(obstacleList))
                        setX(DirectionEval::increment);
                }
                break;
            }
            case Direction::right: 
            {
                if(m_pacVec.x != (win.getScreenX() - 2))
                {
                    setX(DirectionEval::increment); 
                    if(obstacleBoundsCheck(obstacleList))
                        setX(DirectionEval::decrement);
                }
                break;
            }
            default: 
                break;
        }
}
