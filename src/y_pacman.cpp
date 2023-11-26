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
#include <fstream>

using namespace std::chrono_literals;

/********************************************************************** PUBLIC MEMBERS **********************************************************************/

// constructor
Pacman::Pacman(Window& win)
    : m_direction {Direction::right}
    , m_pacVec {23, 13}, m_userInput{}
    , m_interval{130ms}
    , m_lastTime{std::chrono::high_resolution_clock::now()}
    , m_rightPortalX{ 26 }, m_leftPortalX{ 1 }
    , m_portalY{ 14 }
    {
    }

// public methods
LevelState Pacman::timeToMove(Window& win, Ghost& clyde, Inky& inky, Blinky& blinky, Pinky& pinky, int& score, bool powerPelletActive, std::chrono::time_point<std::chrono::high_resolution_clock>& lastTime)
{
    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto currentTime{std::chrono::high_resolution_clock::now()};

    getUserInputAndSetDirection(win);
    if (currentTime - m_lastTime >= m_interval)
    {
        erase(win);
        updateEatenPelletsAndPrintScore(win, score);

        int count{0};
        for(std::size_t y{0}; y < win.getWindowArea().size(); ++y)
        {
            for(std::size_t x{0}; x < win.getWindowArea()[y].size(); ++x)
            {
                if(win.getWindowArea()[y][x] == CellName::pelletEaten || win.getWindowArea()[y][x] == CellName::powerPelletEaten)
                    ++count;
            }
        }

        if(count == 298)
            return LevelState::levelClear;

        GhostCollision collision{ movePacmanBasedOnDirection(win, clyde, inky, blinky, pinky, powerPelletActive) };

        // if pacman moved into ghost with power pellet
        if((collision == GhostCollision::blinky || collision == GhostCollision::clyde || collision == GhostCollision::inky || collision == GhostCollision::pinky) && powerPelletActive)
        {
            printPlusEatGhost(win);
            score += 20;
            printScore(win, score);
            wrefresh(win.getWindow());
            lastTime += 1100ms;
            std::this_thread::sleep_for(1s);
            m_lastTime = currentTime;

            if(collision == GhostCollision::blinky)         
                return LevelState::ateBlinky;
            else if(collision == GhostCollision::clyde)
                return LevelState::ateClyde;
            else if(collision == GhostCollision::inky)         
                return LevelState::ateInky;
            else if(collision == GhostCollision::pinky)
                return LevelState::atePinky;
        }
        else if((collision != GhostCollision::null))
        {
            printPacmanBasedOnDirection(win);
            wrefresh(win.getWindow());
            return LevelState::pacmanDead;
        }
        

        if(powerPelletCheck(win))
        {
            printPacmanBasedOnDirection(win);
            wrefresh(win.getWindow());
            m_lastTime = currentTime;
            return LevelState::powerUp;
        }

        // refresh pacman and update timer
        printPacmanBasedOnDirection(win);
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

void Pacman::printInitialPacman(Window& win)
{
    wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    mvwprintw(win.getWindow(), m_pacVec.y, m_pacVec.x, "<");
    wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));

    wrefresh(win.getWindow());
}

// getters
Vec Pacman::getPacVec() { return m_pacVec; }
void Pacman::setPacVec() { m_pacVec = Vec{23, 13}; }


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

GhostCollision Pacman::movePacmanBasedOnDirection(Window& win, Ghost& clyde, Inky& inky, Blinky& blinky, Pinky& pinky, bool powerPelletActive)
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

                    return ghostCollisionCheck(clyde, inky, blinky, pinky);
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

                    return ghostCollisionCheck(clyde, inky, blinky, pinky);
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

                    return ghostCollisionCheck(clyde, inky, blinky, pinky);
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

                    return ghostCollisionCheck(clyde, inky, blinky, pinky);
                }
                break;
            }
            default: 
                break;
        }

    return GhostCollision::null;
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

GhostCollision Pacman::ghostCollisionCheck(Ghost& clyde, Inky& inky, Blinky& blinky, Pinky& pinky)
{
    if(m_pacVec.y == clyde.getGhostVec().y && m_pacVec.x == clyde.getGhostVec().x)
        return GhostCollision::clyde;

    if(m_pacVec.y == inky.getInkyVec().y && m_pacVec.x == inky.getInkyVec().x)
        return GhostCollision::inky;

    if(m_pacVec.y == blinky.getBlinkyVec().y && m_pacVec.x == blinky.getBlinkyVec().x)
        return GhostCollision::blinky;

    if(m_pacVec.y == pinky.getPinkyVec().y && m_pacVec.x == pinky.getPinkyVec().x)
        return GhostCollision::pinky;

    return GhostCollision::null;
}

bool Pacman::powerPelletCheck(Window& win)
{
    if(win.getWindowArea()[m_pacVec.y][m_pacVec.x] == CellName::powerPellet)
    {
        win.getWindowArea()[m_pacVec.y][m_pacVec.x] = CellName::powerPelletEaten;
        return true;
    }
    else
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

void Pacman::printPlusEatGhost(Window& win)
{
    wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    mvwprintw(win.getWindow(), m_pacVec.y, m_pacVec.x, "+");
    wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
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
    if(win.getWindowArea()[m_pacVec.y][m_pacVec.x] != CellName::pelletEaten && win.getWindowArea()[m_pacVec.y][m_pacVec.x] != CellName::powerPelletEaten)
    {
        ++score;
        win.getWindowArea()[m_pacVec.y][m_pacVec.x] = CellName::pelletEaten;
    }

    printScore(win, score);
}

void Pacman::printScore(Window& win, int score)
{
    mvwprintw(win.getWindow(), 11, 23, "    ");
    mvwprintw(win.getWindow(), 10, 23, "    ");

    wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    mvwprintw(win.getWindow(), 10, 23, "PNTS");
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