    #include <vector>
    #include <algorithm>

    #include <ncurses.h>

    #include "z_aggregate.h"
    #include "z_pellet.h"
    #include "z_obstacle.h"
    #include "z_window.h"


    // private

    void Pellet::printAndRefreshPellet(Window& win, const int perimeterValue)
    {
        for(std::size_t y{0}; y < m_pelletVec.size(); ++y)
        {
            for(std::size_t x{0}; x < m_pelletVec[y].size(); ++x)
            {
                if(m_pelletVec[y][x] != m_garbage && m_pelletVec[y][x] != perimeterValue)
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

    // public
    void Pellet::initAndPrintPelletVector(Window& win, std::vector<Obstacle>& obstacleList, std::vector<std::vector<int>>& windowArea, std::vector<Vec>& windowPerimeter)
    {
        const int perimeterValue{10001};

        // set 2d array windowArea perimeter values to 10001
        for(std::size_t perimeterIterator{0}; perimeterIterator < windowPerimeter.size(); ++perimeterIterator)
        {
            windowArea[windowPerimeter[perimeterIterator].y][windowPerimeter[perimeterIterator].x] = perimeterValue;
        }

        // set 2d array windowArea obstacle values to garbage
        for(std::size_t listIterator{0}; listIterator < obstacleList.size(); ++listIterator)
        {
            for(std::size_t obstacleIterator{0}; obstacleIterator < obstacleList[listIterator].getObsVec().size(); ++obstacleIterator)
            {
                windowArea[obstacleList[listIterator].getObsVec()[obstacleIterator].y][obstacleList[listIterator].getObsVec()[obstacleIterator].x] = m_garbage;
            }
        }

        m_pelletVec = windowArea;

        initOnlyPelletVector();
        printAndRefreshPellet(win, perimeterValue);
    }

    std::vector<std::vector<int>>& Pellet::getPelletVec() { return m_pelletVec; }

    std::vector<Vec>& Pellet::getOnlyPelletVec() { return m_onlyPelletVec; }