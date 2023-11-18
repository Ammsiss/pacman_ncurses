#pragma once

#include <ncurses.h>
#include <vector>

#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"


class Pellet
{
private:
    int m_garbage;

    std::vector<std::vector<int>> m_pelletVec;
    void initPelletVector(std::vector<Obstacle>& obstacleList, std::vector<std::vector<int>>& windowArea, std::vector<Vec>& windowPerimeter);

public:
    Pellet( Window& win, std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter, std::vector<std::vector<int>>& windowArea);

    void printAndRefreshPellet(Window& win);
    std::vector<std::vector<int>> getPelletVec();
};