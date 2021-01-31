#include "combat_game.h"

dungeon_monster buildMonster (monster_type type, memory_arena *memory) {
    dungeon_monster result = {};
    switch (type) {
        case MONSTER_TYPE_GOBLIN: {
            result.name = "Goblin";
            result.combatInfo.health = 30;
        } break;
        case MONSTER_TYPE_GIANT_SNAKE: {
            result.name = "Giant Snake";
            result.combatInfo.health = 100;
            result.segments = dungeon_pointListInit(memory, 40);
            for (int i = 0; i < 40; ++i) {
                dungeon_point segment = {};
                listPush(&result.segments, segment);
            }
        } break;
        case MONSTER_TYPE_DRAGON: {
            result.name = "Dragon";
            result.combatInfo.health = 100;
            result.combatInfo.facing = COMBAT_FACING_LEFT;
            result.attackTimer = 1.9f;
            result.hurtBox = {};
            result.hurtBox.min.x = 0;
            result.hurtBox.min.y = 0;
            result.hurtBox.max.x = 5;
            result.hurtBox.max.y = 2;
        } break;
    }
    return result;
}


// hard coded spawn positions
dungeon_point getStartingPosition (int spawnPositionsID, bool player, int monsterIndex) {
    dungeon_point result = {};
    if (player) {
        switch (spawnPositionsID) {
            case 0: {
                result.x = 35;
                result.y = 15;
            } break;
            case 1: {
                result.x = 35;
                result.y = 15;
            } break;
            case 2: {
                result.x = 40;
                result.y = 13;
            } break;
            case 3: {
                result.x = 18;
                result.y = 15;
            } break;
            case 4: {
                result.x = 18;
                result.y = 15;
            } break;
        }
    }
    else {
        switch (spawnPositionsID) {
            case 0: {
                switch (monsterIndex) {
                    // wrong number of monsters for this room
                    default: {
                        assert(0);
                    } break;
                    case 0: {
                        result.x = 46;
                        result.y = 15;
                    } break;
                }
            } break;
            case 1: {
                switch (monsterIndex) {
                    // wrong number of monsters for this room
                    default: {
                        assert(0);
                    } break;
                    case 0: {
                        result.x = 49;
                        result.y = 20;
                    } break;
                }
            } break;
            case 2: {
                switch (monsterIndex) {
                    // wrong number of monsters for this room
                    default: {
                        assert(0);
                    } break;
                    case 0: {
                        result.x = 17;
                        result.y = 16;
                    } break;
                    case 1: {
                        result.x = 20;
                        result.y = 20;
                    } break;
                    case 2: {
                        result.x = 55;
                        result.y = 12;
                    } break;
                    case 3: {
                        result.x = 69;
                        result.y = 18;
                    } break;
                }
            } break;
            case 3: {
                switch (monsterIndex) {
                    // wrong number of monsters for this room
                    default: {
                        assert(0);
                    } break;
                    case 0: {
                        result.x = 27;
                        result.y = 16;
                    } break;
                    case 1: {
                        result.x = 43;
                        result.y = 19;
                    } break;
                    case 2: {
                        result.x = 65;
                        result.y = 12;
                    } break;
                }
            } break;
            case 4: {
                switch (monsterIndex) {
                    // wrong number of monsters for this room
                    default: {
                        assert(0);
                    } break;
                    case 0: {
                        result.x = 73;
                        result.y = 15;
                    } break;
                }
            } break;
        }
    }
    return result;
}

void initCombatGame(combat_game *combatGame, combat_parameters combatParams, memory_arena *memory) {
    combatGame->memory = memory;
    combatGame->state = COMBAT_GAME_STATE_COMBAT;

    // init player
    combatGame->player = {};
    combatGame->player.combatInfo.position.coords = getStartingPosition(combatParams.spawnPositionsID, true, 0);
    combatGame->player.combatInfo.health = 100;
    combatGame->player.combatInfo.alive = true;
    combatGame->player.combatInfo.facing = COMBAT_FACING_RIGHT;
    combatGame->player.weaponType = combatParams.weaponType;
    combatGame->sokobanEnabled = combatParams.sokobanEnabled;

    // init monsters
    combatGame->monsters = dungeon_monsterListInit(combatGame->memory, 5);
    for (int i = 0; i < combatParams.numMonsters; ++i) {
        dungeon_monster monster = buildMonster(combatParams.monsterType, memory);
        monster.type = combatParams.monsterType;
        monster.combatInfo.position.coords = getStartingPosition(combatParams.spawnPositionsID, false, i);
        monster.combatInfo.alive = true;

        if (monster.type == MONSTER_TYPE_GIANT_SNAKE) {
            for (int j = 0; j < monster.segments.numValues; ++j) {
                dungeon_point *segment = &monster.segments.values[j];
                *segment = monster.combatInfo.position.coords;
            }
        }

        listPush(&combatGame->monsters, monster);
    }

    combatGame->activeAttacks = dungeon_attackListInit(combatGame->memory, 2000);
    combatGame->projectiles = dungeon_projectileListInit(combatGame->memory, 2000);
}

void setArenaChar (combat_game *combatGame, char c, int x, int y) {
    if (x >= 0 && x < COMBAT_ARENA_WIDTH && y >= 0 && y < COMBAT_ARENA_HEIGHT) {
        combatGame->arena[y * COMBAT_ARENA_WIDTH + x] = c;
    }
} 

char getArenaChar (combat_game *combatGame, int x, int y) {
    if (x >= 0 && x < COMBAT_ARENA_WIDTH && y >= 0 && y < COMBAT_ARENA_HEIGHT) {
        return combatGame->arena[y * COMBAT_ARENA_WIDTH + x];
    }
    else {
        return ' ';
    }
}

void setCombatArena (combat_game *combatGame, console_drawer *drawer) {
    for (int i = 0; i < COMBAT_ARENA_HEIGHT; ++i) {
        for (int j = 0; j < COMBAT_ARENA_WIDTH; ++j) {
            combatGame->arena[i * COMBAT_ARENA_WIDTH + j] = ' ';
        }
    }

    int offsetX = (COMBAT_ARENA_WIDTH - GAME_WIDTH) / 2;
    int offsetY = (COMBAT_ARENA_HEIGHT - GAME_HEIGHT) / 2;
    for (int i = 0; i < GAME_HEIGHT; ++i) {
        for (int j = 0; j < GAME_WIDTH; ++j) {
            char c = drawer->window.buffer[(drawer->windowStartY + i) * drawer->window.width + (drawer->windowStartX + j)];
            setArenaChar(combatGame, c, offsetX + j, offsetY + i);
        }
    }
}

void drawArena (combat_game *combatGame, console_drawer *drawer) {
    drawer->color = CONSOLE_COLOR_WHITE;
    int offsetX = (COMBAT_ARENA_WIDTH - GAME_WIDTH) / 2;
    int offsetY = (COMBAT_ARENA_HEIGHT - GAME_HEIGHT) / 2;
    for (int i = 0; i < COMBAT_ARENA_HEIGHT; ++i) {
        for (int j = 0; j < COMBAT_ARENA_WIDTH; ++j) {
            drawCharAtXY(combatGame->arena[i * COMBAT_ARENA_WIDTH + j], j - offsetX, i - offsetY, drawer);
        }
    }
}

void drawHitPoints (combat_game *combatGame, console_drawer *drawer, memory_arena *stringMemory) {
    drawer->color = CONSOLE_COLOR_MAGENTA;
    int offsetX = (COMBAT_ARENA_WIDTH - GAME_WIDTH) / 2;
    int offsetY = (COMBAT_ARENA_HEIGHT - GAME_HEIGHT) / 2;

    char *playerText = "Player HP: ";
    playerText = appendString(playerText, numToString(combatGame->player.combatInfo.health, stringMemory), stringMemory);

    int textLength = stringLength(playerText);
    for (int i = 0; i < textLength; ++i) {
        drawCharAtXY(playerText[i], 1 + i, 23, drawer);
    }

    if (combatGame->lastHitMonster) {
        char *monsterText = "";
        monsterText = appendString(monsterText, combatGame->lastHitMonster->name, stringMemory);
        monsterText = appendString(monsterText, " HP: ", stringMemory);
        monsterText = appendString(monsterText, numToString(combatGame->lastHitMonster->combatInfo.health, stringMemory), stringMemory);

        textLength = stringLength(monsterText);
        for (int i = 0; i < textLength; ++i) {
            drawCharAtXY(monsterText[i], 69 + i - textLength, 23, drawer);
        }
    }
}

void pushBlock (combat_game *combatGame, dungeon_point blockCoords, combat_facing facing) {
    char block = getArenaChar(combatGame, blockCoords.x, blockCoords.y);
    switch (facing) {
        case COMBAT_FACING_UP: {
            char nextBlock = getArenaChar(combatGame, blockCoords.x, blockCoords.y - 1);
            if (nextBlock == ' ') {
            }
            else {
                dungeon_point nextCoords = blockCoords;
                nextCoords.y--;
                pushBlock(combatGame, nextCoords, facing);
            }
            setArenaChar(combatGame, ' ', blockCoords.x, blockCoords.y);
            setArenaChar(combatGame, block, blockCoords.x, blockCoords.y - 1);
        } break;
        case COMBAT_FACING_DOWN:{
            char nextBlock = getArenaChar(combatGame, blockCoords.x, blockCoords.y + 1);
            if (nextBlock == ' ') {

            }
            else {
                dungeon_point nextCoords = blockCoords;
                nextCoords.y++;
                pushBlock(combatGame, nextCoords, facing);
            }
                setArenaChar(combatGame, ' ', blockCoords.x, blockCoords.y);
                setArenaChar(combatGame, block, blockCoords.x, blockCoords.y + 1);
        } break;
        case COMBAT_FACING_LEFT: {
            char nextBlock = getArenaChar(combatGame, blockCoords.x - 1, blockCoords.y);
            if (nextBlock == ' ') {

            }
            else {
                dungeon_point nextCoords = blockCoords;
                nextCoords.x--;
                pushBlock(combatGame, nextCoords, facing);
            }
                setArenaChar(combatGame, ' ', blockCoords.x, blockCoords.y);
                setArenaChar(combatGame, block, blockCoords.x - 1, blockCoords.y);
        } break;
        case COMBAT_FACING_RIGHT: {
            char nextBlock = getArenaChar(combatGame, blockCoords.x + 1, blockCoords.y);
            if (nextBlock == ' ') {

            }
            else {
                dungeon_point nextCoords = blockCoords;
                nextCoords.x++;
                pushBlock(combatGame, nextCoords, facing);
            }
                setArenaChar(combatGame, ' ', blockCoords.x, blockCoords.y);
                setArenaChar(combatGame, block, blockCoords.x + 1, blockCoords.y);
        } break;

    }
}

