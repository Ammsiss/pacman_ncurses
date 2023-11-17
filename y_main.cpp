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

void gameLoop(Window& gameW, std::vector<Obstacle>& obstacleList, std::vector<Vec>& windowPerimeter)
{
    // init ya boi
    Pacman pacman{};
    Ghost pinky{350ms, GhostColor::pink};
    Ghost inky{325ms, GhostColor::cyan};
    Ghost blinky{250ms, GhostColor::red};
    Ghost clyde{275ms, GhostColor::orange};

    /*
    Ghost g1{100ms};
    Ghost g2{200ms};
    Ghost g3{300ms};
    Ghost g4{50ms};
    Ghost g5{20ms};
    Ghost g6{500ms};
    Ghost g7{253ms};
    Ghost g8{258ms};
    Ghost g9{128ms};
    Ghost ga{143ms};
    Ghost gb{53ms};
    Ghost gc{123ms};
    Ghost gd{240ms};
    Ghost ge{350ms};
    Ghost gf{482ms};
    Ghost gg{123ms};
    Ghost gh{90ms};
    Ghost gi{890ms};
    Ghost gj{70ms};
    Ghost gk{50ms};
    Ghost gl{30ms};
    Ghost gm{210ms};
    Ghost gn{220ms};
    Ghost go{230ms};
    Ghost gp{240ms};
    */

    while(true)
    {
        pacman.timeToMove(gameW, obstacleList);
        pinky.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        inky.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        blinky.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);
        clyde.timeToMove(gameW, obstacleList, windowPerimeter, pinky, inky, blinky, clyde);

        /*
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
        */

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
    std::vector<std::vector<Vec>> windowArea{ gameW.getWindowArea() };
    std::vector<Obstacle> obstacleList{obstacleInitAndRefresh(gameW)};

    Pellet pellets{gameW, obstacleList, windowPerimeter, windowArea};

    // Start!
    gameLoop(gameW, obstacleList, windowPerimeter);
    
    endwin();
    return 0;
}