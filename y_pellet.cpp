    #include <vector>
    #include <ncurses.h>

    #include "z_aggregate.h"
    #include "z_pellet.h"
    #include "z_obstacle.h"
    #include "z_window.h"

    // private

    void Pellet::initPelletVector(std::vector<Obstacle>& obstacleList, std::vector<std::vector<Vec>>& windowArea, std::vector<Vec>& windowPerimeter)
    {
        // Algorithm to initialize a vector of all the correct coordinate positions based on obstacles
        // that dynamically resizes based on obstacles.

        for(std::size_t totalArea{0}; totalArea < windowArea.size(); ++totalArea)
        {

        }
    }

    // public

    Pellet::Pellet( Window& win, std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter, std::vector<std::vector<Vec>>& windowArea)
        : m_pelletVec {}
    {
        initPelletVector(obstacleList, windowArea, windowPerimeter);
    }

    void Pellet::printAndRefreshPellet(Window& win)
    {
        
    }

    std::vector<std::vector<int>> Pellet::getPelletVec() { return m_pelletVec; }
