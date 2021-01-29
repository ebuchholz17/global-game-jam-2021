#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <xinput.h>

#include "win_platform.h"
#include "../game/quickmake_game.cpp"

#include "win_console.cpp"

#include "directsound_audio.cpp"

static bool programRunning = false;
static float targetMSPerFrame = 1000.0f / 60.0f;

// xinput functions
// NOTE(ebuchholz): avoiding redefinition of functions in xinput.h
typedef DWORD WINAPI x_input_get_state(DWORD dwUserIndex, XINPUT_STATE *pState);
static x_input_get_state *XInputGetState_;
#define XInputGetState XInputGetState_
typedef DWORD WINAPI x_input_set_state(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration);
static x_input_set_state *XInputSetState_;
#define XInputSetState XInputSetState_

static void updateControllerButton (XINPUT_GAMEPAD gamepad, input_key *button, WORD buttonBitmask) {
    button->justPressed = false;
    if (gamepad.wButtons & buttonBitmask) {
        if (!button->down) {
            button->justPressed = true;
        }
        button->down = true;
    }
    else {
        button->down = false;
    }
}

static void updateControllerTrigger (float *trigger, input_key *triggerButton, BYTE triggerValue, BYTE triggerMax) {
    triggerButton->justPressed = false;
    if (triggerValue >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
        triggerValue -= XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
        *trigger = (float)((float)triggerValue / (float)triggerMax);
        if (!triggerButton->down) {
            triggerButton->justPressed = true;
        }
        triggerButton->down = true;
    }
    else {
        *trigger = 0.0f;
        triggerButton->down = false;
    }

}

static void updateControllerStick (SHORT gamepadStickX, SHORT gamepadStickY, float *stickX, float *stickY, SHORT deadzone) {
    float floatStickX = (float)gamepadStickX;
    float floatStickY = -(float)gamepadStickY;
    float magnitude = sqrtf(floatStickX * floatStickX + floatStickY * floatStickY);
    float normalizedSX = floatStickX / magnitude;
    float normalizedSY = floatStickY / magnitude;

    if (magnitude > (float)deadzone) {
        if (magnitude > 32767.0f) {
            magnitude = 32767.0f;
        }
        magnitude -= deadzone;
        float normalizedMagnitude = magnitude / (32767.0f - deadzone);
        *stickX = normalizedSX * normalizedMagnitude;
        *stickY = normalizedSY * normalizedMagnitude;
    }
    else {
        *stickX = 0.0f;
        *stickY = 0.0f;
    }
}

static void updateControllerStickDirection (input_key *dir0, input_key *dir1, SHORT gamepadVal, SHORT deadzone) {
    dir0->justPressed = false;
    if (gamepadVal < -deadzone) {
        if (!dir0->down) {
            dir0->justPressed = true;
        }
        dir0->down = true;
    }
    else {
        dir0->down = false;
    }
    dir1->justPressed = false;
    if (gamepadVal > deadzone) {
        if (!dir1->down) {
            dir1->justPressed = true;
        }
        dir1->down = true;
    }
    else {
        dir1->down = false;
    }
}

