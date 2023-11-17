#pragma once

#pragma once

#include <ncurses.h>
#include <vector>
#include <chrono>

#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"

class Ghost
{
private:
    Direction m_direction;
    Vec m_ghostVec;
    int m_rightPortalX;
    int m_leftPortalX;
    int m_portalY;
    GhostColor m_ghostColor;
    std::chrono::milliseconds m_interval;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;

    void setY(PositionChange direction);
    void setX(PositionChange direction);
    void setValidDirection(std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter);
    bool obstacleBoundsCheck(std::vector<Obstacle>& obstacleList);
    void printAndRefreshGhost(Window& win);
    void erase(Window& win);
    bool moveGhostBasedOnDirection(std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter);
    bool perimeterBoundsCheck(std::vector<Vec>& windowPerimeter);
    GhostColor checkGhostOverLap(Ghost& pinky, Ghost& inky, Ghost& blinky, Ghost& clyde);
    void printOverLap(Window& win, GhostColor overLapColor);

public:
    Ghost(std::chrono::milliseconds speed, GhostColor ghostColor);
    void timeToMove(Window& win, std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter, Ghost& pinky, Ghost& inky, Ghost& blinky, Ghost& clyde);

    // getters
    GhostColor getGhostColor();
    Vec getGhostVec();

};