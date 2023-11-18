    #include <vector>
    #include <algorithm>

    #include <ncurses.h>

    #include "z_aggregate.h"
    #include "z_pellet.h"
    #include "z_obstacle.h"
    #include "z_window.h"


    // private

    void Pellet::initPelletVector(std::vector<Obstacle>& obstacleList, std::vector<std::vector<int>>& windowArea, std::vector<Vec>& windowPerimeter)
    {
        for(std::size_t y{0}; y < windowArea.size(); ++y)
        {
            for(std::size_t x{0}; x < windowArea[y].size(); ++x)
            {
                for(std::size_t perimeterIterator{0}; perimeterIterator < windowPerimeter.size(); ++perimeterIterator)
                {
                    if(windowPerimeter[perimeterIterator].x == x && windowPerimeter[perimeterIterator].y == y)
                    {
                       windowArea[y][x] = m_garbage;
                    }
                }

                for(std::size_t listIterator{0}; listIterator < obstacleList.size(); ++listIterator)
                {
                    for(std::size_t obstacleIterator{0}; obstacleIterator < obstacleList[listIterator].getObsVec().size(); ++obstacleIterator)
                    {
                        if(obstacleList[listIterator].getObsVec()[obstacleIterator].x == x && obstacleList[listIterator].getObsVec()[obstacleIterator].y == y)
                        {
                            windowArea[y][x] = m_garbage;
                        }
                    }
                }
            }
        }

        m_pelletVec = windowArea;
    }

    // public

    Pellet::Pellet( Window& win, std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter, std::vector<std::vector<int>>& windowArea)
        : m_pelletVec {}, m_garbage{10000}
    {
        initPelletVector(obstacleList, windowArea, windowPerimeter);
        printAndRefreshPellet(win);
    }

    void Pellet::printAndRefreshPellet(Window& win)
    {
        for(std::size_t y{0}; y < m_pelletVec.size(); ++y)
        {
            for(std::size_t x{0}; x < m_pelletVec[y].size(); ++x)
            {
                if(m_pelletVec[y][x] != m_garbage)
                {
                    wattron(win.getWindow(), COLOR_PAIR(7));
                    mvwprintw(win.getWindow(), y, x, "•");
                    wattroff(win.getWindow(), COLOR_PAIR(7));
                    wattron(win.getWindow(), COLOR_PAIR(0));
                }
            }
        }

        wrefresh(win.getWindow());
    }

    std::vector<std::vector<int>> Pellet::getPelletVec() { return m_pelletVec; }
