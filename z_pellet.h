#pragma once

#include <ncurses.h>
#include <vector>

#include "z_aggregate.h"
#include "z_window.h"


class Pellet
{
private:
    std::vector<Vec> m_pelletVec{};
    char pelletCh{'*'};
    
    void initPelletVector(Vec& startPoint, Vec& pelletDimensions);

    void printAndRefreshPellet(Window& win);

public:
    Pellet( Vec startPoint, Vec pelletDimensions, Window& win);

    const std::vector<Vec>& getPelletVec();
};