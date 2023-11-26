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

/********************************************************************** PUBLIC MEMBERS **********************************************************************/

VectorSpace::VectorSpace(Window& win, std::vector<Obstacle>& obstacleList)
{
    init2DVectorSpace(win, obstacleList);
}

void VectorSpace::assignPelletNotEaten(Window& win)
{
    // re assigns all pellets to not eaten
    for(std::size_t y{0}; y < win.getWindowArea().size(); ++y)
    {
        for(std::size_t x{0}; x < win.getWindowArea()[y].size(); ++x)
        {
            if(win.getWindowArea()[y][x] != CellName::obstacleValue && win.getWindowArea()[y][x] != CellName::perimeterValue && win.getWindowArea()[y][x] != CellName::ghostBox && 
                win.getWindowArea()[y][x] != CellName::powerPellet && win.getWindowArea()[y][x] != CellName::powerPelletEaten)
                    win.getWindowArea()[y][x] = CellName::notEaten;
        }
    }

    // re assigns power pellets to not eaten
    win.getWindowArea()[23][1] = CellName::powerPellet;
    win.getWindowArea()[23][26] = CellName::powerPellet;
    win.getWindowArea()[3][1] = CellName::powerPellet;
    win.getWindowArea()[3][26] = CellName::powerPellet;
}

void VectorSpace::printPellet(Window& win)
{
    for(std::size_t y{0}; y < win.getWindowArea().size(); ++y)
    {
        for(std::size_t x{0}; x < win.getWindowArea()[y].size(); ++x) 
        {
            // clears maze
            if(win.getWindowArea()[y][x] != CellName::perimeterValue && win.getWindowArea()[y][x] != CellName::obstacleValue)
            {
                mvwprintw(win.getWindow(), y, x, " ");
            }

            // reprints pellets based on what has been eaten by pacman
            if(win.getWindowArea()[y][x] != CellName::ghostBox && win.getWindowArea()[y][x] != CellName::perimeterValue 
                && win.getWindowArea()[y][x] != CellName::obstacleValue && win.getWindowArea()[y][x] != CellName::pelletEaten && win.getWindowArea()[y][x] != CellName::powerPelletEaten)
            {
                wattron(win.getWindow(), COLOR_PAIR(Color::white_black));
                mvwprintw(win.getWindow(), y, x, "•");
                wattroff(win.getWindow(), COLOR_PAIR(Color::white_black));
                wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
            }
        }
    }
}

void VectorSpace::printPowerPellet(Window& win)
{
    wattron(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    wattron(win.getWindow(), A_BLINK);

    if(win.getWindowArea()[23][1] == CellName::powerPellet)
        mvwprintw(win.getWindow(), 23, 1, "⬤");
    
    if(win.getWindowArea()[23][26] == CellName::powerPellet)
        mvwprintw(win.getWindow(), 23, 26, "⬤");

    if(win.getWindowArea()[3][1] == CellName::powerPellet)
        mvwprintw(win.getWindow(), 3, 1, "⬤");

    if(win.getWindowArea()[3][26] == CellName::powerPellet)
        mvwprintw(win.getWindow(), 3, 26, "⬤");   

    wattroff(win.getWindow(), COLOR_PAIR(Color::yellow_black));
    wattroff(win.getWindow(), A_BLINK);
    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
}

/********************************************************************** PRIVATE MEMBERS **********************************************************************/

void VectorSpace::init2DVectorSpace(Window& win, std::vector<Obstacle>& obstacleList)
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

    win.getWindowArea()[14][1] = CellName::portalLeft;
    win.getWindowArea()[14][26] = CellName::portalRight;

    win.getWindowArea()[23][1] = CellName::powerPellet;
    win.getWindowArea()[23][26] = CellName::powerPellet;
    win.getWindowArea()[3][1] = CellName::powerPellet;
    win.getWindowArea()[3][26] = CellName::powerPellet;
}