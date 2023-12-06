#include <ncurses.h>

#include "z_window.h"
#include "z_aggregate.h"

namespace ColorUtils
{
    void colorOn(Color::ColorPair color, Window& win, bool blink)
    {
        wattron(win.getWindow(), COLOR_PAIR(color));

        if(blink)
            wattron(win.getWindow(), A_BLINK);
    }

    void colorOff(Color::ColorPair color, Window& win)
    {
        wattroff(win.getWindow(), COLOR_PAIR(color));
        wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
        wattroff(win.getWindow(), A_BLINK);
    }
}
