#pragma once

#include <ncurses.h>
#include <chrono>

#include "typealiases.h"
#include "z_window.h"
#include "z_ghost.h"
#include "z_inky.h"
#include "z_pinky.h"
#include "z_blinky.h"

class PrintingUtils
{
private:
    static void ghostFlashing(Window& win, TypeAlias::timepoint& lastTime, std::chrono::milliseconds& interval, TypeAlias::timepoint& currentTime, Vec position);

    friend class Blinky;
    friend class Inky;
    friend class Pinky;
    friend class Ghost;
};
