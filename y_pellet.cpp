    #include <vector>
    #include <algorithm>

    #include <ncurses.h>

    #include "z_aggregate.h"
    #include "z_pellet.h"
    #include "z_obstacle.h"
    #include "z_window.h"


    // private

    void Pellet::printAndRefreshPellet(Window& win)
    {
        for(std::size_t y{0}; y < m_pelletVec.size(); ++y)
        {
            for(std::size_t x{0}; x < m_pelletVec[y].size(); ++x)
            {
                if(m_pelletVec[y][x] != m_garbage)
                {
                    wattron(win.getWindow(), COLOR_PAIR(Color::white_black));
                    mvwprintw(win.getWindow(), y, x, "•");
                    wattroff(win.getWindow(), COLOR_PAIR(Color::white_black));
                    wattron(win.getWindow(), COLOR_PAIR(Color::default_color));
                }
            }
        }

        removeGhostBoxPellet(win);

        wrefresh(win.getWindow());
    }

    void Pellet::assignGarbageToGhostBox()
    {
        // HARDCODED CHANGE MAYBE

        for(int y{13}; y < 16; ++y)
        {
            for(int x{11}; x < 17; ++x)
            {
                m_pelletVec[y][x] = m_garbage;
            }
        }

        m_pelletVec[12][13] = m_garbage;
        m_pelletVec[12][14] = m_garbage;
    }

    void Pellet::removeGhostBoxPellet(Window& win)
    {
        // HARD CODED CHANGE MAYBE

        mvwprintw(win.getWindow(), 13, 11, "      ");
        mvwprintw(win.getWindow(), 14, 11, "      ");
        mvwprintw(win.getWindow(), 15, 11, "      ");
        mvwprintw(win.getWindow(), 12, 13, "  ");

        assignGarbageToGhostBox();

        wrefresh(win.getWindow());
    }

    // public

    Pellet::Pellet()
        : m_pelletVec {}, m_garbage{10000}
    {
    }

    void Pellet::initPelletVector(Window& win, std::vector<Obstacle>& obstacleList, std::vector<std::vector<int>>& windowArea, std::vector<Vec>& windowPerimeter)
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

        printAndRefreshPellet(win);
    }

    std::vector<std::vector<int>>& Pellet::getPelletVec() { return m_pelletVec; }
