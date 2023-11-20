//external
#include <ncurses.h>

//user
#include "z_aggregate.h"
#include "z_vectorspace.h"
#include "z_window.h"
#include "z_pacman.h"
#include "z_obstacle.h"

//std
#include <vector>
#include <chrono>
#include <thread>
#include <string>

using namespace std::chrono_literals;

/********************************************************************** PUBLIC MEMBERS **********************************************************************/

// constructor
Pacman::Pacman()
    : m_direction {Direction::right}
    , m_pacVec {1, 1}, m_userInput{}
    , m_interval{175ms}
    , m_lastTime{std::chrono::high_resolution_clock::now()}
    , m_rightPortalX{ 26 }, m_leftPortalX{ 1 }
    , m_portalY{ 14 }
    {
    }

// public methods
void Pacman::timeToMove(Window& win)
{
    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto currentTime{std::chrono::high_resolution_clock::now()};

    getUserInputAndSetDirection(win);
    if (currentTime - m_lastTime >= m_interval)
    {
        erase(win);
        updateEatenPellets(win);
        printScore(win);
        movePacmanBasedOnDirection(win);
        printPacmanBasedOnDirection(win);

        // refresh pacman and update timer
        wrefresh(win.getWindow());
        m_lastTime = currentTime;
    }
}

/********************************************************************** PRIVATE MEMBERS **********************************************************************/

/* _   _   _   _   _   _   _     _   _   _   _   _   _   _   _   _  
  / \ / \ / \ / \ / \ / \ / \   / \ / \ / \ / \ / \ / \ / \ / \ / \   Functions related to checking pacman's location and either using it for printing or
 ( S | E | T | T | I | N | G ) ( D | I | R | E | C | T | I | O | N )  reverting it so pacman doesnt move into unplayable areas.
  \_/ \_/ \_/ \_/ \_/ \_/ \_/   \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/   
*/

void Pacman::getUserInputAndSetDirection(Window& win)
{
    m_userInput = wgetch(win.getWindow());

    if (m_userInput != ERR)
        setDirection();

    // clears extra input as to not cause input buffer stacking
    while (wgetch(win.getWindow()) != ERR) {}
}

void Pacman::setDirection() 
{
    switch(m_userInput)
    {
        case 'w': m_direction = Direction::up; break;
        case 'a': m_direction = Direction::left; break;
        case 's': m_direction = Direction::down; break;
        case 'd': m_direction = Direction::right; break;
        default: break;
    }
}

void Pacman::movePacmanBasedOnDirection(Window& win)
{
    switch(m_direction)
        {
            case Direction::up: 
            {
                if(m_pacVec.y != 1)
                {
                    setY(PositionChange::decrement);
                    if(obstacleBoundsCheck(win))
                        setY(PositionChange::increment);
                }
                break;
            }
            case Direction::down: 
            {
                if(m_pacVec.y != (win.getScreenY() - 2))
                {
                    setY(PositionChange::increment);
                    if(obstacleBoundsCheck(win))
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
                    if(obstacleBoundsCheck(win))
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
                    if(obstacleBoundsCheck(win))
                        setX(PositionChange::decrement);
                }
                break;
            }
            default: 
                break;
        }
}

void Pacman::setY(PositionChange direction) 
{
    if (direction == PositionChange::decrement) 
        --m_pacVec.y;
    else
        ++m_pacVec.y;
}

void Pacman::setX(PositionChange direction)
{
    if (direction == PositionChange::decrement)
        --m_pacVec.x;
    else
        ++m_pacVec.x;
}

bool Pacman::obstacleBoundsCheck(Window& win)
{
    if(win.getWindowArea()[m_pacVec.y][m_pacVec.x] == CellName::obstacleValue)
        return true;

    return false;
}

/* _   _   _   _   _   _   _   _  
  / \ / \ / \ / \ / \ / \ / \ / \   Functions related to printing pacman's new position as well as incrementing and 
 ( P | R | I | N | T | I | N | G )  printing the score
  \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/                                
*/

void Pacman::erase(Window& win)
{
    mvwprintw(win.getWindow(), m_pacVec.y, m_pacVec.x, " ");
}

void Pacman::updateEatenPellets(Window& win)
{
    if(win.getWindowArea()[m_pacVec.y][m_pacVec.x] != CellName::pelletEaten)
    {
        ++m_score;
        win.getWindowArea()[m_pacVec.y][m_pacVec.x] = CellName::pelletEaten;
    }
}

void Pacman::printPacmanBasedOnDirection(Window& win)
{
    std::string setString{};

    switch(m_direction)
    {
        case Direction::up:
            setString = "v";
            break;
        case Direction::down:
            setString = "ʌ";
            break;
        case Direction::left:
            setString = ">";
            break;
        case Direction::right:
            setString = "<";
            break;
        default:
            break;
    }

    wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    mvwprintw(win.getWindow(), m_pacVec.y, m_pacVec.x, "%s", setString.c_str());
    wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
}

void Pacman::printScore(Window& win)
{
    mvwprintw(win.getWindow(), 11, 23, "    ");
    mvwprintw(win.getWindow(), 10, 23, "    ");

    wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    mvwprintw(win.getWindow(), 10, 23, "SCRE");
    wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));

    mvwprintw(win.getWindow(), 12, 23, "    ");

    // cyan
    wattron(win.getWindow(), COLOR_PAIR(Color::cyan_black));
    mvwprintw(win.getWindow(), 11, 23, "ᗣ");
    wattroff(win.getWindow(), COLOR_PAIR(Color::cyan_black));
    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));

    // orange
    wattron(win.getWindow(), COLOR_PAIR(Color::orange_black));
    mvwprintw(win.getWindow(), 11, 24, "ᗣ");
    wattroff(win.getWindow(), COLOR_PAIR(Color::orange_black));
    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));

    // red
    wattron(win.getWindow(), COLOR_PAIR(Color::red_black));
    mvwprintw(win.getWindow(), 11, 25, "ᗣ");
    wattroff(win.getWindow(), COLOR_PAIR(Color::red_black));
    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));

    // pink
    wattron(win.getWindow(), COLOR_PAIR(Color::pink_black));
    mvwprintw(win.getWindow(), 11, 26, "ᗣ");
    wattroff(win.getWindow(), COLOR_PAIR(Color::pink_black));
    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));

    wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    mvwprintw(win.getWindow(), 12, 23, "%d", m_score);
    wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
}