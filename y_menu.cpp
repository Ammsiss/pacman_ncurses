#include "z_window.h"
#include "z_menu.h"

#include <vector>

// private:

void Menu::clearOldSelection()
{
    mvwprintw(m_menuPerimeterWindow.getWindow(), 22, 9, " ");
    mvwprintw(m_menuPerimeterWindow.getWindow(), 24, 9, " ");
    mvwprintw(m_menuPerimeterWindow.getWindow(), 26, 9, " ");
}

// public:

Window& Menu::getWelcomeDisplayWindow() { return m_welcomeDisplayWindow; }
Window& Menu::getMenuPerimeterWindow() { return m_menuPerimeterWindow; }
std::vector<std::string> Menu::getOptions() { return m_options; }
std::size_t Menu::getSelection() { return m_selection; }
MenuSelection Menu::getMenuSelection() { return m_userMenuSelection; }

void Menu::printOptions()
{
    for(std::size_t i{0}; i < m_options.size(); ++i)
    {
        // joke
        wattron(m_menuPerimeterWindow.getWindow(), COLOR_PAIR(Color::red_black));
        mvwprintw(m_menuPerimeterWindow.getWindow(), 18, 26, "™");
        wattroff(m_menuPerimeterWindow.getWindow(), COLOR_PAIR(Color::red_black));
        wattron(m_menuPerimeterWindow.getWindow(), COLOR_PAIR(Color::default_color));
        //

        if(m_selection == i)
        {
            wattron(m_menuPerimeterWindow.getWindow(), COLOR_PAIR(Color::yellow_black));
            mvwprintw(m_menuPerimeterWindow.getWindow(), 22 + (2 * i), 9, "%s", "<");
            wattroff(m_menuPerimeterWindow.getWindow(), COLOR_PAIR(Color::yellow_black));
            wattron(m_menuPerimeterWindow.getWindow(), COLOR_PAIR(Color::default_color));

            wattron(m_menuPerimeterWindow.getWindow(), COLOR_PAIR(Color::red_black));
            mvwprintw(m_menuPerimeterWindow.getWindow(), (22 + (2 * i)), 11, "%s", m_options[i].c_str());
            wattroff(m_menuPerimeterWindow.getWindow(), COLOR_PAIR((COLOR_PAIR(Color::red_black)))); 
            wattron(m_menuPerimeterWindow.getWindow(), COLOR_PAIR(Color::default_color));
        }
        else
        {
            wattron(m_menuPerimeterWindow.getWindow(), COLOR_PAIR(Color::red_black));
            mvwprintw(m_menuPerimeterWindow.getWindow(), (22 + (2 * i)), 11, "%s", m_options[i].c_str());
            wattroff(m_menuPerimeterWindow.getWindow(), COLOR_PAIR(Color::red_black));
            wattron(m_menuPerimeterWindow.getWindow(), COLOR_PAIR(Color::default_color));
        }
    }
}

bool Menu::getUserInputAndSetSelection()
{
    m_userInput = wgetch(m_menuPerimeterWindow.getWindow());

    clearOldSelection();

    switch(m_userInput)
    {
        case 'w':
            if (m_selection != 0)
                --m_selection;
            break;
        case 's':
            if (m_selection != 2)
                ++m_selection;
            break;
        case '\n':
            setUserSelection();
            return true;
        default:
            break;
    }

    return false;
}

void Menu::setUserSelection()
{
    if(m_selection == 0)
        m_userMenuSelection = MenuSelection::start;
    else if(m_selection == 1)
        m_userMenuSelection = MenuSelection::controls;
    else if(m_selection == 2)
        m_userMenuSelection = MenuSelection::exit;

}

void Menu::printWelcomeDisplayAndRefresh()


{
    wattron(m_welcomeDisplayWindow.getWindow(), COLOR_PAIR(Color::yellow_black));

    // P

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 0, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 1, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 2, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 1, 0, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 1, 2, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 0, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 1, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 2, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 3, 0, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 4, 0, ACS_CKBOARD);

    // A

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 4, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 5, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 6, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 1, 4, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 1, 6, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 4, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 5, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 6, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 3, 4, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 3, 6, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 4, 4, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 4, 6, ACS_CKBOARD);

    // C

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 8, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 9, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 10, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 1, 8, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 8, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 3, 8, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 4, 8, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 4, 9, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 4, 10, ACS_CKBOARD);

    // M
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 12, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 13, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 15, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 16, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 1, 12, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 1, 14, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 1, 16, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 12, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 14, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 16, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 3, 12, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 3, 14, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 3, 16, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 4, 12, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 4, 14, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 4, 16, ACS_CKBOARD);

    // A

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 18, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 19, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 20, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 1, 18, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 1, 20, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 18, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 19, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 20, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 3, 18, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 3, 20, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 4, 18, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 4, 20, ACS_CKBOARD);

    // N

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 22, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 0, 25, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 1, 22, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 1, 23, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 1, 25, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 22, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 24, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 2, 25, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 3, 22, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 3, 25, ACS_CKBOARD);

    mvwaddch(m_welcomeDisplayWindow.getWindow(), 4, 22, ACS_CKBOARD);
    mvwaddch(m_welcomeDisplayWindow.getWindow(), 4, 25, ACS_CKBOARD);

    // PACMAN!

    wattroff(m_welcomeDisplayWindow.getWindow(), COLOR_PAIR(Color::yellow_black));
    wattron(m_welcomeDisplayWindow.getWindow(), COLOR_PAIR(Color::default_color));

    wrefresh(m_welcomeDisplayWindow.getWindow());
}

