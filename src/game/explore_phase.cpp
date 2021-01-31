#include "explore_phase.h"
#include "room_data.cpp"

int stringLength (char *string) {
    int length = 0;
    char *cursor = string;
    while (*cursor != 0) {
        ++cursor;
        ++length;
    }

    return length;
}

void initExplorePhase (explore_game *exploreGame, memory_arena *memory) {
    char *titleText = R"room(Welcome!)room";
    char *introText = R"room(You have stumbled upon the ruins of a great castle, its glory days 
long-past. You can feel the magic in the air. Your thoughts seem to 
have a certain... "tangibility" here. But focus on the task at hand!

Lost treasures await within if you can brave the dangers. But can 
you find the exit?

Press ENTER to continue.)room";

    buildItems(exploreGame, memory);
    buildRooms(exploreGame, memory);
    exploreGame->currentRoom = &exploreGame->allRooms.values[0]; // QQQ
    exploreGame->currentTitleText = titleText;
    exploreGame->currentStatusText = introText;
    exploreGame->isIntro = true;
    exploreGame->revealTime = 0.0f;
    exploreGame->state = EXPLORE_STATE_REVEAL_TEXT;

    exploreGame->inventory = intListInit(memory, 10);
    exploreGame->equippedItemID = -1;
}

void drawStatusText (explore_game *exploreGame, console_drawer *drawer, int numLettersRevealed) {
    drawer->color = CONSOLE_COLOR_WHITE;
    char *cursor = exploreGame->currentStatusText;
    int currentLetterIndex = 0;
    int currentX = 1;
    int currentY = 2;
    while (*cursor != 0 && currentLetterIndex < numLettersRevealed) {
        if (*cursor == '\n') {
            currentX = 1;
            currentY++;
        }
        else {
            drawCharAtXY(*cursor, currentX, currentY, drawer);
            currentX++;
        }
        ++cursor;
        ++currentLetterIndex;
    }
}

bool revealStatusText (explore_game *exploreGame, console_drawer *drawer) {
    bool allTextRevealed = false;
    exploreGame->revealTime += DELTA_TIME;

    int textLength = stringLength(exploreGame->currentStatusText);
    float totalTime = (float)textLength * (1.0f / 100.0f);
    
    if (exploreGame->revealTime >= totalTime) {
        exploreGame->revealTime = totalTime;
        allTextRevealed = true;
    }
    int numLettersRevealed = (int)((exploreGame->revealTime / totalTime) * textLength);
    drawStatusText(exploreGame, drawer, numLettersRevealed);

    return allTextRevealed;
}

void drawTitleText (explore_game *exploreGame, console_drawer *drawer) {
    drawer->color = CONSOLE_COLOR_MAGENTA;
    int currentX = 1;
    char *cursor = exploreGame->currentTitleText;
    while (*cursor != 0) {
        drawCharAtXY(*cursor, currentX, 1, drawer);
        ++currentX;
        ++cursor;
    }
}

int getDescriptionHeight (explore_game *exploreGame) {
    char *cursor = exploreGame->currentStatusText;
    int descriptionHeight = 0;
    while (*cursor != 0) {
        if (*cursor == '\n') {
            ++descriptionHeight;
        }
        ++cursor;
    }
    return descriptionHeight;
}

void drawFightText (explore_game *exploreGame, console_drawer *drawer) {
    int descriptionHeight = getDescriptionHeight(exploreGame);

    int inputY = descriptionHeight + 4; // skip title, add white space
    drawCharAtXY('>', 1, inputY, drawer);
    char *fightText = "FIGHT";
    for (int i = 0; i < 5; ++i) {
        drawCharAtXY(fightText[i], i + 2, inputY, drawer);
    }
}

bool updatePlayerInput (explore_game *exploreGame, game_input *input, console_drawer *drawer) {
    // determine where to draw the input
    int descriptionHeight = getDescriptionHeight(exploreGame);
    int inputY = descriptionHeight + 4; // skip title, add white space

    if (input->numTypedChars > 0) {
        int typedLetterIndex = 0;
        while (exploreGame->numTypedLetters < MAX_TEXT_INPUT_LENGTH && typedLetterIndex < input->numTypedChars) {
            exploreGame->currentText[exploreGame->numTypedLetters++] = input->typedText[typedLetterIndex];
            ++typedLetterIndex;
        }
    }
    if (input->backspaceDown) {
        if (exploreGame->numTypedLetters > 0) {
            exploreGame->numTypedLetters -= 1;
        }
    }

    drawCharAtXY('>', 1, inputY, drawer);
    for (int i = 0; i < exploreGame->numTypedLetters; ++i) {
        drawCharAtXY(exploreGame->currentText[i], i + 2, inputY, drawer);
    }

    static bool blinkOn = false;
    static float blinkTimer = 0.0f;

    blinkTimer += DELTA_TIME;
    while (blinkTimer >= 0.15f) {
        blinkTimer -= 0.15f;
        blinkOn = !blinkOn;
    }

    if (blinkOn) {
        drawCharAtXY('_', exploreGame->numTypedLetters + 2, inputY, drawer);
    }

    return input->enterKey.justPressed;
}

