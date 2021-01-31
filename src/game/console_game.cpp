#include "console_game.h"

void drawFrame (console_drawer *drawer) {
    for (int i = 0; i < GAME_WIDTH; ++i) {
        drawCharAtXY('-', i, 0, drawer);
    }
    for (int i = 0; i < GAME_WIDTH; ++i) {
        drawCharAtXY('-', i, GAME_HEIGHT - 1, drawer);
    }
    for (int i = 0; i < GAME_HEIGHT; ++i) {
        drawCharAtXY('|', 0, i, drawer);
    }
    for (int i = 0; i < GAME_HEIGHT; ++i) {
        drawCharAtXY('|', GAME_WIDTH - 1, i, drawer);
    }
    drawCharAtXY('+', 0, 0, drawer);
    drawCharAtXY('+', GAME_WIDTH - 1, 0, drawer);
    drawCharAtXY('+', 0, GAME_HEIGHT - 1, drawer);
    drawCharAtXY('+', GAME_WIDTH - 1, GAME_HEIGHT - 1, drawer);
}

void updateConsoleGame (memory_arena *memory, memory_arena *tempMemory, console_game *consoleGame,
                        game_assets *assets, game_input *input, game_sounds *gameSounds,
                        game_window window)
{
    if (!consoleGame->initialized) {
        consoleGame->initialized = true;
        consoleGame->state = CONSOLE_GAME_STATE_EXPLORE;
        consoleGame->windowStartX = 0;
        consoleGame->windowStartY = 0;

        consoleGame->combatMemoryArena = {};
        consoleGame->combatMemoryArena.capacity = 30 * 1024 * 1024;
        consoleGame->combatMemoryArena.base = allocateMemorySize(memory, consoleGame->combatMemoryArena.capacity);

        initExplorePhase(&consoleGame->exploreGame, memory);
    }

    memory_arena stringMemory = {};
    stringMemory.capacity = 64 * 1024;
    stringMemory.base = allocateMemorySize(tempMemory, stringMemory.capacity);

    console_drawer drawer = {};
    drawer.window = window;
    drawer.windowStartX = (window.width - GAME_WIDTH) / 2;
    drawer.windowStartY = (window.height - GAME_HEIGHT) / 2;
    drawer.color = CONSOLE_COLOR_WHITE;

    drawFrame(&drawer);

    switch (consoleGame->state) {
        case CONSOLE_GAME_STATE_EXPLORE: {
            bool readyToFight = updateExplorePhase(&consoleGame->exploreGame, input, &drawer, &stringMemory);
            if (readyToFight) {
                // load map, monsters, etc.
                combat_parameters combatParams = getCombatParameters(&consoleGame->exploreGame);
                consoleGame->combatGame = {};
                consoleGame->combatMemoryArena.size = 0;
                initCombatGame(&consoleGame->combatGame, combatParams, &consoleGame->combatMemoryArena);
                setCombatArena(&consoleGame->combatGame, &drawer);
                consoleGame->state = CONSOLE_GAME_STATE_COMBAT;
            }
        } break;
        case CONSOLE_GAME_STATE_COMBAT: {
            bool combatOver = updateCombatPhase(&consoleGame->combatGame, input, &drawer, &stringMemory, tempMemory);
            if (combatOver) {
                bool gameWon = getGameWon(&consoleGame->combatGame);
                if (gameWon) {
                    exploreGameOnGameWon(&consoleGame->exploreGame);
                }
                else {
                    bool victory = getVictory(&consoleGame->combatGame);
                    if (victory) {
                        exploreGameOnVictory(&consoleGame->exploreGame, &stringMemory);
                    }
                    else {
                        exploreGameOnDefeat(&consoleGame->exploreGame);
                    }
                }
                consoleGame->state = CONSOLE_GAME_STATE_EXPLORE;
            }
        } break;
    }

    //consoleGame->currentColor = CONSOLE_COLOR_WHITE;

    //// test animation, colors, window boundaries
    //static float time = 0.0f;
    //time += DELTA_TIME;

    //float windowHeight = (float)window.height;
    //float halfWindowHeight = (float)window.height / 2.0f;

    //for (int i = 0; i < window.width; ++i) {
    //    if (i < 18) {
    //        consoleGame->currentColor = CONSOLE_COLOR_RED;
    //    }
    //    else if (i < 36) {
    //        consoleGame->currentColor = CONSOLE_COLOR_GREEN;
    //    }
    //    else if (i < 54) {
    //        consoleGame->currentColor = CONSOLE_COLOR_MAGENTA;
    //    }
    //    else if (i < 72) {
    //        consoleGame->currentColor = CONSOLE_COLOR_CYAN;
    //    }
    //    else if (i < 90) {
    //        consoleGame->currentColor = CONSOLE_COLOR_YELLOW;
    //    }
    //    else {
    //        consoleGame->currentColor = CONSOLE_COLOR_BLUE;
    //    }

    //    float t = time + i * 0.016f;
    //    float val = sinf(5.0f * t);
    //    float height = (val * (10.0f)) + halfWindowHeight;
    //    int rounded = (int)height;
    //    if (height - (float)(rounded) >= 0.5f) {
    //        rounded += 1;
    //    }
    //    drawCharAtXY('O', i, rounded, consoleGame, window);
    //}

    //// test mouse input
    //char color = CONSOLE_COLOR_WHITE;
    //if (input->pointerDown) {
    //    color = CONSOLE_COLOR_YELLOW;
    //}
    //consoleGame->currentColor = color;
    //drawCharAtXY('@', input->pointerX, input->pointerY, consoleGame, window);

}
