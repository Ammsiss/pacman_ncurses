#pragma once

#include <ncurses.h>
#include <vector>

#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"

class Pacman
{
private:
    Direction m_direction;
    Vec m_pacVec;
    char m_pacman;
    char m_userInput;

    void setY(DirectionEval direction);
    void setX(DirectionEval direction);
    void setDirection();
    bool obstacleBoundsCheck(std::vector<Obstacle>& obstacleList);

public:
    Pacman();
    void printAndRefresh(Window& win);
    void erase(Window& win);
    void getUserInputAndSetDirection(Window& win);
    void movePacmanBasedOnDirection(Window& win, std::vector<Obstacle>& obstacleList);
};