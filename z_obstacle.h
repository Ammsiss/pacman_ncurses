#pragma once

// external
#include <ncurses.h>

// user
#include "z_aggregate.h"
#include "z_window.h"

// std
#include <vector>
class Obstacle
{
public:
    // constructor
    Obstacle( Vec startPoint, Vec obsDimensions, Window& win);

    // getter
    const std::vector<Vec>& getObsVec();

private:
    // private variable
    std::vector<Vec> m_obsVec{};
    chtype obsCh{ACS_CKBOARD};
    
    // private methods
    void initObsVector(Vec& startPoint, Vec& obsDimensions);
    void printAndRefreshObstacle(Window& win);
};