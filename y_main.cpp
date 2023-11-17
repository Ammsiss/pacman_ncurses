// external libraries
#include <ncurses.h>
#include <clocale>

// header's
#include "z_pacman.h"
#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"
#include "z_pellet.h"
#include "z_ghost.h"

// std library
#include <chrono>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

void ncursesInit()
{
    // set up ncurses
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    refresh();
}

std::vector<Obstacle> obstacleInitAndRefresh(Window& gameW)
{

    // Initialize obstacle use format: 
    // { {yoffset, xoffset}, {ydimension, xdimension}, gameW}
    std::vector<Obstacle> obstacleList
    {
        { {2, 2}, {3, 4}, gameW},
        { {2, 7}, {3, 5}, gameW},
        { {1, 13}, {4, 2}, gameW },
        { {2, 16}, {3, 5}, gameW },
        { {2, 22}, {3, 4}, gameW },

        { {6, 2}, {2, 4}, gameW },

        { {6, 7}, {8, 2}, gameW },
        { {9, 9}, {2, 3}, gameW },

        { {6, 10}, {2, 8}, gameW },
        { {8, 13}, {3, 2}, gameW },
        
        { {6, 19}, {8, 2}, gameW },
        { {9, 16}, {2, 3}, gameW },

        { {6, 22}, {2, 4}, gameW },

        { {9, 1}, {5, 5}, gameW },
        { {15, 1}, {5, 5}, gameW },
        { {9, 22}, {5, 5}, gameW },
        { {15, 22}, {5, 5}, gameW },

        { {15, 7}, {5, 2}, gameW },
        { {15, 19}, {5, 2}, gameW },

        { {18, 10}, {2, 8}, gameW },
        { {20, 13}, {3, 2}, gameW },

        { {21, 2}, {2, 4}, gameW },
        { {23, 4}, {3, 2}, gameW },

        { {21, 7}, {2, 5}, gameW },
        { {21, 16}, {2, 5}, gameW },

        { {21, 22}, {2, 4}, gameW },
        { {23, 22}, {3, 2}, gameW },

        { {24, 1}, {2, 2}, gameW },
        { {24, 25}, {2, 2}, gameW },

        { {27, 2}, {2, 10}, gameW },
        { {24, 7}, {3, 2}, gameW },

        { {24, 10}, {2, 8}, gameW },
        { {26, 13}, {3, 2}, gameW },

        { {24, 19}, {3, 2}, gameW },
        { {27, 16}, {2, 10}, gameW },

        // ghost box
        { {12, 10}, {1, 3}, gameW},
        { {12, 15}, {1, 3}, gameW },
        { {13, 10}, {4, 1}, gameW },
        { {16, 11}, {1, 7}, gameW },
        { {13, 17}, {3, 1}, gameW }
    };

    return obstacleList;
}

void gameLoop(Window& gameW, std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter)
{
    // init ya boi
    Pacman pacman{};
    Ghost pinky{175ms};
    Ghost inky{200ms};
    Ghost slinky{300ms};
    Ghost g1{200ms};
    Ghost g2{200ms};
    Ghost g3{200ms};
    Ghost g4{200ms};
    Ghost g5{200ms};
    Ghost g6{200ms};
    Ghost g7{200ms};
    Ghost g8{200ms};
    Ghost g9{200ms};
    Ghost ga{200ms};
    Ghost gb{200ms};
    Ghost gc{200ms};
    Ghost gd{200ms};
    Ghost ge{200ms};
    Ghost gf{200ms};
    Ghost gg{200ms};
    Ghost gh{200ms};
    Ghost gi{200ms};
    Ghost gj{200ms};
    Ghost gk{200ms};
    Ghost gl{200ms};
    Ghost gm{200ms};
    Ghost gn{200ms};
    Ghost go{200ms};
    Ghost gp{200ms};

    while(true)
    {
        pacman.timeToMove(gameW, obstacleList);
        pinky.timeToMove(gameW, obstacleList, windowPerimeter);
        inky.timeToMove(gameW, obstacleList, windowPerimeter);
        slinky.timeToMove(gameW, obstacleList, windowPerimeter);
        g1.timeToMove(gameW, obstacleList, windowPerimeter);
        g2.timeToMove(gameW, obstacleList, windowPerimeter);
        g3.timeToMove(gameW, obstacleList, windowPerimeter);
        g4.timeToMove(gameW, obstacleList, windowPerimeter);
        g5.timeToMove(gameW, obstacleList, windowPerimeter);
        g6.timeToMove(gameW, obstacleList, windowPerimeter);
        g7.timeToMove(gameW, obstacleList, windowPerimeter);
        g8.timeToMove(gameW, obstacleList, windowPerimeter);
        g9.timeToMove(gameW, obstacleList, windowPerimeter);
        ga.timeToMove(gameW, obstacleList, windowPerimeter);
        gb.timeToMove(gameW, obstacleList, windowPerimeter);
        gc.timeToMove(gameW, obstacleList, windowPerimeter);
        gd.timeToMove(gameW, obstacleList, windowPerimeter);
        ge.timeToMove(gameW, obstacleList, windowPerimeter);
        gf.timeToMove(gameW, obstacleList, windowPerimeter);
        gg.timeToMove(gameW, obstacleList, windowPerimeter);
        gh.timeToMove(gameW, obstacleList, windowPerimeter);
        gi.timeToMove(gameW, obstacleList, windowPerimeter);
        gj.timeToMove(gameW, obstacleList, windowPerimeter);
        gk.timeToMove(gameW, obstacleList, windowPerimeter);
        gl.timeToMove(gameW, obstacleList, windowPerimeter);
        gm.timeToMove(gameW, obstacleList, windowPerimeter);
        gn.timeToMove(gameW, obstacleList, windowPerimeter);
        go.timeToMove(gameW, obstacleList, windowPerimeter);
        gp.timeToMove(gameW, obstacleList, windowPerimeter);

        // sleep to avoid infinite checks
        std::this_thread::sleep_for(5ms);
    }
}

int main()
{
    ncursesInit(); 

    Window gameW{};
    nodelay(gameW.getWindow(), true);
    std::vector<Vec> windowPerimeter{ gameW.getWindowPerimeter() };
    std::vector<Obstacle> obstacleList{obstacleInitAndRefresh(gameW)};

    // Start!
    gameLoop(gameW, obstacleList, windowPerimeter);
    
    endwin();
    return 0;
}