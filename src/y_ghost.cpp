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
#include <algorithm>
#include <queue>

using namespace std::chrono_literals;

/********************************************************************** PUBLIC MEMBERS **********************************************************************/

// constructor
Ghost::Ghost()
    : m_direction {Direction::right}
    , m_ghostVec {15, 13} // randomInput{},
    , m_interval{165ms}
    , m_lastTime{std::chrono::high_resolution_clock::now()}
    , m_rightPortalX{ 26 }, m_leftPortalX{ 1 }
    , m_portalY{ 14 }, m_ghostColor{ Color::orange_black }
    {
    }

// public methods
bool Ghost::timeToMove(Window& win, Pinky& pinky, Inky& inky, Blinky& blinky, Pacman& pacman, bool powerPelletTimer, LevelState ateWhichGhost, int& score, std::chrono::time_point<std::chrono::high_resolution_clock>& lastTime, std::chrono::milliseconds& interval)
{
    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto currentTime{std::chrono::high_resolution_clock::now()};

    // if power pellet not active
    if(!powerPelletTimer)
    {
        if (currentTime - m_lastTime >= m_interval)
        {
            eraseLastPosition(win);
            checkForAndPrintOverLaps(win, pinky, inky, blinky, false);

            if(!setValidDirection(win, pacman, false))
                return false;

            printAndRefreshGhost(win, false);

            m_lastTime = currentTime;
        }

        return true;
    }
    else // if power pellet is active
    {
        auto elapsedTime{ currentTime - lastTime };
        auto remainingTime{ interval - elapsedTime };

        if(remainingTime <= 250ms)
        {
            wattron(win.getWindow(), COLOR_PAIR(Color::blue_black));
            mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "ᗣ");
            wattroff(win.getWindow(), COLOR_PAIR(Color::blue_black));
            wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));

            wrefresh(win.getWindow());
        }
        else if(remainingTime <= 500ms)
        {
            wattron(win.getWindow(), COLOR_PAIR(Color::white_black));
            mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "ᗣ");
            wattroff(win.getWindow(), COLOR_PAIR(Color::white_black));
            wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));

            wrefresh(win.getWindow());
        }
        else if(remainingTime <= 750ms)
        {
            wattron(win.getWindow(), COLOR_PAIR(Color::blue_black));
            mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "ᗣ");
            wattroff(win.getWindow(), COLOR_PAIR(Color::blue_black));
            wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));

            wrefresh(win.getWindow());
        }
        else if(remainingTime <= 1000ms)
        {
            wattron(win.getWindow(), COLOR_PAIR(Color::white_black));
            mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "ᗣ");
            wattroff(win.getWindow(), COLOR_PAIR(Color::white_black));
            wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));
        
            wrefresh(win.getWindow());
        }
        else if(remainingTime <= 1250ms)
        {
            wattron(win.getWindow(), COLOR_PAIR(Color::blue_black));
            mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "ᗣ");
            wattroff(win.getWindow(), COLOR_PAIR(Color::blue_black));
            wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));

            wrefresh(win.getWindow());
        }
        else if(remainingTime <= 1500ms)
        {
            wattron(win.getWindow(), COLOR_PAIR(Color::white_black));
            mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "ᗣ");
            wattroff(win.getWindow(), COLOR_PAIR(Color::white_black));
            wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));

            wrefresh(win.getWindow());
        }

        m_ghostIntervalStorage = m_interval;
        m_interval = 350ms;

        if(ateWhichGhost == LevelState::ateClyde)
        {
            m_ghostVec.y = 15;
            m_ghostVec.x = 13;
        }

        if(currentTime - m_lastTime >= m_interval)
        {
            eraseLastPosition(win);
            checkForAndPrintOverLaps(win, pinky, inky, blinky, true);
            
           if(!setValidDirection(win, pacman, true))
            {
                wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));
                mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "+");
                wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
                wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
                wrefresh(win.getWindow());
                lastTime += 1100ms;
                std::this_thread::sleep_for(1s);

                score += 20;
                m_ghostVec.y = 15;
                m_ghostVec.x = 13;
            }

            printAndRefreshGhost(win, true);

            m_lastTime = currentTime;
        }

        m_interval = m_ghostIntervalStorage;

        return true;
    }
}

void Ghost::printGhost(Window& win)
{
    wattron(win.getWindow(), COLOR_PAIR(Color::orange_black));
    mvwprintw(win.getWindow(), 15, 13, "ᗣ");
    wattroff(win.getWindow(), COLOR_PAIR(Color::orange_black));
    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
}

// getters
Color::ColorPair Ghost::getGhostColor() { return m_ghostColor; }
Vec Ghost::getGhostVec() { return m_ghostVec; }

void Ghost::setSpeed() { m_interval -= 10ms; }

void Ghost::setGhostVec() { m_ghostVec = Vec{15, 13}; }


/********************************************************************** PRIVATE MEMBERS **********************************************************************/

/* _   _   _   _   _   _   _     _   _   _   _   _   _   _   _   _  
  / \ / \ / \ / \ / \ / \ / \   / \ / \ / \ / \ / \ / \ / \ / \ / \   Functions related to checking ghost's location and either using it for printing or
 ( S | E | T | T | I | N | G ) ( D | I | R | E | C | T | I | O | N )  reverting it and trying again until a valid input is found.
  \_/ \_/ \_/ \_/ \_/ \_/ \_/   \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/   
*/

