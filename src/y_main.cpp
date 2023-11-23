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
#include "z_blinky.h"
#include "z_pinky.h"
#include "z_inky.h"

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

void controls(Window& gameW)
{
    gameW.drawBoxAndRefresh();

    wattron(gameW.getWindow(), COLOR_PAIR(Color::yellow_black));

    mvwprintw(gameW.getWindow(), 10, 5, "W A S D : MOVEMENT");
    mvwprintw(gameW.getWindow(), 12, 5, "ENTER   : SELECT");

    wattroff(gameW.getWindow(), COLOR_PAIR(Color::yellow_black));
    wattron(gameW.getWindow(), COLOR_PAIR(Color::default_color));

    wattron(gameW.getWindow(), COLOR_PAIR(Color::blue_black));

    mvwprintw(gameW.getWindow(), 18, 1, "==========================");
    mvwprintw(gameW.getWindow(), 23, 1, "==========================");

    wattroff(gameW.getWindow(), COLOR_PAIR(Color::blue_black));
    wattron(gameW.getWindow(), COLOR_PAIR(Color::default_color));

    wattron(gameW.getWindow(), COLOR_PAIR(Color::yellow_black));

    mvwprintw(gameW.getWindow(), 19, 14, " .--.       ");
    mvwprintw(gameW.getWindow(), 20, 14, "/ _.-' .-.  ");
    mvwprintw(gameW.getWindow(), 21, 14, "\\  '-. '-' ");
    mvwprintw(gameW.getWindow(), 22, 14, " '--'       ");

    wattroff(gameW.getWindow(), COLOR_PAIR(Color::yellow_black));
    wattron(gameW.getWindow(), COLOR_PAIR(Color::default_color));

    wattron(gameW.getWindow(), COLOR_PAIR(Color::cyan_black));

    mvwprintw(gameW.getWindow(), 19, 2, " .-.  ");
    mvwprintw(gameW.getWindow(), 20, 2, "| OO| ");
    mvwprintw(gameW.getWindow(), 21, 2, "|   | ");
    mvwprintw(gameW.getWindow(), 22, 2, "'^^^' ");

    wattroff(gameW.getWindow(), COLOR_PAIR(Color::cyan_black));
    wattron(gameW.getWindow(), COLOR_PAIR(Color::default_color));

    wattron(gameW.getWindow(), COLOR_PAIR(Color::red_black));

    mvwprintw(gameW.getWindow(), 19, 7, " .-. ");
    mvwprintw(gameW.getWindow(), 20, 7, "| OO|");
    mvwprintw(gameW.getWindow(), 21, 7, "|   |");
    mvwprintw(gameW.getWindow(), 22, 7, "'^^^'");

    wattroff(gameW.getWindow(), COLOR_PAIR(Color::red_black));
    wattron(gameW.getWindow(), COLOR_PAIR(Color::default_color));

    wrefresh(gameW.getWindow());

    char input{};
    while(true)
    {
        input = wgetch(gameW.getWindow());
        if(input == '\n')
            break;
    }
}

void clearWindow(Window& gameW)
{
    wclear(gameW.getWindow());
    wrefresh(gameW.getWindow());
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

void gameLoop(Window& gameW)
{
    // pacman's score
    static int score{};
    score = 0;

    // init game window stuff
    nodelay(gameW.getWindow(), true);
    gameW.drawBoxAndRefresh();

    // init game obstacles stuff + print it 
    std::vector<Obstacle> obstacleList{obstacleInit(gameW)};

    // init vector space
    VectorSpace vectorSpace{gameW, obstacleList};
    gameW.assignGhostBox();

    Pacman pacman{gameW};
    Ghost clyde{};
    Blinky blinky{};
    Pinky pinky{};
    Inky inky{};

    // restart vector space | new round
    vectorSpace.assignPelletNotEaten(gameW);

    for(int lives{4}; lives > 0; --lives)
    {
        // print lives and score
        gameW.printLives(lives);
        gameW.printScoreOutline(score);

        // print pellets based on what has been eaten
        vectorSpace.printPellet(gameW);

        // set pac back to start + print
        pacman.setPacVec();
        pacman.printInitialPacman(gameW);

        // set ghosts back to start + print
        clyde.setGhostVec();
        clyde.printGhost(gameW);

        inky.setGhostVec();
        inky.printGhost(gameW);

        pinky.setGhostVec();
        pinky.printGhost(gameW);

        blinky.setGhostVec();
        blinky.printGhost(gameW);
        // done

        wrefresh(gameW.getWindow());

        gameW.gameCountDown();

        bool leaveLoop{ false };
        while(true)
        {
            switch(pacman.timeToMove(gameW, clyde, inky, blinky, pinky, score))
            {
                case LevelState::pacmanDead:
                    leaveLoop = true;
                    pacman.printDeathAnimation(gameW);
                    break;
                case LevelState::levelClear:
                    vectorSpace.assignPelletNotEaten(gameW);
                    gameW.flashingObstacles();
                    ++lives;
                    // +5ms speed
                    clyde.setSpeed();
                    inky.setSpeed();
                    blinky.setSpeed();
                    pinky.setSpeed();
                    leaveLoop = true;
                    break;
            }

            // If user gets 1000 points + 1 life
            if((score % 1000 == 0) && lives != 4)
            {
                ++lives;
                gameW.printLives(lives);
            }

            if(leaveLoop)
                break;

            // if ghosts killed pacman break from loop
            if(!clyde.timeToMove(gameW, pinky, inky, blinky, pacman))
            {
                pacman.printDeathAnimation(gameW);
                break;
            }
        
            if(!inky.timeToMove(gameW, pacman, pinky, blinky, clyde))
            {
                pacman.printDeathAnimation(gameW);
                break;
            }
            
            if(!blinky.timeToMove(gameW, pacman, pinky, inky, clyde))
            {
                pacman.printDeathAnimation(gameW);
                break;
            }
            
            if(!pinky.timeToMove(gameW, pacman, inky, blinky, clyde))
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

    Window gameW{};

    bool closeGame{false};
    while(true)
    {
        switch(menuLoop())
        {
            case MenuSelection::start:
                gameLoop(gameW);
                break;
            case MenuSelection::controls:
                controls(gameW);
                clearWindow(gameW);
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
    1) timed release
*/

// ************************MEDIUM*******************************

/*
    1) External high score
    2) Implement random (1-4) fruits spawning in each level
*/

// **************************HARD*******************************

/*
    1) Pacman menu animation
    3) Improve movement responsivness
    4) Fix unfrequent ghost flicker
    5) Power ups
    6) Implement pathfinding through portals
*/


