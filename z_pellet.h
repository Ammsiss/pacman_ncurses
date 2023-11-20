#pragma once

#include <ncurses.h>
#include <vector>

#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"


class Pellet
{
private:
    /*
    int m_garbage{10000};
    int m_score{0};
    */
    /*
    std::vector<std::vector<int>> m_pelletVec{};
    std::vector<Vec> m_onlyPelletVec{};
    */

    /*
    void removeGhostBoxPellet(Window& win);
    void assignGarbageToGhostBox();
    */
    void printAndRefreshPellet(Window& win);
    // void initOnlyPelletVector();

public:
    void initAndPrintPellets(Window& win, std::vector<Obstacle>& obstacleList);

    // std::vector<std::vector<int>>& getPelletVec();
    // std::vector<Vec>& getOnlyPelletVec();
};