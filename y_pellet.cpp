//external
#include <ncurses.h>

//user
#include "z_aggregate.h"
#include "z_pellet.h"
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

/*
// assigns pellet coordinates only not garbage to m_onlyPelletVec
void Pellet::initOnlyPelletVector()
{
    for(std::size_t y{0}; y < m_pelletVec.size(); ++y)
    {
        for(std::size_t x{0}; x < m_pelletVec[y].size(); ++x)
        {
            if(m_pelletVec[y][x] != m_garbage)
                m_onlyPelletVec.emplace_back(Vec{static_cast<int>(y), static_cast<int>(x)});
        }
    }
}
*/

// public

void Pellet::initAndPrintPellets(Window& win, std::vector<Obstacle>& obstacleList)
{
    // ADD PERIMETER TO windowArea
    for(std::size_t perimeterIterator{0}; perimeterIterator < win.getWindowPerimeter().size(); ++perimeterIterator)
    {
        win.getWindowArea()[win.getWindowPerimeter()[perimeterIterator].y][win.getWindowPerimeter()[perimeterIterator].x] = CellName::perimeterValue;
    }

    // ADD OBSTACLES TO windowArea
    for(std::size_t listIterator{0}; listIterator < obstacleList.size(); ++listIterator)
    {
        for(std::size_t obstacleIterator{0}; obstacleIterator < obstacleList[listIterator].getObsVec().size(); ++obstacleIterator)
        {
            win.getWindowArea()[obstacleList[listIterator].getObsVec()[obstacleIterator].y][obstacleList[listIterator].getObsVec()[obstacleIterator].x] = CellName::obstacleValue;
        }
    }

    printAndRefreshPellet(win);
}

// std::vector<std::vector<int>>& Pellet::getPelletVec() { return m_pelletVec; }

// std::vector<Vec>& Pellet::getOnlyPelletVec() { return m_onlyPelletVec; }