bool Ghost::setValidDirection(Window& win, Pacman& pacman, bool powerPelletTimer)  
{
    if(!powerPelletTimer)
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
    else
    {
        std::vector<Vec> ghostPath{};

        if(m_ghostVec.y != 3 || m_ghostVec.x != 26)
        {
            ghostPath = createGhostPath(Vec{m_ghostVec.y, m_ghostVec.x}, win, Vec{3, 26});
            m_ghostVec.y = ghostPath[1].y;
            m_ghostVec.x = ghostPath[1].x;
        }
        else
        {
            ghostPath = createGhostPath(Vec{m_ghostVec.y, m_ghostVec.x}, win, Vec{5, 26});
            m_ghostVec.y = ghostPath[1].y;
            m_ghostVec.x = ghostPath[1].x;
        }

        if(m_ghostVec.y == pacman.getPacVec().y && m_ghostVec.x == pacman.getPacVec().x)
            return false;
        else
            return true;
    }
}

std::vector<Vec> Ghost::createGhostPath(Vec start, Window& win, Vec target)
{
    auto rows{win.getWindowArea().size()};
    auto cols{win.getWindowArea()[0].size()};

    std::vector<std::vector<int>> visited(rows, std::vector<int>(cols, 0));
    std::vector<std::vector<Vec>> parentCell(rows, std::vector<Vec>(cols));

    std::queue<Vec> q{};
    q.push(start);                  // push in start point to q
    visited[start.y][start.x] = 1;  // assign start point visited

    while(!q.empty())
    {
        Vec current{ q.front() };  // assigns new variable "current" to element first out.
        q.pop();                          // removes element at front of q

        // Directions: up, down, left, right
        std::vector<Vec> directions = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};

        for(const auto& dir : directions)
        {
            Vec next{current.y + dir.y, current.x + dir.x};

            //if(next.y >= 0 && next.y < rows && next.x >= 0 && next.x < cols && !visited[next.y][next.x]) 
            if(next.y > 0 && next.y < (rows - 1) && next.x > 0 && next.x < (cols - 1) && win.getWindowArea()[next.y][next.x] != CellName::obstacleValue && 
                win.getWindowArea()[next.y][next.x] != CellName::perimeterValue && !visited[next.y][next.x])
            {
                q.push(next);
                visited[next.y][next.x] = 1;
                parentCell[next.y][next.x] = current;
            }
        }
    }

    std::vector<Vec> path{};
    Vec current{target};
    while(current.y != start.y || current.x != start.x)
    {
        path.push_back(current);
        current = parentCell[current.y][current.x];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    
    return path;
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
void Ghost::checkForAndPrintOverLaps(Window& win, Pinky& pinky, Inky& inky, Blinky& blinky, bool powerPelletTimer)
{
    printPelletBackIfNotEaten(win);
    printOverLap(win, checkGhostOverLap(pinky, inky, blinky), powerPelletTimer);
}

void Ghost::printPelletBackIfNotEaten(Window& win)
{
    if(win.getWindowArea()[m_ghostVec.y][m_ghostVec.x] != CellName::pelletEaten && win.getWindowArea()[m_ghostVec.y][m_ghostVec.x] != CellName::ghostBox &&
        win.getWindowArea()[m_ghostVec.y][m_ghostVec.x] != CellName::perimeterValue && win.getWindowArea()[m_ghostVec.y][m_ghostVec.x] != CellName::powerPelletEaten)
    {
        if(win.getWindowArea()[m_ghostVec.y][m_ghostVec.x] == CellName::powerPellet)
        {
            wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));
            wattron(win.getWindow(), A_BLINK);
            mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "⬤");
            wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
            wattroff(win.getWindow(), A_BLINK);
            wattron(win.getWindow(), COLOR_PAIR(Color::default_color));  
        }
        else
        {
        wattron(win.getWindow(), COLOR_PAIR(Color::white_black));
        mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "•");
        wattroff(win.getWindow(), COLOR_PAIR(Color::white_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));    
        }
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

void Ghost::printOverLap(Window& win, Color::ColorPair overLapColor, bool powerPelletTimer)
{
    // prints ghost back if overlapped
    if(overLapColor != Color::null)
    {
        wattron(win.getWindow(), COLOR_PAIR(overLapColor));
        mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(overLapColor));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
    }

    // prints ghost back if overlapped
    if(powerPelletTimer && overLapColor != Color::null)
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::blue_black));
        mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(Color::blue_black));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color)); 
    }
    else if(overLapColor != Color::null)
    {
        wattron(win.getWindow(), COLOR_PAIR(overLapColor));
        mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(overLapColor));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
    }
}

void Ghost::printAndRefreshGhost(Window& win, bool powerPelletActive)
{
    if(!powerPelletActive)
    {
        wattron(win.getWindow(), COLOR_PAIR(m_ghostColor));
        mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(m_ghostColor));
        wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));
    }
    else
    {
        wattron(win.getWindow(), COLOR_PAIR(Color::blue_black));
        mvwprintw(win.getWindow(), m_ghostVec.y, m_ghostVec.x, "ᗣ");
        wattroff(win.getWindow(), COLOR_PAIR(Color::blue_black));
        wattroff(win.getWindow(), COLOR_PAIR(Color::default_color));
    }

    wrefresh(win.getWindow());
}