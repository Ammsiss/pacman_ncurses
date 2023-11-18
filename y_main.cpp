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
    start_color();
    use_default_colors();
    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_MAGENTA, 1000, 550, 550);
    init_color(COLOR_YELLOW, 1000, 1000, 0);
    init_color(COLOR_GREEN, 1000, 550, 0);
    init_pair(0, -1, -1);
    init_pair(1, 1, 0);
    init_pair(2, 6, 0);
    init_pair(3, 5, 0);
    init_pair(4, 3, 0);
    init_pair(7, 7, 0);
    init_pair(6, COLOR_BLUE, 0);
    init_pair(5, COLOR_GREEN, 0);
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

void gameLoop(Window& gameW, std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter, Pellet& pellets)
{
    // init ya boi
    Pacman pacman{};
    // init ghosts
    Ghost pinky{350ms, GhostColor::pink};
    Ghost inky{325ms, GhostColor::cyan};
    Ghost blinky{250ms, GhostColor::red};
    Ghost clyde{275ms, GhostColor::orange};

    /*
    Ghost g1{100ms, GhostColor::pink};
    Ghost g2{200ms, GhostColor::pink};
    Ghost g3{300ms, GhostColor::pink};
    Ghost g4{50ms, GhostColor::pink};
    Ghost g5{20ms, GhostColor::pink};
    Ghost g6{500ms, GhostColor::pink};
    Ghost g7{253ms, GhostColor::cyan};
    Ghost g8{258ms, GhostColor::cyan};
    Ghost g9{128ms, GhostColor::cyan};
    Ghost ga{143ms, GhostColor::cyan};
    Ghost gb{53ms, GhostColor::cyan};
    Ghost gc{123ms, GhostColor::cyan};
    Ghost gd{240ms, GhostColor::red};
    Ghost ge{350ms, GhostColor::red};
    Ghost gf{482ms, GhostColor::red};
    Ghost gg{123ms, GhostColor::red};
    Ghost gh{90ms, GhostColor::red};
    Ghost gi{890ms, GhostColor::red};
    Ghost gj{70ms, GhostColor::red};
    Ghost gk{50ms, GhostColor::orange};
    Ghost gl{30ms, GhostColor::orange};
    Ghost gm{210ms, GhostColor::orange};
    Ghost gn{220ms, GhostColor::orange};
    Ghost go{230ms, GhostColor::orange};
    Ghost gp{240ms, GhostColor::orange};
    */

    gameW.gameCountDown();

    // main loop
    while(true)
    {
        pacman.timeToMove(gameW, obstacleList, pellets);
        pinky.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde, pellets);
        inky.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde, pellets);
        blinky.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde, pellets);
        clyde.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde, pellets);

        /*
        g1.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        g2.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        g3.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        g4.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        g5.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        g6.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        g7.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        g8.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        g9.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        ga.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        gb.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        gc.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        gd.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        ge.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        gf.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        gg.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        gh.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        gi.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        gj.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        gk.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        gl.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        gm.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        gn.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        go.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        gp.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        */

        // sleep to avoid infinite checks
        std::this_thread::sleep_for(5ms);
    }
}

int main()
{
    ncursesInit(); 

    // init window stuff
    Window gameW{};
    nodelay(gameW.getWindow(), true);
    std::vector<Vec> windowPerimeter{ gameW.getWindowPerimeter() };
    std::vector<std::vector<int>> windowArea{ gameW.getWindowArea() };

    // init obstacles stuff
    std::vector<Obstacle> obstacleList{obstacleInitAndRefresh(gameW)};

    // init pellet stuff
    Pellet pellets{};
    pellets.initPelletVector(gameW, obstacleList, windowArea, windowPerimeter);

    // Start!
    gameLoop(gameW, obstacleList, windowPerimeter, pellets);
    
    endwin();
    return 0;
}