#!/bin/bash

OUTPUT_PROGRAM="pacman"
SOURCES="src/y_main.cpp src/y_pacman.cpp src/y_window.cpp src/y_obstacle.cpp src/y_vectorspace.cpp src/y_ghost.cpp src/y_random.cpp src/y_menu.cpp src/y_blinky.cpp src/y_pinky.cpp src/y_inky.cpp src/color_utils.cpp src/ghost_utils.cpp"
FLAGS="-g -lncursesw" # -g for debugging information, and -lncursesw for linking with ncursesw

rm -f $OUTPUT_PROGRAM
g++ -Iinclude $SOURCES $FLAGS -o $OUTPUT_PROGRAM

if [ $? -eq 0 ]; then
    echo "Compilation successful, running the program:"
    ./$OUTPUT_PROGRAM
else
    echo "Compilation failed, exiting."
fi
