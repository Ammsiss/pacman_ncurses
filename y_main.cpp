#include <ncurses.h>
#include <clocale>

#include "z_pacman.h"
#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"

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
    std::vector<Obstacle> obstacleList
    {
        // Initialize obstacle use format: 
        // { {yoffset, xoffset}, {ydimension, xdimension}, gameW}
        { {10, 10}, {4, 4}, gameW }, 
        { {20, 10}, {2, 2}, gameW }
    };

    return obstacleList;
}

void gameLoop(Window& gameW)
{
    Pacman p1{};

    // define chrono duration and 2 system time instances to create pacman's timed movement
    auto interval{200ms};
    auto lastTime{std::chrono::high_resolution_clock::now()};
    while(true)
    {
        auto currentTime{std::chrono::high_resolution_clock::now()};

        p1.getUserInputAndSetDirection(gameW);
        if (currentTime - lastTime >= interval)
        {
            p1.erase(gameW);
            p1.movePacmanBasedOnDirection(gameW);
            p1.printAndRefresh(gameW);

            lastTime = currentTime;
        }
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
    // std::vector<Obstacle> obstacleList{obstacleInitAndRefresh(gameW)};

    // Start Game
    gameLoop(gameW);
    
    // wait for user input before exiting...
    getch();
    endwin();
    return 0;
}