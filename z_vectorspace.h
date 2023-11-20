#pragma once

#include <ncurses.h>
#include <vector>

#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"


class Pellet
{
private:
    void printAndRefreshPellet(Window& win);

public:
    void init2DVectorSpace(Window& win, std::vector<Obstacle>& obstacleList);
};