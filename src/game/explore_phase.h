#ifndef EXPLORE_PHASE_H
#define EXPLORE_PHASE_H

#define MAX_TEXT_INPUT_LENGTH 60
#define MAX_DESCRIPTION_LENGTH 2048

#define LIST_TYPE int
#include "list.h"

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

typedef char *charPtr;
#define LIST_TYPE charPtr
#include "list.h"

struct dungeon_item {
    int id;
    char *name;
    char *description;
    char *roomDescription;
    bool equippable;
    weapon_type weaponType;
    charPtr_list alternateNames;
};
#define LIST_TYPE dungeon_item
#include "list.h"


struct dungeon_room {
    int id;
    char *title;
    char *description;
    room_exit_list exits;

    bool hasChest;
    bool chestOpen;

    int numMonsters;
    monster_type monsterType;
    bool monstersDefeated;
    int spawnPositionsID;

    char *textToRead;

    int itemID;
};
#define LIST_TYPE dungeon_room
#include "list.h"

enum action_type {
    ACTION_TYPE_LOOK,
    ACTION_TYPE_MOVE,
    ACTION_TYPE_TAKE_ITEM,
    ACTION_TYPE_CHECK_INVENTORY, 
    ACTION_TYPE_EQUIP,
    ACTION_TYPE_EXAMINE_ITEM,
    ACTION_TYPE_READ,
    ACTION_TYPE_OPEN_CHEST,
    ACTION_TYPE_HELP,
    ACTION_TYPE_BAD_INPUT
};

struct explore_action {
    action_type type;

    room_exit_direction moveDirection;
    char target[MAX_TEXT_INPUT_LENGTH]; // for equipping items, examining items, etc.
};

enum explore_state {
    EXPLORE_STATE_REVEAL_TEXT,
    EXPLORE_STATE_INPUT
};

struct explore_game {
    explore_state state;
    dungeon_room_list allRooms;
    dungeon_room *currentRoom;
    dungeon_room *prevRoom;
    float revealTime;

    dungeon_item_list allItems;
    int_list inventory;
    int equippedItemID;

    bool isIntro;
    bool isTitle;
    char *currentTitleText;
    char *currentStatusText;

    int inputEnterY = 0;

    bool aboutToFight;
    bool justLost;
    bool gameWon;

    int numTypedLetters;
    char descriptionBuffer[MAX_DESCRIPTION_LENGTH];
    char currentText[MAX_TEXT_INPUT_LENGTH];

    char *soundToPlay;
};

#endif 