char *readRestOfLine (char *cursor, char *currentWord, int inputLength) {
    int letterIndex = 0;
    while (((*cursor >= 'A' && *cursor <= 'Z') || *cursor == ' ') && *cursor != '\n' && *cursor != 0 && letterIndex < inputLength) {
        currentWord[letterIndex] = *cursor;
        letterIndex++;
        cursor++;
    }
    currentWord[letterIndex] = 0;
    return cursor;
}

char *readInputWord (char *cursor, char *currentWord, int inputLength) {
    int letterIndex = 0;
    while ((*cursor >= 'A' && *cursor <= 'Z') && *cursor != '\n' && *cursor != 0 && letterIndex < inputLength) {
        currentWord[letterIndex] = *cursor;
        letterIndex++;
        cursor++;
    }
    currentWord[letterIndex] = 0;
    return cursor;
}

bool parseMoveDirection (char *input, explore_action *result) {
    if (stringsAreEqual(input, "N") || stringsAreEqual(input, "NORTH")) {
        result->moveDirection = ROOM_EXIT_DIRECTION_NORTH;
        return true;
    }
    else if (stringsAreEqual(input, "S") || stringsAreEqual(input, "SOUTH")) {
        result->moveDirection = ROOM_EXIT_DIRECTION_SOUTH;
        return true;
    }
    else if (stringsAreEqual(input, "E") || stringsAreEqual(input, "EAST")) {
        result->moveDirection = ROOM_EXIT_DIRECTION_EAST;
        return true;
    }
    else if (stringsAreEqual(input, "W") || stringsAreEqual(input, "WEST")) {
        result->moveDirection = ROOM_EXIT_DIRECTION_WEST;
        return true;
    }
    else if (stringsAreEqual(input, "U") || stringsAreEqual(input, "UP") || stringsAreEqual(input, "UPWARD")) {
        result->moveDirection = ROOM_EXIT_DIRECTION_UP;
        return true;
    }
    else if (stringsAreEqual(input, "D") || stringsAreEqual(input, "DOWN") || stringsAreEqual(input, "DOWNWARD")) {
        result->moveDirection = ROOM_EXIT_DIRECTION_DOWN;
        return true;
    }

    return false;
}

bool parseMoveAction (char *input, char *cursor, int inputLength, explore_action *result) {
    bool enteredDirection = parseMoveDirection(input, result);
    if (enteredDirection) {
        return true;
    }
    else {
        if (stringsAreEqual(input, "GO") || stringsAreEqual(input, "MOVE") || 
            stringsAreEqual(input, "WALK") || stringsAreEqual(input, "RUN")) 
        {
            char nextWord[MAX_TEXT_INPUT_LENGTH];
            char *nextWordCursor = cursor + 1; // skip space
            nextWordCursor = readInputWord(nextWordCursor, nextWord, inputLength - 1);

            enteredDirection = parseMoveDirection(nextWord, result);
            return enteredDirection;
        }
        else {
            return false;
        }
    }
}

