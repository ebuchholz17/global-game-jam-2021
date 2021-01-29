#ifndef CONSOLE_GAME_H
#define CONSOLE_GAME_H

#define GAME_WIDTH 70
#define GAME_HEIGHT 25

#include "console_drawing.cpp"
#include "explore_phase.cpp"

enum console_game_state {
    CONSOLE_GAME_STATE_EXPLORE,
    CONSOLE_GAME_STATE_COMBAT
};

struct console_game {
    bool initialized;
    char currentColor;

    int windowStartX;
    int windowStartY;
    console_game_state state;
    explore_game exploreGame;
};


#endif
