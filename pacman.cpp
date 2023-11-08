#include <ncurses.h>

#include <chrono>
#include <thread>
#include <vector>

enum class WindowState
{
    Menu,
    GameLoop,
};

struct Vec
{
    int y{};
    int x{};
};

class Window
{
private:
    int m_screenY{};
    int m_screenX{};
    WINDOW* m_window{};

public:
    // default constructor
    Window()
    : m_screenY {35}
    , m_screenX {80}
    {
        int maxY{};
        int maxX{};
        getmaxyx(stdscr, maxY, maxX);
        m_window = newwin(m_screenY, m_screenX, (maxY - m_screenY) / 2, (maxX - m_screenX) / 2);
    }

    Window(int screenY, int screenX, int startY, int startX)
    : m_screenY {screenY}
    , m_screenX {screenX}
    {
        m_window = newwin(screenY, screenX, startY, startX);
    }

    // destructor
    ~Window() { delwin(m_window); }

    // getters
    WINDOW* getWindow() { return m_window; }
    const int& getScreenY() { return m_screenY; }
    const int& getScreenX() { return m_screenX; }
};

class Obstacle
{
private:
    std::vector<Vec> obVec{};
    char obCh{'#'};
};

void ncursesInit()
{
    // set up ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0);    
    refresh();
}

int main()
{
    ncursesInit(); 
    Window game{};

    Window game2{2, 4, 0, 0};

    box(game.getWindow(), 0, 0);
    wrefresh(game.getWindow());

    box(game2.getWindow(), 0, 0);
    wrefresh(game2.getWindow());

    getch();
    endwin();
    return 0;
}