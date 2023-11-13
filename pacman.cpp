#include <ncurses.h>
#include <clocale>

#include <chrono>
#include <thread>
#include <vector>
#include <thread>

using namespace std::chrono_literals;

enum class WindowState
{
    Menu,
    GameLoop,
};

enum class DirectionEval
{
    increment,
    decrement,
};

enum class Direction
{
    up,
    down,
    left,
    right,
    null,
};

struct Vec
{
    int y{};
    int x{};
};

class Window
{
private:
    static const int defaultGameX{35};
    static const int defaultGameY{35};
    // screenY: height of window
    int m_screenY{};
    // screenX: width of window
    int m_screenX{};
    WINDOW* m_window{};

    void drawBoxAndRefresh()
    {
        wborder(m_window, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD);
        wrefresh(m_window);
    }

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
        drawBoxAndRefresh();
    }

    // paramatized constructor (custom window)
    Window(int screenY, int screenX, int startY, int startX)
    : m_screenY {screenY}
    , m_screenX {screenX}
    {
        m_window = newwin(screenY, screenX, startY, startX);
        drawBoxAndRefresh();
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
    chtype obsCh{ACS_CKBOARD};
    
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
            mvwaddch(win.getWindow(), m_obsVec[i].y, m_obsVec[i].x, obsCh);
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

class Pacman
{
private:
    Direction m_direction{Direction::right};
    Vec m_pacVec{1, 1};
    char m_pacman{'@'};
    char m_userInput{};

    void setY(DirectionEval direction) 
    {
        if (direction == DirectionEval::decrement) --m_pacVec.y;
        else if (direction == DirectionEval::increment) ++m_pacVec.y;
    }

    void setX(DirectionEval direction)
    {
        if (direction == DirectionEval::decrement) --m_pacVec.x;
        else if (direction == DirectionEval::increment) ++m_pacVec.x;
    }

    void setDirection() 
    {
        switch(m_userInput)
        {
            case 'w': m_direction = Direction::up; break;
            case 'a': m_direction = Direction::left; break;
            case 's': m_direction = Direction::down; break;
            case 'd': m_direction = Direction::right; break;
            default: m_direction = Direction::null; break;
        }
    }

public:
    Pacman() = default;

    void printAndRefresh(Window& win)
    {
        mvwaddch(win.getWindow(), m_pacVec.y, m_pacVec.x, m_pacman);
        wrefresh(win.getWindow());
    }

    void erase(Window& win)
    {
        mvwprintw(win.getWindow(), m_pacVec.y, m_pacVec.x, " ");
    }

    void getUserInputAndSetDirection(Window& win)
    {
        m_userInput = wgetch(win.getWindow());
        if (m_userInput != ERR)
        {
            setDirection();
        }

        // clears extra input as to not cause input buffer
        while (wgetch(win.getWindow()) != ERR) {}
    }

    void movePacmanBasedOnDirection()
    {
        switch(m_direction)
            {
                case Direction::up: setY(DirectionEval::decrement); break;
                case Direction::down: setY(DirectionEval::increment); break;
                case Direction::left: setX(DirectionEval::decrement); break;
                case Direction::right: setX(DirectionEval::increment); break;
                default: break;
            }
    }
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
            p1.movePacmanBasedOnDirection();
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
    std::vector<Obstacle> obstacleList{obstacleInitAndRefresh(gameW)};

    // Start Game
    gameLoop(gameW);
    
    // wait for user input before exiting...
    getch();
    endwin();
    return 0;
}