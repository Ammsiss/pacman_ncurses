//external
#include <ncurses.h>

//user
#include "z_aggregate.h"
#include "z_vectorspace.h"
#include "z_obstacle.h"
#include "z_window.h"

//std
#include <vector>
#include <algorithm>


// private

void Pellet::printAndRefreshPellet(Window& win)
{
    for(std::size_t y{0}; y < win.getWindowArea().size(); ++y)
    {
        for(std::size_t x{0}; x < win.getWindowArea()[y].size(); ++x)
        {
            if(win.getWindowArea()[y][x] != CellName::pelletEaten && win.getWindowArea()[y][x] != CellName::perimeterValue && win.getWindowArea()[y][x] != CellName::obstacleValue)
            {
                wattron(win.getWindow(), COLOR_PAIR(Color::white_black));
                mvwprintw(win.getWindow(), y, x, "•");
                wattroff(win.getWindow(), COLOR_PAIR(Color::white_black));
                wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
            }
        }
    }

    wrefresh(win.getWindow());
}

// public

void Pellet::init2DVectorSpace(Window& win, std::vector<Obstacle>& obstacleList)
{
    // ADD PERIMETER TO windowArea vector space
    for(std::size_t perimeterIterator{0}; perimeterIterator < win.getWindowPerimeter().size(); ++perimeterIterator)
    {
        win.getWindowArea()[win.getWindowPerimeter()[perimeterIterator].y][win.getWindowPerimeter()[perimeterIterator].x] = CellName::perimeterValue;
    }

    // ADD OBSTACLES TO windowArea vector space
    for(std::size_t listIterator{0}; listIterator < obstacleList.size(); ++listIterator)
    {
        for(std::size_t obstacleIterator{0}; obstacleIterator < obstacleList[listIterator].getObsVec().size(); ++obstacleIterator)
        {
            win.getWindowArea()[obstacleList[listIterator].getObsVec()[obstacleIterator].y][obstacleList[listIterator].getObsVec()[obstacleIterator].x] = CellName::obstacleValue;
        }
    }

    printAndRefreshPellet(win);
}
