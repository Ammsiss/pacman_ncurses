#pragma once

#include <ncurses.h>
#include <vector>

#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"


class Pellet
{
private:
    std::vector<Vec> m_pelletVec;
    
    void initPelletVector(Vec& startPoint, Vec& pelletDimensions, std::vector<Obstacle>& obstacleList);

    void printAndRefreshPellet(Window& win);

public:
    Pellet( Vec startPoint, Vec pelletDimensions, Window& win, std::vector<Obstacle>& obstacleList);

    const std::vector<Vec>& getPelletVec();
};