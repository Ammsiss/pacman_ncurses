#include <ncurses.h>

#include <chrono>
#include <thread>

class Game
{
private:
    static const int screenY{30};
    static const int screenX{80};
    int maxY{};
    int maxX{};
    WINDOW* gameW{};

public:
    ~Game() { delwin(gameW); }

    // getters
    WINDOW* getGameW() { return gameW; }
    const int& getScreenY() { return screenY; }
    const int& getScreenX() { return screenX; }

    void setScreen()
    {
        getmaxyx(stdscr, maxY, maxX);
    }

    void setGameW()
    {
        gameW = newwin(screenY, screenX, (maxY - screenY) / 2, (maxX - screenX) / 2);
    }
};

void gameInit(Game& game)
{
    // set up ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0);    
    refresh();

    game.setScreen();
    game.setGameW();

    box(game.getGameW(), 0, 0);
    wrefresh(game.getGameW());
}

int main()
{
    Game game{};
    gameInit(game); 


    getch();
    endwin();
    return 0;
}