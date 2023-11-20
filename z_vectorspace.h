#pragma once

#include <ncurses.h>
#include <vector>

#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"


class VectorSpace
{
public:
    VectorSpace(Window& win, std::vector<Obstacle>& obstacleList);

private:
    void printAndRefreshPellet(Window& win);
    void init2DVectorSpace(Window& win, std::vector<Obstacle>& obstacleList);
};