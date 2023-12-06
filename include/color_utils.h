#pragma once

#include <ncurses.h>

#include "z_aggregate.h"
#include "z_window.h"

namespace ColorUtils
{
    void colorOn(Color::ColorPair color, Window& win, bool blink = false);
    void colorOff(Color::ColorPair color, Window& win);
}
