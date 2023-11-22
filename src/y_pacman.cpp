//external
#include <ncurses.h>

//user
#include "z_aggregate.h"
#include "z_vectorspace.h"
#include "z_window.h"
#include "z_pacman.h"
#include "z_obstacle.h"
#include "z_ghost.h"
#include "z_inky.h"
#include "z_pinky.h"
#include "z_blinky.h"

//std
#include <vector>
#include <chrono>
#include <thread>
#include <string>

using namespace std::chrono_literals;

/********************************************************************** PUBLIC MEMBERS **********************************************************************/

// constructor
Pacman::Pacman(Window& win)
    : m_direction {Direction::right}
    , m_pacVec {1, 1}, m_userInput{}
    , m_interval{175ms}
    , m_lastTime{std::chrono::high_resolution_clock::now()}
    , m_rightPortalX{ 26 }, m_leftPortalX{ 1 }
    , m_portalY{ 14 }
    {
        printInitialPacman(win);
    }

// public methods
LevelState Pacman::timeToMove(Window& win, Ghost& clyde, Inky& inky, Blinky& blinky, Pinky& pinky, int& score)
{
    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto currentTime{std::chrono::high_resolution_clock::now()};

    getUserInputAndSetDirection(win);
    if (currentTime - m_lastTime >= m_interval)
    {
        erase(win);
        updateEatenPelletsAndPrintScore(win, score);

        if(score % 298 == 0)
            return LevelState::levelClear;

        // if pacman moved into ghost return true
        if(movePacmanBasedOnDirection(win, clyde, inky, blinky, pinky))
        {
            printPacmanBasedOnDirection(win);
            wrefresh(win.getWindow());
            return LevelState::pacmanDead;
        }

        printPacmanBasedOnDirection(win);

        // refresh pacman and update timer
        wrefresh(win.getWindow());
        m_lastTime = currentTime;
    }

    return LevelState::normalMovement;
}

void Pacman::printDeathAnimation(Window& win)
{    
    std::vector<std::string> animationFrames{"v", "—", "ʌ", "|", "*", " "};
    int subscript{-1};

    auto interval{600ms};
    auto lastTime{ std::chrono::high_resolution_clock::now() };
    while(true)
    {
        auto currentTime{ std::chrono::high_resolution_clock::now() };

        if(currentTime - lastTime >= interval)
        {
            ++subscript;

            wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));
            mvwprintw(win.getWindow(), m_pacVec.y, m_pacVec.x, "%s", animationFrames[subscript].c_str());
            wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
            wattron(win.getWindow(), COLOR_PAIR(Color::default_color));

            wrefresh(win.getWindow());

            lastTime = currentTime;
        }

        if(subscript == 5)
            break;
    }
}

// getters
Vec Pacman::getPacVec() { return m_pacVec; }


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

bool Pacman::movePacmanBasedOnDirection(Window& win, Ghost& clyde, Inky& inky, Blinky& blinky, Pinky& pinky)
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

                    if(ghostCollisionCheck(clyde, inky, blinky, pinky))
                        return true;
                }
                break;
            }
            case Direction::down: 
            {
                if(m_pacVec.y != (win.getScreenY() - 2))
                {
                    setY(PositionChange::increment);

                    if(m_pacVec.y == 12 && (m_pacVec.x == 13 || m_pacVec.x == 14))
                        setY(PositionChange::decrement);

                    if(obstacleBoundsCheck(win))
                        setY(PositionChange::decrement);

                    if(ghostCollisionCheck(clyde, inky, blinky, pinky))
                        return true;
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

                    if(ghostCollisionCheck(clyde, inky, blinky, pinky))
                        return true;
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

                    if(ghostCollisionCheck(clyde, inky, blinky, pinky))
                        return true;
                }
                break;
            }
            default: 
                break;
        }

    return false;
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

bool Pacman::ghostCollisionCheck(Ghost& clyde, Inky& inky, Blinky& blinky, Pinky& pinky)
{
    if(m_pacVec.y == clyde.getGhostVec().y && m_pacVec.x == clyde.getGhostVec().x)
        return true;

    if(m_pacVec.y == inky.getInkyVec().y && m_pacVec.x == inky.getInkyVec().x)
        return true;

    if(m_pacVec.y == blinky.getBlinkyVec().y && m_pacVec.x == blinky.getBlinkyVec().x)
        return true;

    if(m_pacVec.y == pinky.getPinkyVec().y && m_pacVec.x == pinky.getPinkyVec().x)
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

void Pacman::updateEatenPelletsAndPrintScore(Window& win, int& score)
{
    if(win.getWindowArea()[m_pacVec.y][m_pacVec.x] != CellName::pelletEaten)
    {
        ++score;
        win.getWindowArea()[m_pacVec.y][m_pacVec.x] = CellName::pelletEaten;
    }

    printScore(win, score);
}

void Pacman::printInitialPacman(Window& win)
{
    wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    mvwprintw(win.getWindow(), m_pacVec.y, m_pacVec.x, "<");
    wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));

    wrefresh(win.getWindow());
}

void Pacman::printScore(Window& win, int score)
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
    mvwprintw(win.getWindow(), 12, 23, "%d", score);
    wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
}