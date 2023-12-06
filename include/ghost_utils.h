#pragma once

#include <vector>

#include "z_aggregate.h"
#include "z_window.h"
#include "z_ghost.h"
#include "z_blinky.h"
#include "z_pinky.h"
#include "z_inky.h"

class GhostUtils
{
private:
    static std::vector<Vec> createGhostPath(Vec start, Window& win, Vec target);

    friend class Blinky;
    friend class Inky;
    friend class Pinky;
    friend class Ghost;
};