explore_action processPlayerInput (explore_game *exploreGame) {
    char inputWord[MAX_TEXT_INPUT_LENGTH];
    char *cursor = exploreGame->currentText;
    cursor = readInputWord(cursor, inputWord, exploreGame->numTypedLetters);

    explore_action result = {};

    if (stringsAreEqual(inputWord, "LOOK") || stringsAreEqual(inputWord, "L")) {
        result.type = ACTION_TYPE_LOOK;
    }
    else if (stringsAreEqual(inputWord, "TAKE") || stringsAreEqual(inputWord, "GET") || stringsAreEqual(inputWord, "PICK")) {
        result.type = ACTION_TYPE_TAKE_ITEM;
    }
    else if (stringsAreEqual(inputWord, "INVENTORY") || stringsAreEqual(inputWord, "I")) {
        result.type = ACTION_TYPE_CHECK_INVENTORY;
    }
    else if (stringsAreEqual(inputWord, "EQUIP")) {
        result.type = ACTION_TYPE_EQUIP;
        readRestOfLine(cursor + 1, result.target, exploreGame->numTypedLetters - stringLength(inputWord) - 1);
    }
    else if (parseMoveAction(inputWord, cursor, exploreGame->numTypedLetters - stringLength(inputWord), &result)) {
        result.type = ACTION_TYPE_MOVE;
    }
    else if (stringsAreEqual(inputWord, "OPEN") || stringsAreEqual(inputWord, "UNLOCK")) {
        result.type = ACTION_TYPE_OPEN_CHEST;
    }
    else if (stringsAreEqual(inputWord, "READ")) {
        result.type = ACTION_TYPE_READ;
    }
    else {
        result.type = ACTION_TYPE_BAD_INPUT;
    }

    return result;
}

bool processPlayerYesNo (explore_game *exploreGame) {
    char inputWord[MAX_TEXT_INPUT_LENGTH];
    char *cursor = exploreGame->currentText;
    cursor = readInputWord(cursor, inputWord, exploreGame->numTypedLetters);

    if (stringsAreEqual(inputWord, "YES") || stringsAreEqual(inputWord, "Y")) {
        return true;
    }
    else if (stringsAreEqual(inputWord, "NO") || stringsAreEqual(inputWord, "N")) {
        return false;
    }

    return false;
}

void copyDescriptionToBuffer (char *description, char *buffer) {
    // zero out buffer
    char *cursor = buffer;
    while (*cursor != 0) {
        *cursor = 0; 
        ++cursor;
    }

    cursor = description;
    int index = 0;
    while (*cursor != 0) {
        assert(index < MAX_DESCRIPTION_LENGTH);
        buffer[index] = *cursor;
        ++cursor;
        ++index;
    }
}

void updateCurrentRoomDescription (explore_game *exploreGame, memory_arena *stringMemory) {
    exploreGame->currentTitleText = exploreGame->currentRoom->title;
    exploreGame->currentStatusText = exploreGame->currentRoom->description;

    bool copyToDescriptionBuffer = false;
    if (exploreGame->currentRoom->itemID != -1) {
        dungeon_item *item = &exploreGame->allItems.values[exploreGame->currentRoom->itemID];
        exploreGame->currentStatusText = appendString(exploreGame->currentStatusText, "\n\n", stringMemory);
        exploreGame->currentStatusText = appendString(exploreGame->currentStatusText, item->roomDescription, stringMemory);
        copyToDescriptionBuffer = true;
    }

    if (exploreGame->currentRoom->numMonsters > 0 && 
        !exploreGame->currentRoom->monstersDefeated) 
    {
        char *monsterText = "\n\n";
        switch (exploreGame->currentRoom->monsterType) {
            case MONSTER_TYPE_GOBLIN: {
                int numMonsters = exploreGame->currentRoom->numMonsters;
                if (numMonsters == 1) {
                    monsterText = appendString(monsterText, "A goblin emerges from the darkness and ambushes you!", stringMemory);
                }
                else {
                    char *numberText = numToString(numMonsters, stringMemory);
                    monsterText = appendString(monsterText, numberText, stringMemory);
                    monsterText = appendString(monsterText, " goblins emerge from the darkness and ambush you!", stringMemory);
                }
            } break;
            case MONSTER_TYPE_GIANT_SNAKE: {
                monsterText = appendString(monsterText, " A giant snake slithers across your path!", stringMemory);
            } break;
            case MONSTER_TYPE_DRAGON: {
                monsterText = appendString(monsterText, "A ferocious fire-breathing dragon is guarding its treasure!", stringMemory);
            } break;
        }
        exploreGame->currentStatusText = appendString(exploreGame->currentStatusText, monsterText, stringMemory);
        copyToDescriptionBuffer = true;
    }

    if (exploreGame->currentRoom->hasChest && !exploreGame->currentRoom->chestOpen) {
        char *chestText = "\n\n";
        chestText = appendString(chestText, "There is a treasure chest here, covered in glowing runic symbols.", stringMemory);
        exploreGame->currentStatusText = appendString(exploreGame->currentStatusText, chestText, stringMemory);
        copyToDescriptionBuffer = true;
    }

    if (copyToDescriptionBuffer) {
        copyDescriptionToBuffer(exploreGame->currentStatusText, exploreGame->descriptionBuffer);
        exploreGame->currentStatusText = exploreGame->descriptionBuffer;
    }
}

