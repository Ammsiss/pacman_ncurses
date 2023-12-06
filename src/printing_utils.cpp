#include <chrono>
#include <ncurses.h>
#include <array>

#include "color_utils.h"
#include "z_aggregate.h"
#include "z_window.h"
#include "printing_utils.h"

using namespace std::chrono_literals;

void PrintingUtils::ghostFlashing(Window& win, TypeAlias::timepoint& lastTime, std::chrono::milliseconds& interval, TypeAlias::timepoint& currentTime, Vec position)
{
    auto elapsedTime{ currentTime - lastTime };
    auto remainingTime{ interval - elapsedTime };

    Color::ColorPair color{};
    std::array<std::chrono::milliseconds, 6> ghostIntervals{250ms, 500ms, 750ms, 1000ms, 1250ms, 1500ms};

    for(std::size_t i{0}; i < ghostIntervals.size(); ++i)
    {
        if(remainingTime <= ghostIntervals[i])
        {
            if(i % 2 == 0)
                color = Color::white_black;
            else
                color = Color::blue_black;

            ColorUtils::colorOn(color, win);
            mvwprintw(win.getWindow(), position.y, position.x, "ᗣ");
            ColorUtils::colorOff(color, win);
            wrefresh(win.getWindow());

            break;
        }
    }
}
