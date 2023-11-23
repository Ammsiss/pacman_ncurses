#pragma once

#include <ncurses.h>
#include <vector>

#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"


class VectorSpace
{
public:
    // constructor
    VectorSpace(Window& win, std::vector<Obstacle>& obstacleList);

    // public methods
    void assignPelletNotEaten(Window& win);
    void printPellet(Window& win);
    void printPowerPellet(Window& win);

private:
    // private methods
    void init2DVectorSpace(Window& win, std::vector<Obstacle>& obstacleList);
};