void moveToRoom (explore_game *exploreGame, explore_action action, memory_arena *stringMemory) {
    room_exit_list *exits = &exploreGame->currentRoom->exits;

    int nextRoomID = -1;
    for (int i = 0; i < exits->numValues; ++i) {
        room_exit *exit = &exits->values[i];
        if (exit->direction == action.moveDirection) {
            nextRoomID = exit->connectedRoomID;
            break;
        }
    }

    if (nextRoomID != -1) {
        assert(nextRoomID >= 0 && nextRoomID < exploreGame->allRooms.numValues);
        
        // reset goblins
        if (exploreGame->currentRoom->monsterType == MONSTER_TYPE_GOBLIN) {
            exploreGame->currentRoom->monstersDefeated = false;
        }

        exploreGame->prevRoom = exploreGame->currentRoom;
        exploreGame->currentRoom = &exploreGame->allRooms.values[nextRoomID];
        updateCurrentRoomDescription(exploreGame, stringMemory);
    }
    else {
        exploreGame->currentStatusText = "You can't go that way.";
    }
}

void takeItemIfExists (explore_game *exploreGame, explore_action action, memory_arena *stringMemory) {
    if (exploreGame->currentRoom->itemID != -1) {
        dungeon_item *item = &exploreGame->allItems.values[exploreGame->currentRoom->itemID];
        char *statusText;
        if (item->equippable) {
            statusText = appendString("You pick up and equip the ", item->name, stringMemory);
            exploreGame->equippedItemID = item->id;
        }
        else {
            statusText = appendString("You pick up the ", item->name, stringMemory);
        }
        statusText = appendString(statusText, ".", stringMemory);
        copyDescriptionToBuffer(statusText, exploreGame->descriptionBuffer);
        exploreGame->currentStatusText = exploreGame->descriptionBuffer;
        listPush(&exploreGame->inventory, item->id);

        exploreGame->currentRoom->itemID = -1;
    }
    else {
        exploreGame->currentStatusText = "There is nothing to take.";
    }
}

void printInventory (explore_game *exploreGame, memory_arena *stringMemory) {
    if (exploreGame->inventory.numValues == 0) {
        exploreGame->currentStatusText = "You aren't carrying any items.";
    }
    else {
        char *inventoryText = "You are carrying:\n";

        for (int i = 0; i < exploreGame->inventory.numValues; ++i) {
            int itemID = exploreGame->inventory.values[i];
            dungeon_item *item = &exploreGame->allItems.values[itemID];
            inventoryText = appendString(inventoryText, "\nA ", stringMemory);
            inventoryText = appendString(inventoryText, item->name, stringMemory);
            inventoryText = appendString(inventoryText, ".", stringMemory);
        }

        if (exploreGame->equippedItemID != -1) {
            dungeon_item *item = &exploreGame->allItems.values[exploreGame->equippedItemID];
            inventoryText = appendString(inventoryText, "\n\nYou have equipped a ", stringMemory);
            inventoryText = appendString(inventoryText, item->name, stringMemory);
            inventoryText = appendString(inventoryText, ".", stringMemory);
        }
        else {
            inventoryText = appendString(inventoryText, "\n\nYou have nothing equipped.", stringMemory);
        }

        copyDescriptionToBuffer(inventoryText, exploreGame->descriptionBuffer);
        exploreGame->currentStatusText = exploreGame->descriptionBuffer;
    }
}

void equipItem (explore_game *exploreGame, explore_action action, memory_arena *stringMemory) {
    int foundItemID = -1;

    for (int i = 0; i < exploreGame->inventory.numValues; ++i) {
        int itemID = exploreGame->inventory.values[i];
        dungeon_item *item = &exploreGame->allItems.values[itemID];
        for (int j = 0; j < item->alternateNames.numValues; ++j) {
            char *itemName = item->alternateNames.values[j];
            if (stringsAreEqual(action.target, itemName)) {
                foundItemID = itemID;
                goto findItemLoopDone;
            }
        }
    }
findItemLoopDone:

    if (foundItemID != -1) {
        dungeon_item *item = &exploreGame->allItems.values[foundItemID];
        if (item->equippable) {
            exploreGame->equippedItemID = foundItemID;
            char *equipText = appendString("You equip the ", item->name, stringMemory); 
            equipText = appendString(equipText, ".", stringMemory);

            copyDescriptionToBuffer(equipText, exploreGame->descriptionBuffer);
            exploreGame->currentStatusText = exploreGame->descriptionBuffer;
        }
        else {
            exploreGame->currentStatusText = "You can't equip that.";
        }
    }
    else {
        char *equipText = appendString("You don't have a ", action.target, stringMemory); 
        equipText = appendString(equipText, ".", stringMemory);

        copyDescriptionToBuffer(equipText, exploreGame->descriptionBuffer);
        exploreGame->currentStatusText = exploreGame->descriptionBuffer;
    }
}

