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
    void removeGhostBoxPellet(Window& win);
    void assignGarbageToGhostBox();
    void printAndRefreshPellet(Window& win);

public:
    Pellet();

    void initPelletVector(Window& win, std::vector<Obstacle>& obstacleList, std::vector<std::vector<int>>& windowArea, std::vector<Vec>& windowPerimeter);
    std::vector<std::vector<int>>& getPelletVec();
};