void updatePlayerPos (combat_game *combatGame, game_input *input) {
    // TODO(ebuchholz): treat X/Y independently, to slide off walls
    dungeon_player *player = &combatGame->player;
    dungeon_position newPos = player->combatInfo.position;

    if (player->combatInfo.hitStunTime > 0.0f) {
        player->combatInfo.hitStunTime -= DELTA_TIME;
        if (player->combatInfo.hitStunTime <= 0.0f) {
            player->combatInfo.hitStunTime = 0.0f;
        }

        newPos.subX += player->combatInfo.knockbackX * KNOCKBACK_SPEED * DELTA_TIME;
        newPos.subY += player->combatInfo.knockbackY * KNOCKBACK_SPEED * DELTA_TIME;
        while (newPos.subY <= -1.0f) {
            newPos.subY += 1.0f;
            newPos.coords.y--;
            // collision check
            char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
            if (arenaChar != ' ') {
                newPos.coords.y++;
            }
        }
        while (newPos.subY >= 1.0f) {
            newPos.subY -= 1.0f;
            newPos.coords.y++;
            char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
            if (arenaChar != ' ') {
                newPos.coords.y--;
            }
        }
        while (newPos.subX >= 1.0f) {
            newPos.subX -= 1.0f;
            newPos.coords.x++;
            char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
            if (arenaChar != ' ') {
                newPos.coords.x--;
            }
        }
        while (newPos.subX <= -1.0f) {
            newPos.subX += 1.0f;
            newPos.coords.x--;
            char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
            if (arenaChar != ' ') {
                newPos.coords.x++;
            }
        }
    }
    else {
        if (player->combatInfo.cooldownRemaining > 0.0f) {
            // can't move yet
        }
        else {
            bool moveUp = false;
            if (input->upKey.justPressed) {
                newPos.coords.y--;
                newPos.subX = 0.0f;
                newPos.subY = 0.0f;
                player->combatInfo.facing = COMBAT_FACING_UP;
                moveUp = true;
            }
            else if (input->upKey.down) {
                newPos.subY -= PLAYER_SPEED * DELTA_TIME;
                while (newPos.subY <= -1.0f) {
                    newPos.subY += 1.0f;
                    newPos.coords.y--;
                    moveUp = true;
                }
                player->combatInfo.facing = COMBAT_FACING_UP;
            }

            if (moveUp) {
                char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
                if (arenaChar != ' ') {
                    if (combatGame->sokobanEnabled) {
                        pushBlock(combatGame, newPos.coords, COMBAT_FACING_UP);
                    }
                    else {
                        newPos.coords.y++;
                    }
                }
            }

            bool moveDown = false;
            if (input->downKey.justPressed) {
                newPos.coords.y++;
                newPos.subX = 0.0f;
                newPos.subY = 0.0f;
                player->combatInfo.facing = COMBAT_FACING_DOWN;
                moveDown= true;
            }
            else if (input->downKey.down) {
                newPos.subY += PLAYER_SPEED * DELTA_TIME;
                while (newPos.subY >= 1.0f) {
                    newPos.subY -= 1.0f;
                    newPos.coords.y++;
                }
                player->combatInfo.facing = COMBAT_FACING_DOWN;
                moveDown= true;
            }

            if (moveDown) {
                char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
                if (arenaChar != ' ') {
                    if (combatGame->sokobanEnabled) {
                        pushBlock(combatGame, newPos.coords, COMBAT_FACING_DOWN);
                    }
                    else {
                        newPos.coords.y--;
                    }
                }
            }

            bool moveLeft = false;
            if (input->leftKey.justPressed) {
                newPos.coords.x--;
                newPos.subX = 0.0f;
                newPos.subY = 0.0f;
                player->combatInfo.facing = COMBAT_FACING_LEFT;
                moveLeft= true;
            }
            else if (input->leftKey.down) {
                newPos.subX -= PLAYER_SPEED * DELTA_TIME;
                while (newPos.subX <= -1.0f) {
                    newPos.subX += 1.0f;
                    newPos.coords.x--;
                }
                player->combatInfo.facing = COMBAT_FACING_LEFT;
                moveLeft= true;
                
            }

            if (moveLeft) {
                char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
                if (arenaChar != ' ') {
                    if (combatGame->sokobanEnabled) {
                        pushBlock(combatGame, newPos.coords, COMBAT_FACING_LEFT);
                    }
                    else {
                        newPos.coords.x++;
                    }
                }
            }

            bool moveRight = false;
            if (input->rightKey.justPressed) {
                newPos.coords.x++;
                newPos.subX = 0.0f;
                newPos.subY = 0.0f;
                player->combatInfo.facing = COMBAT_FACING_RIGHT;
                moveRight= true;
            }
            else if (input->rightKey.down) {
                newPos.subX += PLAYER_SPEED * DELTA_TIME;
                while (newPos.subX >= 1.0f) {
                    newPos.subX -= 1.0f;
                    newPos.coords.x++;
                }
                player->combatInfo.facing = COMBAT_FACING_RIGHT;
                moveRight= true;
            }

            if (moveRight) {
                char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
                if (arenaChar != ' ') {
                    if (combatGame->sokobanEnabled) {
                        pushBlock(combatGame, newPos.coords, COMBAT_FACING_RIGHT);
                    }
                    else {
                        newPos.coords.x--;
                    }
                }
            }

        }
    } 
    player->combatInfo.position = newPos;

}

dungeon_point getFirstPointOnPathToTarget (combat_game *combatGame, dungeon_point start, dungeon_point target, memory_arena *tempMemory) {
    dungeon_point result = {};
    result.x = -1;
    result.y = -1;

    if (start.x == target.x && start.y == target.y) {
        result.x = start.x;
        result.y = start.y;
        return result;
    }

    unsigned int prevSize = tempMemory->size;

    pathfinding_node startNode = {};
    startNode.coords = start;
    float diffX = (float)(target.x - startNode.coords.x);
    float diffY = (float)(target.y - startNode.coords.y);
    startNode.value = sqrtf(diffX * diffX + diffY * diffY);

    // TODO(ebuchholz): linked-list based queue
    pathfinding_node_list nodes = pathfinding_nodeListInit(tempMemory, COMBAT_ARENA_WIDTH * COMBAT_ARENA_HEIGHT);
    listPush(&nodes, startNode);

    dungeon_point dummyCoords = {};
    dummyCoords.x = -1;
    dummyCoords.y = -1;

    dungeon_point_to_dungeon_point_g_hash_map seenPoints = dungeon_pointTodungeon_pointGeneralHashMapInit(tempMemory, 4 * COMBAT_ARENA_WIDTH * COMBAT_ARENA_HEIGHT);
    storeHashMapValue(&seenPoints, dummyCoords, start);

    // TODO(ebuchholz): try inserting nodes after processed ones, to avoid splicing?
    while (nodes.numValues > 0) {
        pathfinding_node node = nodes.values[0];
        listSplice(&nodes, 0);

        if (node.coords.x == target.x && node.coords.y == target.y) {
            dungeon_point previousCoords = getHashMapValue(&seenPoints, node.coords);

            if (previousCoords.x == start.x || previousCoords.y == start.y) {
                result.x = target.x;
                result.y = target.y;
            }
            else {
                // return node after first
                dungeon_point secondLastCoords = previousCoords;
                while (previousCoords.x != start.x || previousCoords.y != start.y) {
                    secondLastCoords = previousCoords;
                    previousCoords = getHashMapValue(&seenPoints, previousCoords);
                }
                result.x = secondLastCoords.x;
                result.y = secondLastCoords.y;
            }

            break;
        }
        else {
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    if (i == 0 && j == 0) {
                        continue;
                    }
                    if (i != 0 && j != 0) {
                        continue;
                    }

                    dungeon_point coords = {};
                    coords.x = node.coords.x + j;
                    coords.y = node.coords.y + i;

                    if (coords.x < 0 || coords.x >= COMBAT_ARENA_WIDTH ||
                        coords.y < 0 || coords.y >= COMBAT_ARENA_HEIGHT)
                    {
                        continue;
                    }

                    char arenaChar = getArenaChar(combatGame, coords.x, coords.y);
                    if (arenaChar == ' ') {
                        bool seen = keyExistsInHashMap(&seenPoints, coords);
                        if (!seen) {
                            pathfinding_node nextNode = {};
                            nextNode.coords = coords;
                            diffX = (float)(target.x - nextNode.coords.x);
                            diffY = (float)(target.y - nextNode.coords.y);
                            nextNode.value = sqrtf(diffX * diffX + diffY * diffY);

                            // TODO(ebuchholz): log-based insert
                            if (nodes.numValues == 0) {
                                listPush(&nodes, nextNode);
                            }
                            else {
                                bool inserted = false;
                                for (int index = 0; index < nodes.numValues; ++index) {
                                    pathfinding_node *existingNode = &nodes.values[index];
                                    if (nextNode.value < existingNode->value) {
                                        listInsert(&nodes, nextNode, index);
                                        inserted = true;
                                        break;
                                    }
                                }

                                if (!inserted) {
                                    listPush(&nodes, nextNode);
                                }
                            }
                            storeHashMapValue(&seenPoints, node.coords, coords);
                        }
                    }
                }
            }
        }
    }

    tempMemory->size = prevSize;
    return result;
}

