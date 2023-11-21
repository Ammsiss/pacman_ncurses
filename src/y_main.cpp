//external
#include <ncurses.h>
#include <clocale>

//user
#include "z_pacman.h"
#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"
#include "z_vectorspace.h"
#include "z_ghost.h"
#include "z_menu.h"

//std
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

    // color stuff
    start_color();
    use_default_colors();
    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_MAGENTA, 1000, 550, 550); // pink
    init_color(COLOR_YELLOW, 1000, 1000, 0); // better yellow
    init_color(COLOR_GREEN, 1000, 550, 0); // orange
    init_color(COLOR_BLUE, 0, 0, 500);
    init_pair(Color::default_color, -1, -1);
    init_pair(Color::red_black, COLOR_RED, COLOR_BLACK);
    init_pair(Color::cyan_black, COLOR_CYAN, COLOR_BLACK);
    init_pair(Color::pink_black, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(Color::yellow_black, COLOR_YELLOW, COLOR_BLACK);
    init_pair(Color::white_black, COLOR_WHITE, COLOR_BLACK);
    init_pair(Color::blue_black, COLOR_BLUE, COLOR_BLACK);
    init_pair(Color::orange_black, COLOR_GREEN, COLOR_BLACK); 
    init_pair(Color::red_blue, COLOR_RED, COLOR_BLUE);
    init_pair(Color::yellow_blue, COLOR_YELLOW, COLOR_BLUE);
    init_pair(Color::pink_blue, COLOR_MAGENTA, COLOR_BLUE);
    init_pair(Color::orange_blue, COLOR_GREEN, COLOR_BLUE);
    init_pair(Color::cyan_blue, COLOR_CYAN, COLOR_BLUE);

    // init refresh
    refresh();
}

std::vector<Obstacle> obstacleInit(Window& gameW)
{

    // Initialize obstacle use format: 
    // {yoffset, xoffset}, {ydimension, xdimension}
    std::vector<Obstacle> obstacleList
    {
        { {2, 2}, {3, 4}, gameW },
        { {2, 7}, {3, 5}, gameW },
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
        { {12, 10}, {1, 3}, gameW },
        { {12, 15}, {1, 3}, gameW },
        { {13, 10}, {4, 1}, gameW },
        { {16, 11}, {1, 7}, gameW },
        { {13, 17}, {3, 1}, gameW }
    };

    return obstacleList;
}

MenuSelection menuLoop()
{
    // Init menu stuff
    Menu menu;
    menu.getMenuPerimeterWindow().drawBoxAndRefresh();
    menu.getWelcomeDisplayWindow().drawBoxAndRefresh();
    menu.printWelcomeDisplayAndRefresh();

    while(true)
    {
        menu.printOptions();
        wrefresh(menu.getMenuPerimeterWindow().getWindow());

        // if user selected an option return the option
        if(menu.getUserInputAndSetSelection())
            return menu.getMenuSelection();
    }
}

void gameLoop()
{
    // pacman's score
    static int score{};
    score = 0;

    // init game window stuff
    Window gameW{};
    nodelay(gameW.getWindow(), true);
    gameW.drawBoxAndRefresh();

    // init game obstacles stuff + print it 
    std::vector<Obstacle> obstacleList{obstacleInit(gameW)};

    // init vector space
    VectorSpace vectorSpace{gameW, obstacleList};
    gameW.assignGhostBox();

    // restart vector space | new round
    vectorSpace.assignPelletNotEaten(gameW);
    vectorSpace.printAndRefreshPellet(gameW);

    for(int lives{4}; lives > 0; --lives)
    {
        gameW.printLives(lives);
        gameW.printScoreOutline(score);

        vectorSpace.printAndRefreshPellet(gameW);

        // init characters
        Pacman pacman{};
        Ghost pinky{ 300ms, Color::pink_black };
        Ghost inky{ 325ms, Color::cyan_black };
        Ghost blinky{ 250ms, Color::red_black };
        Ghost clyde{ 275ms, Color::orange_black };

        gameW.gameCountDown();

        bool leaveLoop{ false };
        while(true)
        {
            switch(pacman.timeToMove(gameW, inky, blinky, pinky, clyde, score))
            {
                case LevelState::pacmanDead:
                    leaveLoop = true;
                    pacman.printDeathAnimation(gameW);
                    break;
                case LevelState::levelClear:
                    vectorSpace.assignPelletNotEaten(gameW);
                    gameW.flashingObstacles();
                    ++lives;
                    leaveLoop = true;
                    break;
            }

            if(leaveLoop)
                break;

            // if ghosts killed pacman break from loop
            if(!pinky.timeToMove(gameW, inky, blinky, clyde, pacman))
            {
                pacman.printDeathAnimation(gameW);
                break;
            }
            if(!inky.timeToMove(gameW, pinky, blinky, clyde, pacman))
            {
                pacman.printDeathAnimation(gameW);
                break;
            }
            if(!blinky.timeToMove(gameW, pinky, inky, clyde, pacman))
            {
                pacman.printDeathAnimation(gameW);
                break;
            }
            if(!clyde.timeToMove(gameW, pinky, inky, blinky, pacman))
            {
                pacman.printDeathAnimation(gameW);
                break;
            }
            // sleep to avoid redundant checks
            std::this_thread::sleep_for(5ms);
        }
    }

    gameW.printGameOverMessage();
}

int main()
{
    ncursesInit(); 

    bool closeGame{false};
    while(true)
    {
        switch(menuLoop())
        {
            case MenuSelection::start:
                gameLoop();
                break;
            case MenuSelection::controls:
                break;
            case MenuSelection::exit:
                closeGame = true;
                break;
        }

        if(closeGame)
            break;
    }
    
    endwin();
    return 0;
}

// *************************EASY********************************

/*
    1) Controls section
*/

// ************************MEDIUM*******************************

/*
    1) External high score
    2) Implement random (1-4) fruits spawning in each level
*/

// **************************HARD*******************************

/*
    1) Pacman menu animation
    2) Pathfinding
    3) Improve movement responsivness
    4) Fix unfrequent ghost flicker
    5) Power ups
*/


