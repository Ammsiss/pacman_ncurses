#pragma once

//user
#include "z_obstacle.h"
#include "z_window.h"

//std
#include <vector>
#include <string>
class Menu
{
public:
    // public methods
    void printOptions();
    bool getUserInputAndSetSelection();
    void setUserSelection();
    void printWelcomeDisplayAndRefresh();

    // getters
    Window& getWelcomeDisplayWindow();
    Window& getMenuPerimeterWindow();
    std::vector<std::string> getOptions();
    std::size_t getSelection();
    MenuSelection getMenuSelection();

private:
    //private variables
    Window m_welcomeDisplayWindow{5, 26};
    Window m_menuPerimeterWindow{};
    std::size_t m_selection{0};
    std::vector<std::string> m_options{"START", "CONTROLS", "EXIT"};
    char m_userInput{};
    MenuSelection m_userMenuSelection{};

    //private methods
    void clearOldSelection();
};