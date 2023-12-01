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

    // {Y offset, X offset}, {Y dimension, X dimension}, window
    std::vector<Obstacle> obstacleList
    {
        // layer 1
        { {1, 13}, {4, 2}, gameW },

        // layer 2
        { {2, 2}, {3, 4}, gameW },
        { {2, 7}, {3, 5}, gameW },
        { {2, 16}, {3, 5}, gameW },
        { {2, 22}, {3, 4}, gameW },

        // layer 3
        { {6, 2}, {2, 4}, gameW },
        { {6, 7}, {8, 2}, gameW },
        { {6, 10}, {2, 8}, gameW },
        { {6, 19}, {8, 2}, gameW },
        { {6, 22}, {2, 4}, gameW },

        // layer 4
        { {8, 13}, {3, 2}, gameW },

        // layer 5
        { {9, 9}, {2, 3}, gameW },
        { {9, 16}, {2, 3}, gameW },
        { {9, 1}, {5, 5}, gameW },
        { {9, 22}, {5, 5}, gameW },

        // layer 6
        { {15, 22}, {5, 5}, gameW },
        { {15, 1}, {5, 5}, gameW },
        { {15, 7}, {5, 2}, gameW },
        { {15, 19}, {5, 2}, gameW },

        // layer 7
        { {18, 10}, {2, 8}, gameW },

        // layer 7.5
        { {20, 13}, {3, 2}, gameW },

        // layer 8
        { {21, 2}, {2, 4}, gameW },
        { {21, 7}, {2, 5}, gameW },
        { {21, 16}, {2, 5}, gameW },
        { {21, 22}, {2, 4}, gameW },

        // layer 9
        { {23, 4}, {3, 2}, gameW },
        { {23, 22}, {3, 2}, gameW },

        // layer 10
        { {24, 19}, {3, 2}, gameW },
        { {24, 1}, {2, 2}, gameW },
        { {24, 25}, {2, 2}, gameW },
        { {24, 7}, {3, 2}, gameW },
        { {24, 10}, {2, 8}, gameW },

        // layer 11
        { {26, 13}, {3, 2}, gameW },

        // layer 12
        { {27, 2}, {2, 10}, gameW },
        { {27, 16}, {2, 10}, gameW },

        // Ghost Box
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

    mvwprintw(gameW.getWindow(), 6, 5, "W A S D : MOVEMENT");
    mvwprintw(gameW.getWindow(), 8, 5, "ENTER   : SELECT");
    mvwprintw(gameW.getWindow(), 12, 5, "1000 POINTS = 1UP");
    mvwprintw(gameW.getWindow(), 14, 5, "GHOST = 20 POINTS");

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

bool powerPelletTimer(std::chrono::time_point<std::chrono::high_resolution_clock> lastTime, bool& powerPelletActive, std::chrono::milliseconds& interval)
{
    auto currentTime{ std::chrono::high_resolution_clock::now() };

    if(currentTime - lastTime <= interval && powerPelletActive)
    {
        return true;
    }
    else
    {
        powerPelletActive = false;
        return false;
    }

   return true;

}

void gameLoop(Window& gameW)
{
    // pacman's score
    int score{0};

    // init game window stuff
    nodelay(gameW.getWindow(), true);
    gameW.drawBoxAndRefresh();

    // init game obstacles stuff + print it 
    std::vector<Obstacle> obstacleList{obstacleInit(gameW)};

    // init vector space
    VectorSpace vectorSpace{gameW, obstacleList};
    gameW.assignGhostBox();

    // init characters
    Pacman pacman{gameW};
    Ghost clyde{};
    Blinky blinky{};
    Pinky pinky{};
    Inky inky{};

    // stuff for power pellet timer
    auto lastTime{ std::chrono::high_resolution_clock::now() }; 
    bool powerPelletActive{ false };
    auto interval{ std::chrono::duration(6000ms) };

    // 1up score stuff
    int levelUpCount{1};

    // restart vector space | new round
    vectorSpace.assignPelletNotEaten(gameW);

    for(int lives{4}; lives > 0; --lives)
    {
        // print lives and score
        gameW.printLives(lives);
        gameW.printScoreOutline(score);
        gameW.printHighScore();

        // print pellets based on what has been eaten
        vectorSpace.printPellet(gameW);
        vectorSpace.printPowerPellet(gameW);

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
            LevelState ateWhichGhost{LevelState::normalMovement};
            
            switch(pacman.timeToMove(gameW, clyde, inky, blinky, pinky, score, powerPelletActive, lastTime))
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
                    // quicker pp timer
                    if(interval != 2000ms)
                        interval -= 500ms;
                    leaveLoop = true;
                    break;
                case LevelState::powerUp:
                    lastTime = std::chrono::high_resolution_clock::now();
                    powerPelletActive = true;
                    break;
                case LevelState::ateBlinky:
                    ateWhichGhost = LevelState::ateBlinky;
                    break;
                case LevelState::ateInky:
                    ateWhichGhost = LevelState::ateInky;
                    break;
                case LevelState::atePinky:
                    ateWhichGhost = LevelState::atePinky;
                    break;
                case LevelState::ateClyde:
                    ateWhichGhost = LevelState::ateClyde;
                    break;
            }

            // If user gets 1000 points + 1 life
            if((score >= (1000 * levelUpCount)))
            {
                ++levelUpCount;

                if(lives != 4)
                {
                    ++lives;
                    gameW.printLives(lives);
                }
            }

            if(leaveLoop)
                break;

            // if timeToMove returns false Pacman was murdered
            
            if(!clyde.timeToMove(gameW, pinky, inky, blinky, pacman, powerPelletTimer(lastTime, powerPelletActive, interval), 
                ateWhichGhost, score, lastTime, interval))
            {
                pacman.printDeathAnimation(gameW);
                break;
            }
        
            if(!inky.timeToMove(gameW, pacman, pinky, blinky, clyde, powerPelletTimer(lastTime, powerPelletActive, interval), 
                ateWhichGhost, score, lastTime, interval))
            {
                pacman.printDeathAnimation(gameW);
                break;
            }
            
            if(!blinky.timeToMove(gameW, pacman, pinky, inky, clyde, powerPelletTimer(lastTime, powerPelletActive, interval), 
                ateWhichGhost, score, lastTime, interval))
            {
                pacman.printDeathAnimation(gameW);
                break;
            }
            
            if(!pinky.timeToMove(gameW, pacman, inky, blinky, clyde, powerPelletTimer(lastTime, powerPelletActive, interval), 
                ateWhichGhost, score, lastTime, interval))
            {
                pacman.printDeathAnimation(gameW);
                break;
            }

            // sleep to avoid redundant checks
            std::this_thread::sleep_for(5ms);
        }
    }

    gameW.setHighScore(score);
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
                clearWindow(gameW);
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
