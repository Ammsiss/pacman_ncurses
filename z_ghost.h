#pragma once

#pragma once

#include <ncurses.h>
#include <vector>
#include <chrono>

#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"
#include "z_pellet.h"

class Ghost
{
private:
    Direction m_direction;
    Vec m_ghostVec;
    int m_rightPortalX;
    int m_leftPortalX;
    int m_portalY;
    Color::ColorPair m_ghostColor;
    std::chrono::milliseconds m_interval;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;

    void setY(PositionChange direction);
    void setX(PositionChange direction);
    void setValidDirection(std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter);
    bool obstacleBoundsCheck(std::vector<Obstacle>& obstacleList);
    void printAndRefreshGhost(Window& win);
    void erase(Window& win);
    bool moveGhostInValidDirection(std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter);
    bool perimeterBoundsCheck(std::vector<Vec>& windowPerimeter);
    Color::ColorPair checkGhostOverLap(Ghost& pinky, Ghost& inky, Ghost& blinky, Ghost& clyde);
    void printOverLap(Window& win, Color::ColorPair overLapColor);
    void printPelletBack(Pellet& pellets, Window& win);
    bool oppositeDirectionCheck(Direction directionCheck);

public:
    Ghost(std::chrono::milliseconds speed, Color::ColorPair ghostColor);
    void timeToMove(Window& win, std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter, Ghost& pinky, Ghost& inky, Ghost& blinky, Ghost& clyde, Pellet& pellets);

    // getters
    Color::ColorPair getGhostColor();
    Vec getGhostVec();

};