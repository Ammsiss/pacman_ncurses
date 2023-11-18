#pragma once

#include <ncurses.h>
#include <vector>
#include <chrono>

#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"
#include "z_pellet.h"

class Pacman
{
private:
    Direction m_direction;
    Vec m_pacVec;
    char m_userInput;
    int m_rightPortalX;
    int m_leftPortalX;
    int m_portalY;
    std::chrono::milliseconds m_interval;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;

    void setY(PositionChange direction);
    void setX(PositionChange direction);
    void setDirection();
    bool obstacleBoundsCheck(std::vector<Obstacle>& obstacleList);
    void printPacmanBasedOnDirectionAndRefresh(Window& win);
    void erase(Window& win);
    void getUserInputAndSetDirection(Window& win);
    void movePacmanBasedOnDirection(Window& win, std::vector<Obstacle>& obstacleList);
    void setGarbageValue(Pellet& pellets);

public:
    Pacman();
    void timeToMove(Window& win, std::vector<Obstacle>& obstacleList, Pellet& pellets);

};