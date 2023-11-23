#pragma once

// external
#include <ncurses.h>

// user
#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"
#include "z_vectorspace.h"
#include "z_ghost.h"
#include "z_random.h"

// std
#include <vector>
#include <chrono>

class Ghost;
class Inky;
class Blinky;
class Pinky;
class Pacman
{
public:
    // constructor
    Pacman(Window& win);
    // public method
    LevelState timeToMove(Window& win, Ghost& clyde, Inky& inky, Blinky& blinky, Pinky& pinky, int& score, bool powerPelletActive);
    void printDeathAnimation(Window& win);
    void printInitialPacman(Window& win);

    // getter
    Vec getPacVec();
    void setPacVec();

private:
    // private variables

    // pacman attributes
    Direction m_direction;
    Vec m_pacVec;
    char m_userInput;

    // portal coordinates
    int m_rightPortalX;
    int m_leftPortalX;
    int m_portalY;

    // timing
    std::chrono::milliseconds m_interval;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;

    // printing
    void erase(Window& win);
    void updateEatenPelletsAndPrintScore(Window& win, int& score);
    void printPacmanBasedOnDirection(Window& win);
    void printScore(Window& win, int score);

    // setting direction
    void setDirection();
    GhostCollision movePacmanBasedOnDirection(Window& win, Ghost& clyde, Inky& inky, Blinky& blinky, Pinky& pinky, bool powerPelletActive);
    bool obstacleBoundsCheck(Window& win);
    GhostCollision ghostCollisionCheck(Ghost& clyde, Inky& inky, Blinky& blinky, Pinky& pinky);
    bool powerPelletCheck(Window& win);

    // getters
    void getUserInputAndSetDirection(Window& win);

    // setters
    void setY(PositionChange direction);
    void setX(PositionChange direction);
};