static void updateController (game_controller_input *controller, XINPUT_STATE state) {
    XINPUT_GAMEPAD gamepad = state.Gamepad;
    updateControllerButton(gamepad, &controller->dPadUp, XINPUT_GAMEPAD_DPAD_UP);
    updateControllerButton(gamepad, &controller->dPadDown, XINPUT_GAMEPAD_DPAD_DOWN);
    updateControllerButton(gamepad, &controller->dPadLeft, XINPUT_GAMEPAD_DPAD_LEFT);
    updateControllerButton(gamepad, &controller->dPadRight, XINPUT_GAMEPAD_DPAD_RIGHT);
    updateControllerButton(gamepad, &controller->start, XINPUT_GAMEPAD_START);
    updateControllerButton(gamepad, &controller->back, XINPUT_GAMEPAD_BACK);
    updateControllerButton(gamepad, &controller->leftStick, XINPUT_GAMEPAD_LEFT_THUMB);
    updateControllerButton(gamepad, &controller->rightStick, XINPUT_GAMEPAD_RIGHT_THUMB);
    updateControllerButton(gamepad, &controller->leftBumper, XINPUT_GAMEPAD_LEFT_SHOULDER);
    updateControllerButton(gamepad, &controller->rightBumper, XINPUT_GAMEPAD_RIGHT_SHOULDER);
    updateControllerButton(gamepad, &controller->aButton, XINPUT_GAMEPAD_A);
    updateControllerButton(gamepad, &controller->bButton, XINPUT_GAMEPAD_B);
    updateControllerButton(gamepad, &controller->xButton, XINPUT_GAMEPAD_X);
    updateControllerButton(gamepad, &controller->yButton, XINPUT_GAMEPAD_Y);

    BYTE triggerMax = 255 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
    updateControllerTrigger(&controller->leftTrigger, &controller->leftTriggerButton, gamepad.bLeftTrigger, triggerMax);
    updateControllerTrigger(&controller->rightTrigger, &controller->rightTriggerButton, gamepad.bRightTrigger, triggerMax);

    updateControllerStick(gamepad.sThumbLX, gamepad.sThumbLY, &controller->leftStickX, &controller->leftStickY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    updateControllerStick(gamepad.sThumbRX, gamepad.sThumbRY, &controller->rightStickX, &controller->rightStickY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

    updateControllerStickDirection(&controller->leftStickUp, &controller->leftStickDown, -gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    updateControllerStickDirection(&controller->leftStickLeft, &controller->leftStickRight, gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    updateControllerStickDirection(&controller->rightStickUp, &controller->rightStickDown, -gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
    updateControllerStickDirection(&controller->rightStickLeft, &controller->rightStickRight, gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
}

static void loadXInput () {
    HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
    if(!XInputLibrary) {
        XInputLibrary = LoadLibraryA("xinput9_1_0.dll");
    }
    if(!XInputLibrary) {
        XInputLibrary = LoadLibraryA("xinput1_3.dll");
    }

    // TODO(ebuchholz): replace with error-handling that doesn't crash program
    if(XInputLibrary) {
        XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
        if(!XInputGetState) { 
            assert(0);
        }
        XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
        if(!XInputSetState) { 
            assert(0);
        }
    }
    else {
        assert(0); // couldn't load xinput
    }
}

static void clearWinConsoleWindow (win_console_window winConsoleWindow) {
    for (int i = 0; i < winConsoleWindow.height; ++i) {
        for (int j = 0; j < winConsoleWindow.width; ++j) {
            winConsoleWindow.buffer[i * winConsoleWindow.width + j] = ' ';
            winConsoleWindow.colors[i * winConsoleWindow.width + j] = CONSOLE_COLOR_WHITE;
        }
    }
}

static void resizeWinConsoleBuffer (win_console *winConsole, win_console_window winConsoleWindow) {
    int requiredWindowBufferSize = initWinConsoleBuffer(winConsole, winConsoleWindow.width, winConsoleWindow.height);
    void *windowInternalBuffer = malloc(requiredWindowBufferSize);
    setWindowBufferMemory(winConsole, windowInternalBuffer);
}

int WINAPI WinMain (HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode) {
    LARGE_INTEGER perfCountFrequencyResult;
    QueryPerformanceFrequency(&perfCountFrequencyResult);
    long long perfCountFrequency = perfCountFrequencyResult.QuadPart;


    // Game memory (game state, etc.)
    game_memory gameMemory = {};
    gameMemory.memoryCapacity = 10 * 1024 * 1024; // 10MB arbitrarily decided
    gameMemory.memory = malloc(gameMemory.memoryCapacity);
    gameMemory.tempMemoryCapacity = 5 * 1024 * 1024; // another 5MB arbitrarily decided
    gameMemory.tempMemory = malloc(gameMemory.tempMemoryCapacity);

    for (unsigned int i = 0; i < gameMemory.memoryCapacity; ++i) {
        *((char *)gameMemory.memory + i) = 0;
    }

    // init console window
    AllocConsole();
    HWND windowHandle = GetConsoleWindow();

    win_console winConsole;
    initWinConsole(&winConsole, targetMSPerFrame);
    setWinConsoleOutputActive(&winConsole);

    win_console_window winConsoleWindow = {};
    winConsoleWindow.width = 100;
    winConsoleWindow.height = 40;
    winConsoleWindow.buffer = (char *)malloc(winConsoleWindow.width * winConsoleWindow.height);
    winConsoleWindow.colors = (char *)malloc(winConsoleWindow.width * winConsoleWindow.height);

    clearWinConsoleWindow(winConsoleWindow);
    resizeWinConsoleBuffer(&winConsole, winConsoleWindow);

    // init sound
    win_sound_output soundOutput = {};
    soundOutput.samplesPerSecond = 44100;
    soundOutput.bytesPerSample = 2;
    soundOutput.secondaryBufferSize = soundOutput.samplesPerSecond * soundOutput.bytesPerSample;
    initDirectSound(windowHandle, &soundOutput);
    clearSecondaryBuffer(&soundOutput);
    soundOutput.secondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
    soundOutput.soundValid = false;

    sound_sample *soundSamples = (sound_sample *)malloc(soundOutput.samplesPerSecond * 4);
    assert(soundSamples);

    // init input
    game_input input = {};
    win_console_input consoleInput = {};
    loadXInput();
    bool shouldCheckForNewControllers = true;

    // set up windows-specific options that the game needs to know about
    platform_options options = {};
    options.audioSampleRate = soundOutput.samplesPerSecond;

    // Load assets from asset pack
    FILE *assetsFile; 
    fopen_s(&assetsFile, "assets.qpk", "rb");
    assert(assetsFile);

    fseek(assetsFile, 0, SEEK_END);
    int assetPackFileSize = ftell(assetsFile);
    fseek(assetsFile, 0, SEEK_SET);

    char *assetPackFileData = (char *)malloc(assetPackFileSize);
    fread(assetPackFileData, assetPackFileSize, 1, assetsFile);
    fclose(assetsFile);

    memory_arena workingAssetMemory = {};
    workingAssetMemory.capacity = 10 * 1024 * 1024; // 10MB limit for working with asset files?
    workingAssetMemory.base = malloc(workingAssetMemory.capacity);

    asset_pack_data assetPackData = {};
    assetPackData.assetData = assetPackFileData;
    do {
        workingAssetMemory.size = 0;
        loadNextAssetFile(&assetPackData, &gameMemory, &workingAssetMemory, &options);
        if (assetPackData.needPlatformLoad) {
            // NOTE(ebuchholz): none of these currently used for console game engine
            switch (assetPackData.lastAssetType){
                default: {
                    assert(0);
                } break;
                case ASSET_TYPE_OBJ: {
                    //loadRendererMesh(&rendererMemory, (loaded_mesh_asset *)workingAssetMemory.base);
                } break;
                case ASSET_TYPE_QMM: {
                    //loadRendererAnimatedMesh(&rendererMemory, (loaded_animated_mesh_asset *)workingAssetMemory.base);
                } break;
                case ASSET_TYPE_BMP: {
                    //loadRendererTexture(&rendererMemory, (loaded_texture_asset *)workingAssetMemory.base);
                } break;
                case ASSET_TYPE_ATLAS_TEXTURE: {
                    //loadRendererTexture(&rendererMemory, (loaded_texture_asset *)workingAssetMemory.base);
                } break;
            }
        }

    } while (!assetPackData.complete);
    free(assetPackFileData);
    free(workingAssetMemory.base);

    // start the frame
    LARGE_INTEGER lastCounter;
    QueryPerformanceCounter(&lastCounter);

    LARGE_INTEGER lastControllerCheckTime = lastCounter;
    startWinConsoleFrame(&winConsole);

    programRunning = true;
    while (programRunning) {
        // reset input
        input.pointerJustDown = false;
        input.upKey.justPressed = false;
        input.downKey.justPressed = false;
        input.leftKey.justPressed = false;
        input.rightKey.justPressed = false;
        input.enterKey.justPressed = false;
        input.backspaceDown = false;
        input.actionKey.justPressed = false;
        input.pointerJustDown = false;

        winConsoleResetInput(&consoleInput);

        // resize console window and reallocate buffer if changed
        int winConsoleWidth;
        int winConsoleHeight;
        winConsoleGetWindowDimensions(&winConsole, &winConsoleWidth, &winConsoleHeight);

        if (winConsoleWidth != winConsoleWindow.width || winConsoleHeight != winConsoleWindow.height) {
            winConsoleWindow.width = winConsoleWidth;
            winConsoleWindow.height = winConsoleHeight;
            free(winConsoleWindow.buffer);
            winConsoleWindow.buffer = (char *)malloc(winConsoleWindow.width * winConsoleWindow.height);
            free(winConsoleWindow.colors);
            winConsoleWindow.colors = (char *)malloc(winConsoleWindow.width * winConsoleWindow.height);

            clearWinConsoleWindow(winConsoleWindow);

            free(winConsole.windowBuffer);
            resizeWinConsoleBuffer(&winConsole, winConsoleWindow);
        }

        // poll controllers
        LARGE_INTEGER controllerCheckTime;
        QueryPerformanceCounter(&controllerCheckTime);

        long long controllerTimeElapsed = controllerCheckTime.QuadPart - lastControllerCheckTime.QuadPart;
        float elapsedControllerSeconds = (float)controllerTimeElapsed / (float)perfCountFrequency;
        if (elapsedControllerSeconds >= 3.0f) {
            shouldCheckForNewControllers = true;
            lastControllerCheckTime = controllerCheckTime;
        }
        for (int i = 0; i < 4; ++i) {
            game_controller_input *controller = &input.controllers[i];
            if (controller->connected || shouldCheckForNewControllers) {
                XINPUT_STATE state = {};
                DWORD result = XInputGetState(i, &state);
                if (result == ERROR_SUCCESS) {
                    controller->connected = true;
                    updateController(controller, state);
                }
                else {
                    controller->connected = false;
                }
            }
        }
        if (shouldCheckForNewControllers) {
            shouldCheckForNewControllers = false;
        }

        // convert console input and window props to standard game format
        getWinConsoleInput(&winConsole, &consoleInput);

        input.upKey.down = consoleInput.upKey.down;
        input.upKey.justPressed = consoleInput.upKey.justPressed;
        input.downKey.down = consoleInput.downKey.down;
        input.downKey.justPressed = consoleInput.downKey.justPressed;
        input.leftKey.down = consoleInput.leftKey.down;
        input.leftKey.justPressed = consoleInput.leftKey.justPressed;
        input.rightKey.down = consoleInput.rightKey.down;
        input.rightKey.justPressed = consoleInput.rightKey.justPressed;
        input.actionKey.down = consoleInput.actionKey.down;
        input.actionKey.justPressed = consoleInput.actionKey.justPressed;
        input.enterKey.down = consoleInput.enterKey.down;
        input.enterKey.justPressed = consoleInput.enterKey.justPressed;
        input.backspaceDown = consoleInput.backspaceDown;
        input.pointerDown = consoleInput.pointerState.down;
        input.pointerJustDown = consoleInput.pointerState.justPressed;
        input.pointerX = consoleInput.pointerX;
        input.pointerY = consoleInput.pointerY;

        for (int i = 0; i < 10; ++i) {
            input.typedText[i] = consoleInput.typedText[i];
        }
        input.numTypedChars = consoleInput.numTypedChars;

        game_window gameWindow = {};
        gameWindow.width = winConsoleWindow.width;
        gameWindow.height = winConsoleWindow.height;
        gameWindow.buffer = winConsoleWindow.buffer;
        gameWindow.colors = winConsoleWindow.colors;

        // main game update
        updateGame(&input, &gameMemory, &options, gameWindow);

        // play audio
        LARGE_INTEGER audioStartTime;
        QueryPerformanceCounter(&audioStartTime);
        float timeFromFrameBeginning = ((float)(audioStartTime.QuadPart - lastCounter.QuadPart) /
                                        (float)perfCountFrequency);
        int numSoundSampleBytes;
        DWORD byteToLock;
        getNumSoundSamples(&soundOutput, &numSoundSampleBytes, &byteToLock, timeFromFrameBeginning);
        if (numSoundSampleBytes > 0) {
            game_sound_output gameSoundOutput = {};
            gameSoundOutput.samplesPerSecond = soundOutput.samplesPerSecond;
            gameSoundOutput.sampleCount = numSoundSampleBytes / soundOutput.bytesPerSample;
            gameSoundOutput.samples = soundSamples;

            getGameSoundSamples(&gameMemory, &gameSoundOutput);
            fillDirectSoundBuffer(&soundOutput, byteToLock, numSoundSampleBytes, &gameSoundOutput);
        }

        // render game to console
        writeWinConsoleCharsToWholeScreen(&winConsole, winConsoleWindow.buffer, winConsoleWindow.colors);
        renderWinConsole(&winConsole);
        waitForEndOfWinConsoleFrame(&winConsole);

        clearWinConsoleWindow(winConsoleWindow);

        // start new frame
        QueryPerformanceCounter(&lastCounter);
        startWinConsoleFrame(&winConsole);
    }

    FreeConsole();

    return 0;
}