void updateGoblinPosition (combat_game *combatGame, dungeon_monster *monster, memory_arena *tempMemory) {
    dungeon_position newPos = monster->combatInfo.position;
    dungeon_point playerPos = combatGame->player.combatInfo.position.coords;


    if (monster->combatInfo.hitStunTime > 0.0f) {
        monster->combatInfo.hitStunTime -= DELTA_TIME;
        if (monster->combatInfo.hitStunTime <= 0.0f) {
            monster->combatInfo.hitStunTime = 0.0f;
        }

        newPos.subX += monster->combatInfo.knockbackX * KNOCKBACK_SPEED * DELTA_TIME;
        newPos.subY += monster->combatInfo.knockbackY * KNOCKBACK_SPEED * DELTA_TIME;
        while (newPos.subY <= -1.0f) {
            newPos.subY += 1.0f;
            newPos.coords.y--;

            char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
            if (arenaChar != ' ') {
                newPos.coords.y++;
            }
        }
        while (newPos.subY >= 1.0f) {
            newPos.subY -= 1.0f;
            newPos.coords.y++;
            char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
            if (arenaChar != ' ') {
                newPos.coords.y--;
            }
        }
        while (newPos.subX >= 1.0f) {
            newPos.subX -= 1.0f;
            newPos.coords.x++;
            char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
            if (arenaChar != ' ') {
                newPos.coords.x--;
            }
        }
        while (newPos.subX <= -1.0f) {
            newPos.subX += 1.0f;
            newPos.coords.x--;
            char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
            if (arenaChar != ' ') {
                newPos.coords.x++;
            }
        }
    }
    else {
        if (monster->combatInfo.cooldownRemaining > 0.0f) {
            // can't move yet
        }
        else {
            dungeon_point nextPoint = getFirstPointOnPathToTarget(combatGame, newPos.coords, playerPos, tempMemory);

            if (nextPoint.y < newPos.coords.y) {
                newPos.subY -= GOBLIN_SPEED * DELTA_TIME;
                while (newPos.subY <= -1.0f) {
                    newPos.subY += 1.0f;
                    newPos.coords.y--;
                    char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
                    if (arenaChar != ' ') {
                        newPos.coords.y++;
                    }
                }
            }
            if (nextPoint.y > newPos.coords.y) {
                newPos.subY += GOBLIN_SPEED * DELTA_TIME;
                while (newPos.subY >= 1.0f) {
                    newPos.subY -= 1.0f;
                    newPos.coords.y++;
                    char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
                    if (arenaChar != ' ') {
                        newPos.coords.y--;
                    }
                }
            }
            if (nextPoint.x < newPos.coords.x) {
                newPos.subX -= GOBLIN_SPEED * DELTA_TIME;
                while (newPos.subX <= -1.0f) {
                    newPos.subX += 1.0f;
                    newPos.coords.x--;
                    char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
                    if (arenaChar != ' ') {
                        newPos.coords.x++;
                    }
                }
            }
            if (nextPoint.x > newPos.coords.x) {
                newPos.subX += GOBLIN_SPEED * DELTA_TIME;
                while (newPos.subX >= 1.0f) {
                    newPos.subX -= 1.0f;
                    newPos.coords.x++;
                    char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
                    if (arenaChar != ' ') {
                        newPos.coords.x--;
                    }
                }
            }
        }
    }
    monster->combatInfo.position = newPos;

}

void faceMonsterTowardsPlayer (combat_info *playerInfo, combat_info *monsterInfo) {
    if (playerInfo->position.coords.x == monsterInfo->position.coords.x) {
        if (playerInfo->position.coords.y < monsterInfo->position.coords.y) {
            monsterInfo->facing = COMBAT_FACING_UP;
        }
        else if (playerInfo->position.coords.y > monsterInfo->position.coords.y) {
            monsterInfo->facing = COMBAT_FACING_DOWN;
        }
    }
    else if (playerInfo->position.coords.y == monsterInfo->position.coords.y) {
        if (playerInfo->position.coords.x < monsterInfo->position.coords.x) {
            monsterInfo->facing = COMBAT_FACING_LEFT;
        }
        else if (playerInfo->position.coords.x > monsterInfo->position.coords.x) {
            monsterInfo->facing = COMBAT_FACING_RIGHT;
        }
    }
}

void updateSnakePosition (combat_game *combatGame, dungeon_monster *monster, memory_arena *tempMemory) {
    dungeon_position newPos = monster->combatInfo.position;

    if (monster->combatInfo.hitStunTime > 0.0f) {
        monster->combatInfo.hitStunTime -= DELTA_TIME;
        if (monster->combatInfo.hitStunTime <= 0.0f) {
            monster->combatInfo.hitStunTime = 0.0f;
        }
    }

    monster->timer += DELTA_TIME;
    while (monster->timer >= 2.5f) {
        monster->timer -= 2.5f;
    }
    if (monster->timer > 2.0f) {
        dungeon_player *player = &combatGame->player;
        dungeon_point playerPos = player->combatInfo.position.coords;
        dungeon_point nextPoint = getFirstPointOnPathToTarget(combatGame, newPos.coords, playerPos, tempMemory);

        if (nextPoint.y < newPos.coords.y) {
            newPos.subY -= SNAKE_CHARGE_SPEED * DELTA_TIME;
            while (newPos.subY <= -1.0f) {
                newPos.subY += 1.0f;
                newPos.coords.y--;
                char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
                if (arenaChar != ' ') {
                    newPos.coords.y++;
                }
            }
        }
        if (nextPoint.y > newPos.coords.y) {
            newPos.subY += SNAKE_CHARGE_SPEED * DELTA_TIME;
            while (newPos.subY >= 1.0f) {
                newPos.subY -= 1.0f;
                newPos.coords.y++;
                char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
                if (arenaChar != ' ') {
                    newPos.coords.y--;
                }
            }
        }
        if (nextPoint.x < newPos.coords.x) {
            newPos.subX -= SNAKE_CHARGE_SPEED * DELTA_TIME;
            while (newPos.subX <= -1.0f) {
                newPos.subX += 1.0f;
                newPos.coords.x--;
                char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
                if (arenaChar != ' ') {
                    newPos.coords.x++;
                }
            }
        }
        if (nextPoint.x > newPos.coords.x) {
            newPos.subX += SNAKE_CHARGE_SPEED * DELTA_TIME;
            while (newPos.subX >= 1.0f) {
                newPos.subX -= 1.0f;
                newPos.coords.x++;
                char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
                if (arenaChar != ' ') {
                    newPos.coords.x--;
                }
            }
        }

            dungeon_point lastPos = monster->combatInfo.position.coords;

        if (lastPos.x != newPos.coords.x || lastPos.y != newPos.coords.y) {
            dungeon_point *segment = &monster->segments.values[0];

            segment->x = newPos.coords.x;
            segment->y = newPos.coords.y;

            lastPos = monster->combatInfo.position.coords;

            for (int i = 1; i < monster->segments.numValues; ++i) {
                segment = &monster->segments.values[i];

                dungeon_point tempPos = *segment;

                segment->x = lastPos.x;
                segment->y = lastPos.y;

                lastPos = tempPos;
            }
        }
        monster->combatInfo.position = newPos;
    }
    else {
        switch (monster->combatInfo.facing) {
            case COMBAT_FACING_UP: {
                newPos.subY -= SNAKE_SPEED * DELTA_TIME;
                while (newPos.subY <= -1.0f) {
                    newPos.subY += 1.0f;
                    newPos.coords.y--;
                }
            } break;
            case COMBAT_FACING_DOWN: {
                newPos.subY += SNAKE_SPEED * DELTA_TIME;
                while (newPos.subY >= 1.0f) {
                    newPos.subY -= 1.0f;
                    newPos.coords.y++;
                }
            } break;
            case COMBAT_FACING_LEFT: {
                newPos.subX -= SNAKE_SPEED * DELTA_TIME;
                while (newPos.subX <= -1.0f) {
                    newPos.subX += 1.0f;
                    newPos.coords.x--;
                }
            } break;
            case COMBAT_FACING_RIGHT: {
                newPos.subX += SNAKE_SPEED * DELTA_TIME;
                while (newPos.subX >= 1.0f) {
                    newPos.subX -= 1.0f;
                    newPos.coords.x++;
                }
            } break;
        }

        static int facingIndex = 0;

        char arenaChar = getArenaChar(combatGame, newPos.coords.x, newPos.coords.y);
        if (arenaChar == ' ') {
            dungeon_point lastPos = monster->combatInfo.position.coords;

            if (lastPos.x != newPos.coords.x || lastPos.y != newPos.coords.y) {
                dungeon_point *segment = &monster->segments.values[0];

                segment->x = newPos.coords.x;
                segment->y = newPos.coords.y;

                lastPos = monster->combatInfo.position.coords;

                for (int i = 1; i < monster->segments.numValues; ++i) {
                    segment = &monster->segments.values[i];

                    dungeon_point tempPos = *segment;

                    segment->x = lastPos.x;
                    segment->y = lastPos.y;

                    lastPos = tempPos;
                }
            }

            monster->combatInfo.position = newPos;
        }
        else {
            facingIndex = (facingIndex + 1) % 11;
            if (facingIndex == 1) {
                faceMonsterTowardsPlayer(&combatGame->player.combatInfo, &monster->combatInfo);
            }
            else {
                switch (monster->combatInfo.facing) {
                    case COMBAT_FACING_UP: {
                        if (facingIndex == 0 || facingIndex == 3 || facingIndex == 4 || facingIndex == 6 || facingIndex == 9 || facingIndex == 10) {
                            monster->combatInfo.facing = COMBAT_FACING_LEFT;
                        }
                        else {
                            monster->combatInfo.facing = COMBAT_FACING_RIGHT;
                        }
                    } break;
                    case COMBAT_FACING_DOWN: {
                        if (facingIndex == 0 || facingIndex == 3 || facingIndex == 4 || facingIndex == 6 || facingIndex == 9 || facingIndex == 10) {
                            monster->combatInfo.facing = COMBAT_FACING_LEFT;
                        }
                        else {
                            monster->combatInfo.facing = COMBAT_FACING_RIGHT;
                        }
                    } break;
                    case COMBAT_FACING_LEFT: {
                        if (facingIndex == 0 || facingIndex == 3 || facingIndex == 4 || facingIndex == 6 || facingIndex == 9 || facingIndex == 10) {
                            monster->combatInfo.facing = COMBAT_FACING_UP;
                        }
                        else {
                            monster->combatInfo.facing = COMBAT_FACING_DOWN;
                        }
                    } break;
                    case COMBAT_FACING_RIGHT: {
                        if (facingIndex == 0 || facingIndex == 3 || facingIndex == 4 || facingIndex == 6 || facingIndex == 9 || facingIndex == 10) {
                            monster->combatInfo.facing = COMBAT_FACING_UP;
                        }
                        else {
                            monster->combatInfo.facing = COMBAT_FACING_DOWN;
                        }
                    } break;
                }
            }
        }
    }

}

