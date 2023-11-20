#!/bin/bash

OUTPUT_PROGRAM="pacman"
SOURCES="y_main.cpp y_pacman.cpp y_window.cpp y_obstacle.cpp y_vectorspace.cpp y_ghost.cpp y_random.cpp y_menu.cpp"
FLAGS="-g -lncursesw" # -g for debugging information, and -lncursesw for linking with ncursesw

rm -f $OUTPUT_PROGRAM
g++ $SOURCES $FLAGS -o $OUTPUT_PROGRAM

if [ $? -eq 0 ]; then
    echo "Compilation successful, running the program:"
    ./$OUTPUT_PROGRAM
else
    echo "Compilation failed, exiting."
fi