#ifndef EXPLORE_PHASE_H
#define EXPLORE_PHASE_H

#define MAX_TEXT_INPUT_LENGTH 60


enum room_exit_direction {
    ROOM_EXIT_DIRECTION_NORTH,
    ROOM_EXIT_DIRECTION_SOUTH,
    ROOM_EXIT_DIRECTION_EAST,
    ROOM_EXIT_DIRECTION_WEST,
    ROOM_EXIT_DIRECTION_UP,
    ROOM_EXIT_DIRECTION_DOWN
};

struct room_exit {
    int connectedRoomID;
    room_exit_direction direction;
};
#define LIST_TYPE room_exit
#include "list.h"

struct dungeon_room {
    int id;
    char *title;
    char *description;
    room_exit_list exits;
};
#define LIST_TYPE dungeon_room
#include "list.h"

enum action_type {
    ACTION_TYPE_LOOK,
    ACTION_TYPE_MOVE,
    ACTION_TYPE_BAD_INPUT
};

struct explore_action {
    action_type type;

    room_exit_direction moveDirection;
};

enum explore_state {
    EXPLORE_STATE_REVEAL_TEXT,
    EXPLORE_STATE_INPUT
};

struct explore_game {
    explore_state state;
    dungeon_room_list allRooms;
    dungeon_room *currentRoom;
    float revealTime;

    bool isIntro;
    char *currentTitleText;
    char *currentStatusText;

    int inputEnterY = 0;

    int numTypedLetters;
    char currentText[MAX_TEXT_INPUT_LENGTH];
};


#endif
