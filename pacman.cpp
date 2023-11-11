#include <ncurses.h>
#include <clocale>

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
    static const int defaultGameX{80};
    static const int defaultGameY{35};
    // screenY: height of window
    int m_screenY{};
    // screenX: width of window
    int m_screenX{};
    WINDOW* m_window{};

public:
    // default constructor (centered window)
    Window(int screenY = defaultGameY, int screenX = defaultGameX)
    : m_screenY {screenY}
    , m_screenX {screenX}
    {
        int maxY{};
        int maxX{};
        getmaxyx(stdscr, maxY, maxX);
        m_window = newwin(m_screenY, m_screenX, (maxY - m_screenY) / 2, (maxX - m_screenX) / 2);
    }

    // paramatized constructor (custom window)
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
    std::vector<Vec> m_obsVec{};
    char m_obsCh{ACS_CKBOARD};
    
    // adds elements and assigns them to coordinates based on specified init values
    void initVector(Vec& startPoint, Vec& obsDimensions)
    {
        int totalVectors{obsDimensions.y * obsDimensions.x};

        // emplace elements to obsVec
        for (std::size_t i{0}; i < totalVectors; ++i)
        {
            m_obsVec.emplace_back(); 
        }

        // assigns coordinates to obsVec
        std::size_t vectorIncrement{0};
        for(std::size_t iY{0}; iY < obsDimensions.y; ++iY)
        {
            for(std::size_t iX{0}; iX < obsDimensions.x; ++iX)
            {
                m_obsVec[vectorIncrement].y = (iY + startPoint.y);

                m_obsVec[vectorIncrement].x = (iX + startPoint.x);

                ++vectorIncrement;
            }
        }
    }

    // prints and refreshes to " create " the object in the specified window
    void printAndRefreshObstacle(Window& win)
    {
        for(std::size_t i{0}; i < m_obsVec.size(); ++i)
        {
            mvwaddch(win.getWindow(), m_obsVec[i].y, m_obsVec[i].x, ACS_CKBOARD);
        }
        wrefresh(win.getWindow());
    }

public:
    Obstacle( Vec startPoint, Vec obsDimensions, Window& win)
    {
        initVector(startPoint, obsDimensions);
        printAndRefreshObstacle(win);
    }

    const std::vector<Vec>& getObsVec() { return m_obsVec; }
};

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

int main()
{
    ncursesInit(); 

    Window gameW{};
    wborder(gameW.getWindow(), ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD);
    wrefresh(gameW.getWindow());

    Obstacle ob1{Vec{10, 25}, Vec{8, 4}, gameW};
    Obstacle ob2{Vec{10, 10}, Vec{4, 4}, gameW};

    getch();
    endwin();
    return 0;
}