void updateDragonPosition (combat_game *combatGame, dungeon_monster *monster) {
    dungeon_position newPos = monster->combatInfo.position;

    if (monster->phase2) {
        float flyDuration = 0.5f;
        monster->moveTimer += DELTA_TIME;

        if (monster->moveTimer >= flyDuration) {
            monster->moveTimer = flyDuration;
        }

        float t = monster->moveTimer / flyDuration;

        if (monster->leftSide) {
            float x = 11.0f + t * (73.0f - 11.0f);
            newPos.coords.x = (int)x;
        }
        else {
            float x = 73.0f + t * (11.0f - 73.0f);
            newPos.coords.x = (int)x;
        }

        monster->combatInfo.position = newPos;

        if (monster->moveTimer == flyDuration) {
            monster->phase2 = false;

            if (monster->leftSide) {
                monster->leftSide = false;
                monster->combatInfo.facing = COMBAT_FACING_LEFT;
            }
            else {
                monster->leftSide = true;
                monster->combatInfo.facing = COMBAT_FACING_RIGHT;
            }
            monster->moveTimer = 0.0f;
        }

    }
    else {
        if (monster->combatInfo.hitStunTime > 0.0f) {
            monster->combatInfo.hitStunTime -= DELTA_TIME;
            if (monster->combatInfo.hitStunTime <= 0.0f) {
                monster->combatInfo.hitStunTime = 0.0f;
            }
        }

        monster->timer += DELTA_TIME;
        float yVelocity = cosf(2.0f * monster->timer) * DRAGON_ACCELERATION;

        newPos.subY += yVelocity * DELTA_TIME;
        while (newPos.subY <= -1.0f) {
            newPos.subY += 1.0f;
            newPos.coords.y--;
        }
        while (newPos.subY >= 1.0f) {
            newPos.subY -= 1.0f;
            newPos.coords.y++;
        }

        monster->combatInfo.position = newPos;
    }
}

void updateMonsterPositions (combat_game *combatGame, memory_arena *tempMemory) {
    if (combatGame->freeze) {
        return;
    }
    dungeon_monster_list *monsters = &combatGame->monsters;
    for (int i = 0; i < monsters->numValues; ++i) {
        dungeon_monster *monster = &monsters->values[i];

        if (!monster->combatInfo.alive) {
            continue;
        }

        switch (monster->type) {
            case MONSTER_TYPE_GOBLIN: {
                updateGoblinPosition(combatGame, monster, tempMemory);
            } break;
            case MONSTER_TYPE_GIANT_SNAKE: {
                updateSnakePosition(combatGame, monster, tempMemory);
            } break;
            case MONSTER_TYPE_DRAGON: {
                updateDragonPosition(combatGame, monster);
            } break;
        }
    }
}

void buildUnarmedAttack (combat_info *combatInfo, dungeon_attack *attack) {
    combatInfo->cooldownDuration = 0.2f;
    attack->duration = combatInfo->cooldownDuration;
    attack->damage = 8;

    dungeon_point hitBox = {};
    switch (combatInfo->facing) {
        case COMBAT_FACING_UP: {
            hitBox.x = combatInfo->position.coords.x;
            hitBox.y = combatInfo->position.coords.y - 1;
        } break;
        case COMBAT_FACING_DOWN: {
            hitBox.x = combatInfo->position.coords.x;
            hitBox.y = combatInfo->position.coords.y + 1;
        } break;
        case COMBAT_FACING_LEFT: {
            hitBox.x = combatInfo->position.coords.x - 1;
            hitBox.y = combatInfo->position.coords.y;
        } break;
        case COMBAT_FACING_RIGHT: {
            hitBox.x = combatInfo->position.coords.x + 1;
            hitBox.y = combatInfo->position.coords.y;
        } break;
    }

    attack->hitBoxes[attack->numHitBoxes++] = hitBox;
}

void buildDaggerAttack (combat_game *combatGame, combat_info *combatInfo, dungeon_attack *attack) {
    combatInfo->cooldownDuration = 0.1f;
    attack->duration = 0.1f;
    attack->damage = 13;

    dungeon_point hitBox = {};
    switch (combatInfo->facing) {
        case COMBAT_FACING_UP: {
            hitBox.x = combatInfo->position.coords.x;
            hitBox.y = combatInfo->position.coords.y - 1;
        } break;
        case COMBAT_FACING_DOWN: {
            hitBox.x = combatInfo->position.coords.x;
            hitBox.y = combatInfo->position.coords.y + 1;
        } break;
        case COMBAT_FACING_LEFT: {
            hitBox.x = combatInfo->position.coords.x - 1;
            hitBox.y = combatInfo->position.coords.y;
        } break;
        case COMBAT_FACING_RIGHT: {
            hitBox.x = combatInfo->position.coords.x + 1;
            hitBox.y = combatInfo->position.coords.y;
        } break;
    }

    attack->hitBoxes[attack->numHitBoxes++] = hitBox;

    dungeon_point firstHitBox = hitBox;
    hitBox = {};
    switch (combatInfo->facing) {
        case COMBAT_FACING_UP: {
            hitBox.x = combatInfo->position.coords.x;
            hitBox.y = combatInfo->position.coords.y - 2;
        } break;
        case COMBAT_FACING_DOWN: {
            hitBox.x = combatInfo->position.coords.x;
            hitBox.y = combatInfo->position.coords.y + 2;
        } break;
        case COMBAT_FACING_LEFT: {
            hitBox.x = combatInfo->position.coords.x - 2;
            hitBox.y = combatInfo->position.coords.y;
        } break;
        case COMBAT_FACING_RIGHT: {
            hitBox.x = combatInfo->position.coords.x + 2;
            hitBox.y = combatInfo->position.coords.y;
        } break;
    }
    char arenaChar = getArenaChar(combatGame, firstHitBox.x, firstHitBox.y);
    if (arenaChar == ' ') {
        attack->hitBoxes[attack->numHitBoxes++] = hitBox;
    }
}

void buildSwordAttack (combat_game *combatGame, combat_info *combatInfo, dungeon_attack *attack) {
    combatInfo->cooldownDuration = 0.15f;
    attack->duration = 0.1f;
    attack->damage = 22;

    switch (combatInfo->facing) {
        case COMBAT_FACING_UP: {
            int width = 3;
            int start = combatInfo->position.coords.x - 1;
            bool continueAttack = true; // stop if hit wall
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < width; ++j) {
                    dungeon_point hitBox = {};
                    hitBox.x = start + j;
                    hitBox.y = combatInfo->position.coords.y - i - 1;

                    attack->hitBoxes[attack->numHitBoxes++] = hitBox;

                    char arenaChar = getArenaChar(combatGame, hitBox.x, hitBox.y);
                    if (arenaChar != ' ') {
                        continueAttack = false;
                    }
                }
                start -= 1;
                width += 2;

                // TODO(ebuchholz): have walls block attacks in a better way, for now just let the player
                // attack through walls
                //if (!continueAttack) {
                //    break;
                //}
            }
        } break;
        case COMBAT_FACING_DOWN: {
            int width = 3;
            int start = combatInfo->position.coords.x - 1;
            bool continueAttack = true; // stop if hit wall
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < width; ++j) {
                    dungeon_point hitBox = {};
                    hitBox.x = start + j;
                    hitBox.y = combatInfo->position.coords.y + i + 1;

                    attack->hitBoxes[attack->numHitBoxes++] = hitBox;

                    char arenaChar = getArenaChar(combatGame, hitBox.x, hitBox.y);
                    if (arenaChar != ' ') {
                        continueAttack = false;
                    }
                }
                start -= 1;
                width += 2;

                // TODO(ebuchholz): have walls block attacks in a better way, for now just let the player
                // attack through walls
                //if (!continueAttack) {
                //    break;
                //}
            }
        } break;
        case COMBAT_FACING_LEFT: {
            int width = 3;
            int start = combatInfo->position.coords.y - 1;
            bool continueAttack = true; // stop if hit wall
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < width; ++j) {
                    dungeon_point hitBox = {};
                    hitBox.x = combatInfo->position.coords.x - i - 1;
                    hitBox.y = start + j;

                    attack->hitBoxes[attack->numHitBoxes++] = hitBox;

                    char arenaChar = getArenaChar(combatGame, hitBox.x, hitBox.y);
                    if (arenaChar != ' ') {
                        continueAttack = false;
                    }
                }
                start -= 1;
                width += 2;

                // TODO(ebuchholz): have walls block attacks in a better way, for now just let the player
                // attack through walls
                //if (!continueAttack) {
                //    break;
                //}
            }
        } break;
        case COMBAT_FACING_RIGHT: {
            int width = 3;
            int start = combatInfo->position.coords.y - 1;
            bool continueAttack = true; // stop if hit wall
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < width; ++j) {
                    dungeon_point hitBox = {};
                    hitBox.x = combatInfo->position.coords.x + i + 1;
                    hitBox.y = start + j;

                    attack->hitBoxes[attack->numHitBoxes++] = hitBox;

                    char arenaChar = getArenaChar(combatGame, hitBox.x, hitBox.y);
                    if (arenaChar != ' ') {
                        continueAttack = false;
                    }
                }
                start -= 1;
                width += 2;

                //if (!continueAttack) {
                //    break;
                //}
            }
        } break;
    }

}
void spawnProjectile (combat_game *combatGame, dungeon_position pos, vector2 velocity, char character, char color, dungeon_attack attack) {
    dungeon_projectile projectile = {};
    projectile.pos = pos;
    projectile.velocity = velocity;
    projectile.character = character;
    projectile.color = color;

    attack.ownerPos = pos.coords;

    dungeon_point hitBox = {};
    hitBox.x = pos.coords.x;
    hitBox.y = pos.coords.y;

    attack.hitBoxes[attack.numHitBoxes++] = hitBox;

    projectile.attack = attack;

    listPush(&combatGame->projectiles, projectile);
}

void spawnFireball (combat_game *combatGame) {
    dungeon_projectile projectile = {};
    projectile.pos = combatGame->player.combatInfo.position;
    projectile.velocity = Vector2();
    projectile.character = 'O';
    projectile.color = CONSOLE_COLOR_RED;

    combatGame->soundToPlay = "fireball";

    dungeon_attack attack = {};
    attack.t = 0.0f;
    attack.damage = 25;
    attack.duration = 5.0f;
    attack.ownerPos = projectile.pos.coords;
    attack.ownerIsPlayer = true;

    dungeon_point hitBox = {};
    hitBox.x = projectile.pos.coords.x;
    hitBox.y = projectile.pos.coords.y;
    attack.hitBoxes[attack.numHitBoxes++] = hitBox;

    projectile.attack = attack;
    projectile.pathfinding = true;

    float minDistance = 10000.0f;
    int minDistIndex = 0;
    dungeon_monster_list *monsters = &combatGame->monsters;
    for (int monsterIndex = 0; monsterIndex < monsters->numValues; ++monsterIndex) {
        dungeon_monster *monster = &monsters->values[monsterIndex];
        if (!monster->combatInfo.alive) {
            continue;
        }

        float diffX = (float)(monster->combatInfo.position.coords.x - projectile.pos.coords.x);
        float diffY = (float)(monster->combatInfo.position.coords.y - projectile.pos.coords.y);
        float distance = sqrtf(diffX * diffX + diffY * diffY);
        if (distance < minDistance) {
            minDistance = distance;
            minDistIndex = monsterIndex;
        }
    }

    projectile.target = &monsters->values[minDistIndex];

    listPush(&combatGame->projectiles, projectile);
}