void tryOpenChest (explore_game *exploreGame, memory_arena *stringMemory) {
    if (exploreGame->currentRoom->hasChest) {

        bool haveKey = false;
        for (int i = 0; i < exploreGame->inventory.numValues; ++i) {
            int itemID = exploreGame->inventory.values[i];
            dungeon_item *item = &exploreGame->allItems.values[itemID];

            // 4 = key
            if (item->id == 4) {
                haveKey = true;
                break;
            }
        }

        if (haveKey) {
            exploreGame->currentStatusText = R"room(Your key slips into the lock and with a satisfying click, the chest 
opens.

Inside the chest is a leatherbound book. You can't understand the 
writing inside, but you figure it's probably valuable, so you take 
it and equip it.)room";
                // 5 = spellbook
            listPush(&exploreGame->inventory, 5);
            exploreGame->equippedItemID = 5;
            exploreGame->currentRoom->hasChest = false;
        }
        else {
            exploreGame->currentStatusText = "The chest is locked.";
        }
    }
    else {
        exploreGame->currentStatusText = "There is nothing to open here.";
    }
}

void tryRead (explore_game *exploreGame, memory_arena *stringMemory) {
    if (exploreGame->currentRoom->textToRead) {
        exploreGame->currentStatusText = exploreGame->currentRoom->textToRead;
    }
}

void doExploreAction (explore_game *exploreGame, explore_action action, memory_arena *stringMemory) {
    switch (action.type) {
        case ACTION_TYPE_LOOK: {
            updateCurrentRoomDescription(exploreGame, stringMemory);
        } break;
        case ACTION_TYPE_MOVE: {
           moveToRoom(exploreGame, action, stringMemory);
           if (exploreGame->currentRoom->numMonsters > 0 && 
               !exploreGame->currentRoom->monstersDefeated) 
           {
               exploreGame->aboutToFight = true;
           }
        } break;
        case ACTION_TYPE_OPEN_CHEST: {
           tryOpenChest(exploreGame, stringMemory);
        } break;
        case ACTION_TYPE_READ: {
           tryRead(exploreGame, stringMemory);
        } break;
        case ACTION_TYPE_TAKE_ITEM: {
           takeItemIfExists(exploreGame, action, stringMemory);
        } break;
        case ACTION_TYPE_CHECK_INVENTORY: {
           printInventory(exploreGame, stringMemory);
        } break;
        case ACTION_TYPE_EQUIP: {
           equipItem(exploreGame, action, stringMemory);
        } break;
        case ACTION_TYPE_BAD_INPUT: {
            exploreGame->currentStatusText = "Sorry, I don't recognize that command.";
        } break;
    }
    exploreGame->state = EXPLORE_STATE_REVEAL_TEXT;
}

