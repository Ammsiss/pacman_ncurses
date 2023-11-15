    #include <vector>
    #include <ncurses.h>

    #include "z_aggregate.h"
    #include "z_pellet.h"
    
    // private

    void Pellet::initPelletVector(Vec& startPoint, Vec& pelletDimensions)
    {
        int totalVectors{pelletDimensions.y * pelletDimensions.x};

        for (std::size_t i{0}; i < totalVectors; ++i)
        {
            m_pelletVec.emplace_back(); 
        }

        std::size_t vectorIncrement{0};
        for(std::size_t iY{0}; iY < pelletDimensions.y; ++iY)
        {
            for(std::size_t iX{0}; iX < pelletDimensions.x; ++iX)
            {
                m_pelletVec[vectorIncrement].y = (iY + startPoint.y);

                m_pelletVec[vectorIncrement].x = (iX + startPoint.x);

                ++vectorIncrement;
            }
        }
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

    Pellet::Pellet( Vec startPoint, Vec pelletDimensions, Window& win)
        : m_pelletVec {}
    {
        initPelletVector(startPoint, pelletDimensions);
        printAndRefreshPellet(win);
    }

    const std::vector<Vec>& Pellet::getPelletVec() { return m_pelletVec; }
