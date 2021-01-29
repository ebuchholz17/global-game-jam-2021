#ifndef CONSOLE_GAME_H
#define CONSOLE_GAME_H

struct dungeon_room {
    char *title;
    char *description;
};
#define LIST_TYPE dungeon_room
#include "list.h"

enum console_game_state {
    CONSOLE_GAME_STATE_EXPLORE,
    CONSOLE_GAME_STATE_ADVENTURE
};

enum explore_state {
    EXPLORE_STATE_REVEAL_TEXT,
    EXPLORE_STATE_INPUT
};

struct explore_game {
    explore_state exploreState;
    dungeon_room_list allRooms;
    dungeon_room *currentRoom;
    float revealTime;
};

struct console_game {
    bool initialized;
    char currentColor;

    console_game_state gameState;

    explore_game exploreGame;
};


#endif