bool updateExplorePhase (explore_game *exploreGame, game_input *input, console_drawer *drawer, memory_arena *stringMemory) {
    drawTitleText(exploreGame, drawer);

    bool readyToFight = false;

    switch (exploreGame->state) {
        case EXPLORE_STATE_REVEAL_TEXT: {
            bool textRevealed = revealStatusText(exploreGame, drawer);
            if (input->enterKey.justPressed) {
                textRevealed = true;
            }
            if (textRevealed) {
                exploreGame->state = EXPLORE_STATE_INPUT;
            }
        } break;
        case EXPLORE_STATE_INPUT: {
            drawStatusText(exploreGame, drawer, stringLength(exploreGame->currentStatusText));
            if (exploreGame->gameWon) {
                return false;
            }

            if (exploreGame->aboutToFight) {
                drawFightText(exploreGame, drawer);

                if (input->enterKey.justPressed) {
                    readyToFight = true;
                }
            }
            else if (exploreGame->justLost) {
                bool playerHitEnter = updatePlayerInput(exploreGame, input, drawer);
                if (playerHitEnter) {
                    if (exploreGame->isIntro) {
                        exploreGame->isIntro = false;
                        updateCurrentRoomDescription(exploreGame, stringMemory);
                        exploreGame->state = EXPLORE_STATE_REVEAL_TEXT;
                    }
                    else {
                        bool tryAgain = processPlayerYesNo(exploreGame);
                        exploreGame->justLost = false;
                        if (tryAgain) {
                            updateCurrentRoomDescription(exploreGame, stringMemory);
                            exploreGame->aboutToFight = true;
                        }
                        else {
                            assert(exploreGame->prevRoom);
                            exploreGame->currentRoom = exploreGame->prevRoom;
                            updateCurrentRoomDescription(exploreGame, stringMemory);
                            exploreGame->state = EXPLORE_STATE_REVEAL_TEXT;
                        }
                    }

                    exploreGame->numTypedLetters = 0;
                    exploreGame->revealTime = 0.0f;
                }
            }
            else {
                bool playerHitEnter = updatePlayerInput(exploreGame, input, drawer);

                if (playerHitEnter) {
                    if (exploreGame->isIntro) {
                        exploreGame->isIntro = false;
                        updateCurrentRoomDescription(exploreGame, stringMemory);
                        exploreGame->state = EXPLORE_STATE_REVEAL_TEXT;
                    }
                    else {
                        explore_action action = processPlayerInput(exploreGame);
                        doExploreAction(exploreGame, action, stringMemory);
                    }

                    exploreGame->numTypedLetters = 0;
                    exploreGame->revealTime = 0.0f;
                }
            }
        } break;
    }

    return readyToFight;
}

combat_parameters getCombatParameters (explore_game *exploreGame) {
    combat_parameters result = {};
    result.numMonsters = exploreGame->currentRoom->numMonsters;
    result.monsterType = exploreGame->currentRoom->monsterType;
    result.spawnPositionsID = exploreGame->currentRoom->spawnPositionsID;

    switch (exploreGame->equippedItemID) {
        default: {
            result.weaponType = WEAPON_TYPE_UNARMED;
        } break;
        case 0: {
            result.weaponType = WEAPON_TYPE_DAGGER;
        } break;
        case 1: {
            result.weaponType = WEAPON_TYPE_SWORD;
        } break;
        case 2: {
            result.weaponType = WEAPON_TYPE_BOW;
        } break;
        case 5: {
            result.weaponType = WEAPON_TYPE_SPELLBOOK;
        } break;
    }

    for (int i = 0; i < exploreGame->inventory.numValues; ++i) {
        int itemID = exploreGame->inventory.values[i];
        if (itemID == 3) {
            result.sokobanEnabled = true;
            break;
        }
    }

    return result;
}

void exploreGameOnVictory (explore_game *exploreGame, memory_arena *stringMemory) {
    exploreGame->currentRoom->monstersDefeated = true;
    updateCurrentRoomDescription(exploreGame, stringMemory);
    exploreGame->state = EXPLORE_STATE_INPUT;
    exploreGame->aboutToFight = false;
}

void exploreGameOnDefeat (explore_game *exploreGame) {
    exploreGame->currentStatusText = "Try this fight again?";
    exploreGame->state = EXPLORE_STATE_INPUT;
    exploreGame->aboutToFight = false;
    exploreGame->justLost = true;
}

void exploreGameOnGameWon (explore_game *exploreGame) {
    exploreGame->currentTitleText = "";
    if (exploreGame->inventory.numValues == 6) {
    exploreGame->currentStatusText = R"room(
         __      _______ _____ _______ ____  _______     __
         \ \    / /_   _/ ____|__   __/ __ \|  __ \ \   / /
          \ \  / /  | || |       | | | |  | | |__) \ \_/ / 
           \ \/ /   | || |       | | | |  | |  _  / \   /  
            \  /   _| || |____   | | | |__| | | \ \  | |   
             \/   |_____\_____|  |_|  \____/|_|  \_\ |_|   

      Congratulations! Consider this castle thoroughly looted.)room";
    }
    else {
    exploreGame->currentStatusText = R"room(
         __      _______ _____ _______ ____  _______     __
         \ \    / /_   _/ ____|__   __/ __ \|  __ \ \   / /
          \ \  / /  | || |       | | | |  | | |__) \ \_/ / 
           \ \/ /   | || |       | | | |  | |  _  / \   /  
            \  /   _| || |____   | | | |__| | | \ \  | |   
             \/   |_____\_____|  |_|  \____/|_|  \_\ |_|   

      You made it out alive... But lost treasure still remains.)room";
    }
    exploreGame->state = EXPLORE_STATE_REVEAL_TEXT;
    exploreGame->aboutToFight = false;
    exploreGame->gameWon = true;
}