void spawnArrow (combat_game *combatGame) {
    dungeon_player *player = &combatGame->player;
    player->combatInfo.cooldownDuration = 0.2f;

    dungeon_attack attack = {};
    attack.t = 0.0f;
    attack.damage = 12;
    attack.duration = 1.0f;
    attack.ownerIsPlayer = true;

    dungeon_position pos = {};

    vector2 velocity = Vector2();
    char c = ' ';

    switch (player->combatInfo.facing) {
        case COMBAT_FACING_UP: {
            pos.coords.x = player->combatInfo.position.coords.x;
            pos.coords.y = player->combatInfo.position.coords.y -1;
            velocity.x = 0.0f;
            velocity.y = -1.0f;
            c = '|';
        } break;
        case COMBAT_FACING_DOWN: {
            pos.coords.x = player->combatInfo.position.coords.x;
            pos.coords.y = player->combatInfo.position.coords.y +1;
            velocity.x = 0.0f;
            velocity.y = 1.0f;
            c = '|';
        } break;
        case COMBAT_FACING_LEFT: {
            pos.coords.x = player->combatInfo.position.coords.x-1;
            pos.coords.y = player->combatInfo.position.coords.y;
            velocity.x = -1.0f;
            velocity.y = 0.0f;
            c = '-';
        } break;
        case COMBAT_FACING_RIGHT: {
            pos.coords.x = player->combatInfo.position.coords.x+1;
            pos.coords.y = player->combatInfo.position.coords.y;
            velocity.x = 1.0f;
            velocity.y = 0.0f;
            c = '-';
        } break;

    } 

    spawnProjectile(combatGame, pos, velocity * FIRE_SPEED, c, CONSOLE_COLOR_CYAN, attack);

}

bool updateSpellbookInput (combat_game *combatGame, game_input *input) {
    // determine where to draw the input
    if (input->numTypedChars > 0) {
        int typedLetterIndex = 0;
        while (combatGame->numTypedLetters < MAX_TEXT_INPUT_LENGTH && typedLetterIndex < input->numTypedChars) {
            combatGame->currentText[combatGame->numTypedLetters++] = input->typedText[typedLetterIndex];
            ++typedLetterIndex;
        }
    }
    if (input->backspaceDown) {
        if (combatGame->numTypedLetters > 0) {
            combatGame->numTypedLetters -= 1;
        }
    }

    return input->enterKey.justPressed;
}

void updatePlayerAttack (combat_game *combatGame, game_input *input) {
    dungeon_player *player = &combatGame->player;
    if (player->weaponType == WEAPON_TYPE_SPELLBOOK) {
        bool enterPressed = updateSpellbookInput(combatGame, input);
        if (enterPressed) {
            char inputWord[MAX_TEXT_INPUT_LENGTH];
            char *cursor = combatGame->currentText;
            cursor = readInputWord(cursor, inputWord, combatGame->numTypedLetters);

            if (stringsAreEqual(inputWord, "FIREBALL")) {
                spawnFireball(combatGame);
            }
            else if (stringsAreEqual(inputWord, "FREEZE")) {
                combatGame->freeze = true;
                combatGame->freezeTimer = 0.0f;
                combatGame->soundToPlay = "freeze";
            }
            else if (stringsAreEqual(inputWord, "TELEPORT")) {
                combatGame->soundToPlay = "teleport";
                if (player->combatInfo.position.coords.x % 4 == 0) {
                    player->combatInfo.position.coords.y = 4;
                }
                else if (player->combatInfo.position.coords.x % 4 == 1) {
                    player->combatInfo.position.coords.y = 30;
                }
                else if (player->combatInfo.position.coords.x % 4 == 2) {
                    player->combatInfo.position.coords.x = 8;
                }
                else if (player->combatInfo.position.coords.x % 4 == 3) {
                    player->combatInfo.position.coords.x = 84;
                }
            }

            combatGame->numTypedLetters = 0;
        }
    }
    else {
        player->combatInfo.cooldownRemaining -= DELTA_TIME;
        if (player->combatInfo.cooldownRemaining <= 0.0f) {
            player->combatInfo.cooldownRemaining = 0.0f;
        }
        if (player->combatInfo.cooldownRemaining == 0.0f && player->combatInfo.hitStunTime == 0.0f) {
            if (input->actionKey.justPressed) {
                dungeon_attack attack = {};
                attack.t = 0.0f;
                attack.ownerIsPlayer = true;
                attack.ownerPos = player->combatInfo.position.coords;

                combatGame->soundToPlay = "swing";

                // TODO(ebuchholz): reconsider attack durations
                switch (player->weaponType) {
                    case WEAPON_TYPE_UNARMED: {
                        buildUnarmedAttack(&player->combatInfo, &attack);
                    } break;
                    case WEAPON_TYPE_DAGGER: {
                        buildDaggerAttack(combatGame, &player->combatInfo, &attack);
                    } break;
                    case WEAPON_TYPE_SWORD: {
                        buildSwordAttack(combatGame, &player->combatInfo, &attack);
                    } break;
                    case WEAPON_TYPE_BOW: {
                        spawnArrow(combatGame);
                    } break;
                    case WEAPON_TYPE_SPELLBOOK: {
                    } break;
                }

                player->combatInfo.cooldownRemaining = player->combatInfo.cooldownDuration;

                listPush(&combatGame->activeAttacks, attack);
            }
        }
    }
}


void updateMonsterAttacks (combat_game *combatGame) {
    dungeon_player *player = &combatGame->player;
    dungeon_monster_list *monsters = &combatGame->monsters;

    for (int monsterIndex = 0; monsterIndex < monsters->numValues; ++monsterIndex) {
        dungeon_monster *monster = &monsters->values[monsterIndex];

        if (!monster->combatInfo.alive) {
            continue;
        }

        if (combatGame->freeze) {
            continue;
        }

        monster->combatInfo.cooldownRemaining -= DELTA_TIME;
        if (monster->combatInfo.cooldownRemaining <= 0.0f) {
            monster->combatInfo.cooldownRemaining = 0.0f;
        }
        if (monster->combatInfo.cooldownRemaining == 0.0f && monster->combatInfo.hitStunTime == 0.0f || monster->type == MONSTER_TYPE_DRAGON) {
            switch (monster->type) {
                case MONSTER_TYPE_GOBLIN: {
                    // hurt if touched
                    dungeon_point hitCell = {};
                    hitCell.x = monster->combatInfo.position.coords.x;
                    hitCell.y = monster->combatInfo.position.coords.y;

                    if (hitCell.x == player->combatInfo.position.coords.x && 
                        hitCell.y == player->combatInfo.position.coords.y)
                    {
                        dungeon_attack attack = {};
                        attack.t = 0.0f;
                        attack.damage = 8;
                        attack.duration = 0.0001f;
                        attack.ownerIsPlayer = false;
                        attack.ownerPos = monster->combatInfo.position.coords;

                        if (player->combatInfo.position.subX - monster->combatInfo.position.subX < 0) {
                            attack.ownerPos.x++;
                        }
                        else {
                            attack.ownerPos.x--;
                        }
                        if (player->combatInfo.position.subY - monster->combatInfo.position.subY < 0) {
                            attack.ownerPos.y++;
                        }
                        else {
                            attack.ownerPos.y--;
                        }

                        attack.hitBoxes[attack.numHitBoxes++] = hitCell;

                        listPush(&combatGame->activeAttacks, attack);
                    }

                    //short range attack
                    dungeon_point monsterPos = monster->combatInfo.position.coords;
                    dungeon_point playerPos = player->combatInfo.position.coords;
                    if ((monsterPos.y == playerPos.y && absoluteValue(monsterPos.x - playerPos.x) <= 1) || 
                        (monsterPos.x == playerPos.x && absoluteValue(monsterPos.y - playerPos.y) <= 1)) 
                    {
                        dungeon_attack attack = {};
                        attack.t = 0.0f;
                        attack.ownerIsPlayer = false;
                        attack.ownerPos = monster->combatInfo.position.coords;

                        faceMonsterTowardsPlayer(&player->combatInfo, &monster->combatInfo);
                        buildUnarmedAttack(&monster->combatInfo, &attack);

                        // hack it to be weaker
                        attack.damage = 8;
                        monster->combatInfo.cooldownRemaining = 0.5f;

                        listPush(&combatGame->activeAttacks, attack);
                    }
                } break;
                case MONSTER_TYPE_GIANT_SNAKE: {
                    for (int i = 0; i < monster->segments.numValues; ++i) {
                        dungeon_point *segment = &monster->segments.values[i];

                        // spawn an attack on the player if he touches a segment
                        if (segment->x == player->combatInfo.position.coords.x && 
                            segment->y == player->combatInfo.position.coords.y)
                        {
                            dungeon_attack attack = {};
                            attack.t = 0.0f;
                            attack.damage = 20;
                            attack.duration = 0.0001f;
                            attack.ownerIsPlayer = false;
                            attack.ownerPos = *segment;

                            dungeon_point hitBox = {};
                            hitBox.x = segment->x;
                            hitBox.y = segment->y;

                            attack.hitBoxes[attack.numHitBoxes++] = hitBox;

                            if (i == 0) {
                                attack.ownerPos = monster->segments.values[1];
                            }
                            else if (i == monster->segments.numValues - 1) {
                                attack.ownerPos = monster->segments.values[monster->segments.numValues - 2];
                            }
                            else {
                                static int knockbackSwap = 0;
                                knockbackSwap = (knockbackSwap + 1) % 2;

                                dungeon_point *nextSegment = &monster->segments.values[i+1];
                                if (segment->x == nextSegment->x) {
                                    if (knockbackSwap == 0) {
                                        attack.ownerPos.x += 1;
                                    }
                                    else {
                                        attack.ownerPos.x -= 1;
                                    }
                                }
                                else if (segment->y == nextSegment->y) {
                                    if (knockbackSwap == 0) {
                                        attack.ownerPos.y += 1;
                                    }
                                    else {
                                        attack.ownerPos.y -= 1;
                                    }
                                }
                            }

                            monster->combatInfo.cooldownRemaining = 0.25f;

                            listPush(&combatGame->activeAttacks, attack);

                            break;
                        }

                    }
                } break;
                case MONSTER_TYPE_DRAGON: {
                    // spawn fire
                    if (!monster->phase2) {
                        monster->moveTimer += DELTA_TIME;
                        while (monster->moveTimer >= 4.0f) {
                            monster->moveTimer -= 4.0f;
                            monster->phase2 = true;
                        }

                        monster->attackTimer += DELTA_TIME;
                        while (monster->attackTimer >= 2.0f) {
                            monster->attackTimer -= 2.0f;
                            combatGame->soundToPlay = "fire_breath";
                        }

                        if (monster->attackTimer < 1.5f) {
                            if (monster->combatInfo.facing == COMBAT_FACING_LEFT) {
                                // TODO(ebuchholz): different directions
                                for (int y = 0; y < 3; ++y) {
                                    dungeon_attack attack = {};
                                    attack.t = 0.0f;
                                    attack.damage = 5;
                                    attack.duration = 5.0f;
                                    attack.ownerIsPlayer = false;

                                    dungeon_position pos = {};
                                    pos.coords.x = monster->combatInfo.position.coords.x - 1;
                                    pos.coords.y = monster->combatInfo.position.coords.y + y;

                                    spawnProjectile(combatGame, pos, Vector2(-1.0f, 0.0f) * FIRE_SPEED, 'Z', CONSOLE_COLOR_RED, attack);
                                }
                            }
                            else {
                                for (int y = 0; y < 3; ++y) {
                                    dungeon_attack attack = {};
                                    attack.t = 0.0f;
                                    attack.damage = 5;
                                    attack.duration = 5.0f;
                                    attack.ownerIsPlayer = false;

                                    dungeon_position pos = {};
                                    pos.coords.x = monster->combatInfo.position.coords.x + (int)monster->hurtBox.max.x + 1;
                                    pos.coords.y = monster->combatInfo.position.coords.y + y;

                                    spawnProjectile(combatGame, pos, Vector2(1.0f, 0.0f) * FIRE_SPEED, 'Z', CONSOLE_COLOR_RED, attack);
                                }
                            }
                        }
                    }

                    // also hurt on touch
                    bool hurtOnTouch = false;
                    for (int x = 0; x <= (int)monster->hurtBox.max.x; ++x) {
                        for (int y = 0; y <= (int)monster->hurtBox.max.y; ++y) {
                            dungeon_point hitCell = {};
                            hitCell.x = monster->combatInfo.position.coords.x + x;
                            hitCell.y = monster->combatInfo.position.coords.y + y;

                            if (hitCell.x == player->combatInfo.position.coords.x && 
                                hitCell.y == player->combatInfo.position.coords.y)
                            {
                                dungeon_attack attack = {};
                                attack.t = 0.0f;
                                attack.damage = 20;
                                attack.duration = 0.0001f;
                                attack.ownerIsPlayer = false;
                                attack.ownerPos = monster->combatInfo.position.coords;

                                aabb hurtBox = monster->hurtBox;
                                attack.ownerPos.x += (int)((hurtBox.max.x - hurtBox.min.x)/2.0f);
                                attack.ownerPos.y += (int)((hurtBox.max.y - hurtBox.min.y)/2.0f);
                                //monster->combatInfo.cooldownRemaining = 0.25f;

                                attack.hitBoxes[attack.numHitBoxes++] = hitCell;

                                listPush(&combatGame->activeAttacks, attack);
                                hurtOnTouch = true;
                                break;
                            }
                        }
                        if (hurtOnTouch) {
                            break;
                        }
                    }
                } break;
            }
        }
    }
}

