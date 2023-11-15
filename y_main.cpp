// external libraries
#include <ncurses.h>
#include <clocale>

// header's
#include "z_pacman.h"
#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"
#include "z_pellet.h"

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
        // Top map obstacles
        {{2, 18}, {3, 15}, gameW},
        {{2, 2}, {3, 15}, gameW},
    };

    return obstacleList;
}

std::vector<Pellet> pelletInitAndRefresh(Window& gameW)
{
    std::vector<Pellet> pelletList
    {
        { {10, 10}, {2, 10}, gameW}
    };

    return pelletList;
}

void gameLoop(Window& gameW, std::vector<Obstacle>& obstacleList)
{
    // init ya boi
    Pacman p1{};

    while(true)
    {
        p1.timeToMovePacman(gameW, obstacleList);

        // sleep to avoid infinite checks
        std::this_thread::sleep_for(5ms);
    }
}

int main()
{
    ncursesInit(); 

    // Init windows
    Window gameW{};
    nodelay(gameW.getWindow(), true);

    // Init Obstacles
    std::vector<Obstacle> obstacleList{obstacleInitAndRefresh(gameW)};

    // Init Pellets
    std::vector<Pellet> pelletList{pelletInitAndRefresh(gameW)};

    // Start Game
    gameLoop(gameW, obstacleList);
    
    // wait for user input before exiting...
    getch();
    endwin();
    return 0;
}