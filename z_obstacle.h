#pragma once

#include <ncurses.h>
#include <vector>

#include "z_aggregate.h"
#include "z_window.h"


class Obstacle
{
private:
    std::vector<Vec> m_obsVec{};
    chtype obsCh{ACS_CKBOARD};
    
    void initObsVector(Vec& startPoint, Vec& obsDimensions);

    void printAndRefreshObstacle(Window& win);

public:
    Obstacle( Vec startPoint, Vec obsDimensions, Window& win);

    const std::vector<Vec>& getObsVec();
};