bool attackOverlapsPoint (dungeon_attack *attack, dungeon_point point) {
    for (int hitBoxIndex = 0; hitBoxIndex < attack->numHitBoxes; ++hitBoxIndex) {
        dungeon_point hitBox = attack->hitBoxes[hitBoxIndex];
        if (hitBox.x == point.x && hitBox.y == point.y) {
            return true;
        } 
    }
    return false;
}

bool testAttack (combat_game *combatGame, dungeon_attack *attack) {
    dungeon_player *player = &combatGame->player;

    if (!attack->hit) {
        if (attack->ownerIsPlayer) {
            dungeon_monster_list *monsters = &combatGame->monsters;
            for (int monsterIndex = 0; monsterIndex < monsters->numValues; ++monsterIndex) {
                dungeon_monster *monster = &monsters->values[monsterIndex];

                if (!monster->combatInfo.alive) {
                    continue;
                }

                if (monster->combatInfo.hitStunTime == 0.0f) {
                    bool hitMonster = false;
                    switch (monster->type) {
                        case MONSTER_TYPE_GOBLIN: {
                            hitMonster = attackOverlapsPoint(attack, monster->combatInfo.position.coords);
                        } break;
                        case MONSTER_TYPE_GIANT_SNAKE: {
                            for (int segmentIndex = 0; segmentIndex < monster->segments.numValues; ++segmentIndex) {
                                dungeon_point *segment = &monster->segments.values[segmentIndex];
                                if (attackOverlapsPoint(attack, *segment)) {
                                    hitMonster = true;
                                    break;
                                }
                            }
                        } break;
                        case MONSTER_TYPE_DRAGON: {
                            for (int x = 0; x <= (int)monster->hurtBox.max.x; ++x) {
                                for (int y = 0; y <= (int)monster->hurtBox.max.y; ++y) {
                                    dungeon_point hurtCell = {};
                                    hurtCell.x = monster->combatInfo.position.coords.x + x;
                                    hurtCell.y = monster->combatInfo.position.coords.y + y;
                                    if (attackOverlapsPoint(attack, hurtCell)) {
                                        hitMonster = true;
                                        goto monsterHitCheckDone;
                                    }
                                }
                            }
                        } break;
                    }
monsterHitCheckDone:
                    if (hitMonster) {
                        attack->hit = true;
                        combatGame->lastHitMonster = monster;
                        combatGame->soundToPlay = "hit";

                        monster->combatInfo.health -= attack->damage;
                        if (monster->combatInfo.health <= 0) {
                            monster->combatInfo.alive = false;
                        }
                        else {
                            monster->combatInfo.hitStunTime = 0.1f;
                            if (monster->type == MONSTER_TYPE_GOBLIN) {
                                vector2 knockbackDir = Vector2((float)monster->combatInfo.position.coords.x, (float)monster->combatInfo.position.coords.y) -  
                                                       Vector2((float)player->combatInfo.position.coords.x, (float)player->combatInfo.position.coords.y);
                                knockbackDir = normalize(knockbackDir);
                                monster->combatInfo.knockbackX = knockbackDir.x;
                                monster->combatInfo.knockbackY = knockbackDir.y;
                            }
                        }
                    }
                }
            }

        }
        else {
            if (player->combatInfo.hitStunTime == 0.0f) {
                bool hitPlayer = attackOverlapsPoint(attack, player->combatInfo.position.coords);
                if (hitPlayer) {
                    attack->hit = true;
                    player->combatInfo.health -= attack->damage;
                    combatGame->soundToPlay = "hurt";
                    if (player->combatInfo.health <= 0) {
                        player->combatInfo.alive = false;
                    }
                    else {
                        player->combatInfo.hitStunTime = 0.1f;
                        vector2 knockbackDir = Vector2((float)player->combatInfo.position.coords.x, (float)player->combatInfo.position.coords.y) -  
                                               Vector2((float)attack->ownerPos.x, (float)attack->ownerPos.y);
                        knockbackDir = normalize(knockbackDir);
                        player->combatInfo.knockbackX = knockbackDir.x;
                        player->combatInfo.knockbackY = knockbackDir.y;
                    }
                }
            }
        }
    }

    attack->t += DELTA_TIME;
    if (attack->t >= attack->duration) {
        return true;
    }
    return false;
}

void updateProjectiles (combat_game *combatGame, memory_arena *tempMemory) {
    dungeon_projectile_list *projectiles = &combatGame->projectiles;

    for (int i = projectiles->numValues - 1; i >= 0; --i) {
        dungeon_projectile *projectile = &projectiles->values[i];

        if (projectile->pathfinding) {
            dungeon_point target = projectile->target->combatInfo.position.coords;
            dungeon_point nextPoint = getFirstPointOnPathToTarget(combatGame, projectile->pos.coords, target, tempMemory);

            dungeon_point lastCoords = projectile->pos.coords;

            if (nextPoint.y < projectile->pos.coords.y) {
                projectile->pos.subY -= FIRE_SPEED * DELTA_TIME;
                while (projectile->pos.subY <= -1.0f) {
                    projectile->pos.subY += 1.0f;
                    projectile->pos.coords.y--;
                    char arenaChar = getArenaChar(combatGame, projectile->pos.coords.x, projectile->pos.coords.y);
                    if (arenaChar != ' ') {
                        projectile->pos.coords.y++;
                    }
                }
            }
            if (nextPoint.y > projectile->pos.coords.y) {
                projectile->pos.subY += FIRE_SPEED * DELTA_TIME;
                while (projectile->pos.subY >= 1.0f) {
                    projectile->pos.subY -= 1.0f;
                    projectile->pos.coords.y++;
                    char arenaChar = getArenaChar(combatGame, projectile->pos.coords.x, projectile->pos.coords.y);
                    if (arenaChar != ' ') {
                        projectile->pos.coords.y--;
                    }
                }
            }
            if (nextPoint.x < projectile->pos.coords.x) {
                projectile->pos.subX -= FIRE_SPEED * DELTA_TIME;
                while (projectile->pos.subX <= -1.0f) {
                    projectile->pos.subX += 1.0f;
                    projectile->pos.coords.x--;
                    char arenaChar = getArenaChar(combatGame, projectile->pos.coords.x, projectile->pos.coords.y);
                    if (arenaChar != ' ') {
                        projectile->pos.coords.x++;
                    }
                }
            }
            if (nextPoint.x > projectile->pos.coords.x) {
                projectile->pos.subX += FIRE_SPEED * DELTA_TIME;
                while (projectile->pos.subX >= 1.0f) {
                    projectile->pos.subX -= 1.0f;
                    projectile->pos.coords.x++;
                    char arenaChar = getArenaChar(combatGame, projectile->pos.coords.x, projectile->pos.coords.y);
                    if (arenaChar != ' ') {
                        projectile->pos.coords.x--;
                    }
                }
            }

            if (projectile->pos.coords.x != lastCoords.x || projectile->pos.coords.y != lastCoords.y) {
                projectile->attack.ownerPos = lastCoords;
            }

            projectile->attack.hitBoxes[0].x = projectile->pos.coords.x;
            projectile->attack.hitBoxes[0].y = projectile->pos.coords.y;

            bool attackRemoved = testAttack(combatGame, &projectile->attack);
            if (attackRemoved || projectile->attack.hit) {
                listSplice(&combatGame->projectiles, i);
            }
        }
        else {
            projectile->pos.subX += projectile->velocity.x * DELTA_TIME;
            projectile->pos.subY += projectile->velocity.y * DELTA_TIME;

            dungeon_point lastCoords = projectile->pos.coords;

            while (projectile->pos.subY <= -1.0f) {
                projectile->pos.subY += 1.0f;
                projectile->pos.coords.y--;
            }
            while (projectile->pos.subY >= 1.0f) {
                projectile->pos.subY -= 1.0f;
                projectile->pos.coords.y++;
            }
            while (projectile->pos.subX >= 1.0f) {
                projectile->pos.subX -= 1.0f;
                projectile->pos.coords.x++;
            }
            while (projectile->pos.subX <= -1.0f) {
                projectile->pos.subX += 1.0f;
                projectile->pos.coords.x--;
            }

            if (projectile->pos.coords.x != lastCoords.x || projectile->pos.coords.y != lastCoords.y) {
                projectile->attack.ownerPos = lastCoords;
            }

            projectile->attack.hitBoxes[0].x = projectile->pos.coords.x;
            projectile->attack.hitBoxes[0].y = projectile->pos.coords.y;

            char arenaChar = getArenaChar(combatGame, projectile->pos.coords.x, projectile->pos.coords.y);
            if (arenaChar != ' ') {
                listSplice(&combatGame->projectiles, i);
            }
            else {
                bool attackRemoved = testAttack(combatGame, &projectile->attack);
                if (attackRemoved || projectile->attack.hit) {
                    listSplice(&combatGame->projectiles, i);
                }
            }
        }
    }

}

