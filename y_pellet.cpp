    #include <vector>
    #include <ncurses.h>

    #include "z_aggregate.h"
    #include "z_pellet.h"
    #include "z_obstacle.h"

    // private

    void Pellet::initPelletVector(Vec& startPoint, Vec& pelletDimensions, std::vector<Obstacle>& obstacleList)
    {

    }

    void Pellet::printAndRefreshPellet(Window& win)
    {
        for(std::size_t i{0}; i < m_pelletVec.size(); ++i)
        {
            mvwprintw(win.getWindow(), m_pelletVec[i].y, m_pelletVec[i].x, "•");
        }
        wrefresh(win.getWindow());
    }

    // public

    Pellet::Pellet( Vec startPoint, Vec pelletDimensions, Window& win, std::vector<Obstacle>& obstacleList)
        : m_pelletVec {}
    {
        initPelletVector(startPoint, pelletDimensions, obstacleList);
        printAndRefreshPellet(win);
    }

    const std::vector<Vec>& Pellet::getPelletVec() { return m_pelletVec; }
