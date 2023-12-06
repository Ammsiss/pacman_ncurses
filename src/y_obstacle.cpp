//user
#include "z_aggregate.h"
#include "z_obstacle.h"
#include "z_window.h"
#include "color_utils.h"

//std
#include <vector>
#include <ncurses.h>

/********************************************************************** PUBLIC MEMBERS **********************************************************************/

// Constructor
Obstacle::Obstacle( Vec startPoint, Vec obsDimensions, Window& win)
{
    initObsVector(startPoint, obsDimensions);
    printAndRefreshObstacle(win);
}

// getter
const std::vector<Vec>& Obstacle::getObsVec() { return m_obsVec; }

/********************************************************************** PRIVATE MEMBERS **********************************************************************/

// private methods
void Obstacle::initObsVector(Vec& startPoint, Vec& obsDimensions)
{
    int totalVectors{obsDimensions.y * obsDimensions.x};

    for (std::size_t i{0}; i < totalVectors; ++i)
    {
        m_obsVec.emplace_back(); 
    }

    std::size_t vectorIncrement{0};
    for(std::size_t iY{0}; iY < obsDimensions.y; ++iY)
    {
        for(std::size_t iX{0}; iX < obsDimensions.x; ++iX)
        {
            m_obsVec[vectorIncrement].y = (iY + startPoint.y);

            m_obsVec[vectorIncrement].x = (iX + startPoint.x);

            ++vectorIncrement;
        }
    }
}

void Obstacle::printAndRefreshObstacle(Window& win)
{
    for(std::size_t i{0}; i < m_obsVec.size(); ++i)
    {
        ColorUtils::colorOn(Color::blue_black, win);
        mvwaddch(win.getWindow(), m_obsVec[i].y, m_obsVec[i].x, obsCh);
        ColorUtils::colorOff(Color::blue_black, win);
    }
    wrefresh(win.getWindow());
}