// TODO(ebuchholz): refactor to general combat info logic, instead of separate for player/monster?
void updateActiveAttacks (combat_game *combatGame) {

    for (int i = combatGame->activeAttacks.numValues - 1; i >= 0; --i) {
        dungeon_attack *attack = &combatGame->activeAttacks.values[i];
        // check for hits
        bool attackRemoved = testAttack(combatGame, attack);
        if (attackRemoved) {
            listSplice(&combatGame->activeAttacks, i);
        }
    }
}

void drawPlayer (combat_game *combatGame, console_drawer *drawer) {

    static float hitStunTimer = 0.0f;
    static float blink = false;
    hitStunTimer += DELTA_TIME;
    while (hitStunTimer > 0.05f) {
        hitStunTimer -= 0.05f;
        blink = !blink;
    }

    drawer->color = CONSOLE_COLOR_GREEN;
    int offsetX = (COMBAT_ARENA_WIDTH - GAME_WIDTH) / 2;
    int offsetY = (COMBAT_ARENA_HEIGHT - GAME_HEIGHT) / 2;
    if (combatGame->player.combatInfo.alive) {
        if (blink && combatGame->player.combatInfo.hitStunTime > 0.0f) {
            drawer->color = CONSOLE_COLOR_RED;
        }
        drawCharAtXY('@', combatGame->player.combatInfo.position.coords.x - offsetX, combatGame->player.combatInfo.position.coords.y - offsetY, drawer);
    }
    else {
        drawCharAtXY('X', combatGame->player.combatInfo.position.coords.x - offsetX, combatGame->player.combatInfo.position.coords.y - offsetY, drawer);
    }
}

void drawMonsters (combat_game *combatGame, console_drawer *drawer) {
    drawer->color = CONSOLE_COLOR_YELLOW;
    if (combatGame->freeze) {
        drawer->color = CONSOLE_COLOR_CYAN;
    }
    int offsetX = (COMBAT_ARENA_WIDTH - GAME_WIDTH) / 2;
    int offsetY = (COMBAT_ARENA_HEIGHT - GAME_HEIGHT) / 2;

    static float hitStunTimer = 0.0f;
    static float blink = false;
    hitStunTimer += DELTA_TIME;
    while (hitStunTimer > 0.05f) {
        hitStunTimer -= 0.05f;
        blink = !blink;
    }

    dungeon_monster_list *monsters = &combatGame->monsters;
    for (int i = 0; i < monsters->numValues; ++i) {
        dungeon_monster *monster = &monsters->values[i];

        if (blink && monster->combatInfo.hitStunTime > 0.0f) {
            drawer->color = CONSOLE_COLOR_RED;
        }

        switch (monster->type) {
            case MONSTER_TYPE_GOBLIN: {
                if (monster->combatInfo.alive) {
                    drawCharAtXY('G', monster->combatInfo.position.coords.x - offsetX, monster->combatInfo.position.coords.y - offsetY, drawer);
                }
                else {
                    drawCharAtXY('X', monster->combatInfo.position.coords.x - offsetX, monster->combatInfo.position.coords.y - offsetY, drawer);
                }
            } break;
            case MONSTER_TYPE_GIANT_SNAKE: {
                if (monster->combatInfo.alive) {
                    for (int j = monster->segments.numValues - 1; j >= 0; --j) {
                        dungeon_point segment = monster->segments.values[j];
                        char c = j == 0 ? 'O' : 'o';
                        drawCharAtXY(c, segment.x - offsetX, segment.y - offsetY, drawer);
                    }
                }
                else {
                    for (int j = 0; j < monster->segments.numValues; ++j) {
                        dungeon_point segment = monster->segments.values[j];
                        drawCharAtXY('X', segment.x - offsetX, segment.y - offsetY, drawer);
                    }
                }
            } break;
            case MONSTER_TYPE_DRAGON: {
                char *top = "_N/FF ";
                char *mid = "7' \\/\\";
                char *bot = " NML/'";
                if (monster->combatInfo.alive) {
                    dungeon_point pos = monster->combatInfo.position.coords;
                    for (int j = 0; j < stringLength(top); ++j) {
                        drawCharAtXY(top[j], pos.x - offsetX + j, pos.y - offsetY, drawer);
                    }
                    for (int j = 0; j < stringLength(mid); ++j) {
                        drawCharAtXY(mid[j], pos.x - offsetX + j, pos.y - offsetY + 1, drawer);
                    }
                    for (int j = 0; j < stringLength(bot); ++j) {
                        drawCharAtXY(bot[j], pos.x - offsetX + j, pos.y - offsetY + 2, drawer);
                    }
                }
                else {
                    dungeon_point pos = monster->combatInfo.position.coords;
                    for (int j = 0; j < stringLength(top); ++j) {
                        char c = top[j] == ' ' ? ' ' : 'X';
                        drawCharAtXY(c, pos.x - offsetX + j, pos.y - offsetY, drawer);
                    }
                    for (int j = 0; j < stringLength(mid); ++j) {
                        char c = mid[j] == ' ' ? ' ' : 'X';
                        drawCharAtXY(c, pos.x - offsetX + j, pos.y - offsetY + 1, drawer);
                    }
                    for (int j = 0; j < stringLength(bot); ++j) {
                        char c = bot[j] == ' ' ? ' ' : 'X';
                        drawCharAtXY(c, pos.x - offsetX + j, pos.y - offsetY + 2, drawer);
                    }
                }
            } break;                      
        }                                 
    }
}

void drawProjectiles (combat_game *combatGame, console_drawer *drawer) {
    int offsetX = (COMBAT_ARENA_WIDTH - GAME_WIDTH) / 2;
    int offsetY = (COMBAT_ARENA_HEIGHT - GAME_HEIGHT) / 2;

    for (int i = combatGame->projectiles.numValues - 1; i >= 0; --i) {
        dungeon_projectile *projectile = &combatGame->projectiles.values[i];

        drawer->color = projectile->color;
        dungeon_point hitBox = projectile->pos.coords;
        drawCharAtXY(projectile->character, hitBox.x - offsetX, hitBox.y - offsetY, drawer);
    }
}

void drawActiveAttacks (combat_game *combatGame, console_drawer *drawer) {
    drawer->color = CONSOLE_COLOR_RED;
    int offsetX = (COMBAT_ARENA_WIDTH - GAME_WIDTH) / 2;
    int offsetY = (COMBAT_ARENA_HEIGHT - GAME_HEIGHT) / 2;

    for (int i = combatGame->activeAttacks.numValues - 1; i >= 0; --i) {
        dungeon_attack *attack = &combatGame->activeAttacks.values[i];

        for (int j = 0; j < attack->numHitBoxes; ++j) {
            dungeon_point hitBox = attack->hitBoxes[j];
            drawCharAtXY('X', hitBox.x - offsetX, hitBox.y - offsetY, drawer);
        }

    }
}

bool allMonstersDead (combat_game *combatGame) {
    bool monsterIsAlive = false;
    dungeon_monster_list *monsters = &combatGame->monsters;
    for (int i = 0; i < monsters->numValues; ++i) {
        dungeon_monster *monster = &monsters->values[i];
        if (monster->combatInfo.alive) {
            monsterIsAlive = true;
            break;
        }
    }

    return !monsterIsAlive;
}

bool getVictory (combat_game *combatGame) {
    return combatGame->player.combatInfo.alive;
}

bool getGameWon (combat_game *combatGame) {
    return combatGame->gameWon;
}

void drawGameEnd (combat_game *combatGame, console_drawer *drawer) {
    char *endMessage;
    if (getVictory(combatGame)) {
        endMessage = "VICTORY";
        drawer->color = CONSOLE_COLOR_GREEN;
    }
    else {
        endMessage = "GAME OVER";
        drawer->color = CONSOLE_COLOR_RED;
    }

    combatGame->endMessageTimer += DELTA_TIME;
    float revealPercent = combatGame->endMessageTimer / 0.5f;
    int numLettersRevealed;
    if (revealPercent >= 1.0f) {
        numLettersRevealed = stringLength(endMessage);
    }
    else {
        numLettersRevealed = (int)(revealPercent * stringLength(endMessage));
    }

    char *cursor = endMessage;
    int currentLetterIndex = 0;
    int currentX = 35 - numLettersRevealed;
    int currentY = 10;
    while (*cursor != 0 && currentLetterIndex < numLettersRevealed) {
        drawCharAtXY(*cursor, currentX, currentY, drawer);
        currentX++;
        ++cursor;
        ++currentLetterIndex;
    }
}

