#pragma once

#include <vector>
#include <string>

#include "z_obstacle.h"
#include "z_window.h"

class Menu
{
private:
    Window m_welcomeDisplayWindow{5, 26};
    Window m_menuPerimeterWindow{};
    std::size_t m_selection{0};
    std::vector<std::string> m_options{"START", "CONTROLS", "EXIT"};
    char m_userInput{};
    MenuSelection m_userMenuSelection{};

    void clearOldSelection();

public:

    // getters
    Window& getWelcomeDisplayWindow();
    Window& getMenuPerimeterWindow();
    std::vector<std::string> getOptions();
    std::size_t getSelection();
    MenuSelection getMenuSelection();


    void printWelcomeDisplayAndRefresh();
    void printOptions();
    bool getUserInputAndSetSelection();
    void setUserSelection();
};