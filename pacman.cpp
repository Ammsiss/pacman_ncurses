#include <iostream>
#include <ncurses.h>

int main()
{
    initscr();

    raw();
    noecho();
    curs_set(0);
    refresh();

    wprintw(stdscr, "Hello World!");
    
    getch();
    endwin();
    return 0;
}