void drawPlayerAttack (combat_game *combatGame, console_drawer *drawer) {
    drawer->color = CONSOLE_COLOR_CYAN;
    int offsetX = (COMBAT_ARENA_WIDTH - GAME_WIDTH) / 2;
    int offsetY = (COMBAT_ARENA_HEIGHT - GAME_HEIGHT) / 2;

    dungeon_player *player = &combatGame->player;
    dungeon_point playerPos = player->combatInfo.position.coords;
    switch (player->weaponType) {
        case WEAPON_TYPE_UNARMED: {
            if (player->combatInfo.cooldownRemaining) {
                switch (player->combatInfo.facing) {
                    case COMBAT_FACING_UP: {
                        drawCharAtXY('O', playerPos.x - offsetX, playerPos.y - 1 - offsetY, drawer);
                    } break;
                    case COMBAT_FACING_DOWN: {
                        drawCharAtXY('O', playerPos.x - offsetX, playerPos.y + 1 - offsetY, drawer);
                    } break;
                    case COMBAT_FACING_LEFT: {
                        drawCharAtXY('O', playerPos.x - 1 - offsetX, playerPos.y - offsetY, drawer);
                    } break;
                    case COMBAT_FACING_RIGHT: {
                        drawCharAtXY('O', playerPos.x + 1 - offsetX, playerPos.y - offsetY, drawer);
                    } break;
                }
            }
        } break;
        case WEAPON_TYPE_DAGGER: {
            if (player->combatInfo.cooldownRemaining) {
                switch (player->combatInfo.facing) {
                    case COMBAT_FACING_UP: {
                        drawCharAtXY('|', playerPos.x - offsetX, playerPos.y - 1 - offsetY, drawer);
                        drawCharAtXY('|', playerPos.x - offsetX, playerPos.y - 2 - offsetY, drawer);
                    } break;
                    case COMBAT_FACING_DOWN: {
                        drawCharAtXY('|', playerPos.x - offsetX, playerPos.y + 1 - offsetY, drawer);
                        drawCharAtXY('|', playerPos.x - offsetX, playerPos.y + 2 - offsetY, drawer);
                    } break;
                    case COMBAT_FACING_LEFT: {
                        drawCharAtXY('-', playerPos.x - 1 - offsetX, playerPos.y - offsetY, drawer);
                        drawCharAtXY('-', playerPos.x - 2 - offsetX, playerPos.y - offsetY, drawer);
                    } break;
                    case COMBAT_FACING_RIGHT: {
                        drawCharAtXY('-', playerPos.x + 1 - offsetX, playerPos.y - offsetY, drawer);
                        drawCharAtXY('-', playerPos.x + 2 - offsetX, playerPos.y - offsetY, drawer);
                    } break;
                }
            }
        } break;
        case WEAPON_TYPE_SWORD: {
            float swingTime = player->combatInfo.cooldownRemaining - 0.05f;
            if (swingTime > 0.0f) {
                if (swingTime > 0.067f) {
                    switch (player->combatInfo.facing) {
                        case COMBAT_FACING_UP: {
                            drawCharAtXY('\\', playerPos.x - 1 - offsetX, playerPos.y - 1 - offsetY, drawer);
                            drawCharAtXY('\\', playerPos.x - 2 - offsetX, playerPos.y - 2 - offsetY, drawer);
                            drawCharAtXY('\\', playerPos.x - 3 - offsetX, playerPos.y - 3 - offsetY, drawer);
                        } break;
                        case COMBAT_FACING_DOWN: {
                            drawCharAtXY('\\', playerPos.x + 1 - offsetX, playerPos.y + 1 - offsetY, drawer);
                            drawCharAtXY('\\', playerPos.x + 2 - offsetX, playerPos.y + 2 - offsetY, drawer);
                            drawCharAtXY('\\', playerPos.x + 3 - offsetX, playerPos.y + 3 - offsetY, drawer);
                        } break;
                        case COMBAT_FACING_LEFT: {
                            drawCharAtXY('/', playerPos.x - 1 - offsetX, playerPos.y + 1 - offsetY, drawer);
                            drawCharAtXY('/', playerPos.x - 2 - offsetX, playerPos.y + 2 - offsetY, drawer);
                            drawCharAtXY('/', playerPos.x - 3 - offsetX, playerPos.y + 3 - offsetY, drawer);
                        } break;
                        case COMBAT_FACING_RIGHT: {
                            drawCharAtXY('/', playerPos.x + 1 - offsetX, playerPos.y - 1 - offsetY, drawer);
                            drawCharAtXY('/', playerPos.x + 2 - offsetX, playerPos.y - 2 - offsetY, drawer);
                            drawCharAtXY('/', playerPos.x + 3 - offsetX, playerPos.y - 3 - offsetY, drawer);
                        } break;
                    }
                }
                else if (swingTime > 0.033f) {
                    switch (player->combatInfo.facing) {
                        case COMBAT_FACING_UP: {
                            drawCharAtXY('|', playerPos.x - offsetX, playerPos.y - 1 - offsetY, drawer);
                            drawCharAtXY('|', playerPos.x - offsetX, playerPos.y - 2 - offsetY, drawer);
                            drawCharAtXY('|', playerPos.x - offsetX, playerPos.y - 3 - offsetY, drawer);
                        } break;
                        case COMBAT_FACING_DOWN: {
                            drawCharAtXY('|', playerPos.x - offsetX, playerPos.y + 1 - offsetY, drawer);
                            drawCharAtXY('|', playerPos.x - offsetX, playerPos.y + 2 - offsetY, drawer);
                            drawCharAtXY('|', playerPos.x - offsetX, playerPos.y + 3 - offsetY, drawer);
                        } break;
                        case COMBAT_FACING_LEFT: {
                            drawCharAtXY('-', playerPos.x - 1 - offsetX, playerPos.y - offsetY, drawer);
                            drawCharAtXY('-', playerPos.x - 2 - offsetX, playerPos.y - offsetY, drawer);
                            drawCharAtXY('-', playerPos.x - 3 - offsetX, playerPos.y - offsetY, drawer);
                        } break;
                        case COMBAT_FACING_RIGHT: {
                            drawCharAtXY('-', playerPos.x + 1 - offsetX, playerPos.y - offsetY, drawer);
                            drawCharAtXY('-', playerPos.x + 2 - offsetX, playerPos.y - offsetY, drawer);
                            drawCharAtXY('-', playerPos.x + 3 - offsetX, playerPos.y - offsetY, drawer);
                        } break;
                    }
                }
                else {
                    switch (player->combatInfo.facing) {
                        case COMBAT_FACING_UP: {
                            drawCharAtXY('/', playerPos.x + 1 - offsetX, playerPos.y - 1 - offsetY, drawer);
                            drawCharAtXY('/', playerPos.x + 2 - offsetX, playerPos.y - 2 - offsetY, drawer);
                            drawCharAtXY('/', playerPos.x + 3 - offsetX, playerPos.y - 3 - offsetY, drawer);
                        } break;
                        case COMBAT_FACING_DOWN: {
                            drawCharAtXY('/', playerPos.x - 1 - offsetX, playerPos.y + 1 - offsetY, drawer);
                            drawCharAtXY('/', playerPos.x - 2 - offsetX, playerPos.y + 2 - offsetY, drawer);
                            drawCharAtXY('/', playerPos.x - 3 - offsetX, playerPos.y + 3 - offsetY, drawer);
                        } break;
                        case COMBAT_FACING_LEFT: {
                            drawCharAtXY('\\', playerPos.x - 1 - offsetX, playerPos.y - 1 - offsetY, drawer);
                            drawCharAtXY('\\', playerPos.x - 2 - offsetX, playerPos.y - 2 - offsetY, drawer);
                            drawCharAtXY('\\', playerPos.x - 3 - offsetX, playerPos.y - 3 - offsetY, drawer);
                        } break;
                        case COMBAT_FACING_RIGHT: {
                            drawCharAtXY('\\', playerPos.x + 1 - offsetX, playerPos.y + 1 - offsetY, drawer);
                            drawCharAtXY('\\', playerPos.x + 2 - offsetX, playerPos.y + 2 - offsetY, drawer);
                            drawCharAtXY('\\', playerPos.x + 3 - offsetX, playerPos.y + 3 - offsetY, drawer);
                        } break;
                    }
                }
            }
        } break;
    }
}
void drawSpellbookText (combat_game *combatGame, console_drawer *drawer) {
    drawCharAtXY('>', 16, 23, drawer);
    for (int i = 0; i < combatGame->numTypedLetters; ++i) {
        drawCharAtXY(combatGame->currentText[i], i + 17, 23, drawer);
    }

    static bool blinkOn = false;
    static float blinkTimer = 0.0f;

    blinkTimer += DELTA_TIME;
    while (blinkTimer >= 0.15f) {
        blinkTimer -= 0.15f;
        blinkOn = !blinkOn;
    }

    if (blinkOn) {
        drawCharAtXY('_', combatGame->numTypedLetters + 17, 23, drawer);
    }
}


bool updateCombatPhase (combat_game *combatGame, game_input *input, console_drawer *drawer, memory_arena *stringMemory, memory_arena *tempMemory, game_sounds *gameSounds, game_assets *assets) {
    bool endPhase = false;
    combatGame->soundToPlay = 0;
    // update combat
    switch (combatGame->state) {
        case COMBAT_GAME_STATE_COMBAT: {
            updatePlayerPos(combatGame, input);
            updateMonsterPositions(combatGame, tempMemory);

            updatePlayerAttack(combatGame, input);

            if (combatGame->freeze) {
                combatGame->freezeTimer += DELTA_TIME;
                while (combatGame->freezeTimer >= 2.0f) {
                    combatGame->freezeTimer -= 2.0f;
                    combatGame->freeze = false;
                }
            }

            //update monster attacks
            updateMonsterAttacks(combatGame);
            
            updateProjectiles(combatGame, tempMemory);
            updateActiveAttacks(combatGame);

            bool combatOver = false;
            if (allMonstersDead(combatGame)) {
                combatOver = true;
            }
            if (!combatGame->player.combatInfo.alive) {
                combatOver = true;
            }

            dungeon_point playerPos = combatGame->player.combatInfo.position.coords;
            if (playerPos.x < 0 || playerPos.x >= COMBAT_ARENA_WIDTH || 
                playerPos.y < 0 || playerPos.y >= COMBAT_ARENA_HEIGHT) 
            {
                combatGame->gameWon = true;
                endPhase = true;
            }

            if (combatOver) {
                combatGame->state = COMBAT_GAME_STATE_END;
            }
        } break;
        case COMBAT_GAME_STATE_END: {
            if (combatGame->endMessageTimer > 2.0f) {
                endPhase = true;
            }
        } break;
    }

    // draw combat
    drawArena(combatGame, drawer);
    
    drawHitPoints(combatGame, drawer, stringMemory);

    drawMonsters(combatGame, drawer);
    drawPlayer(combatGame, drawer);

    if (combatGame->player.weaponType == WEAPON_TYPE_SPELLBOOK) {
        drawSpellbookText(combatGame, drawer);
    }

    // debug
    //drawActiveAttacks(combatGame, drawer);
    drawProjectiles(combatGame, drawer);
    drawPlayerAttack(combatGame, drawer);

    // end game logic
    switch (combatGame->state) {
        case COMBAT_GAME_STATE_COMBAT: {
        } break;
        case COMBAT_GAME_STATE_END: {
            drawGameEnd(combatGame, drawer);
        } break;
    }

    if (combatGame->soundToPlay != 0) {
        playSound(combatGame->soundToPlay, gameSounds, assets);
    }
    return endPhase;
}
