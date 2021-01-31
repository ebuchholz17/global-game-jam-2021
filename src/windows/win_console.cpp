#include <windows.h>
#include <fileapi.h>

#include <timeapi.h>

struct win_console {
    HANDLE inputHandle;
    HANDLE outputHandle;
    HANDLE standardOutputHandle;

    SMALL_RECT windowDimensions;
    COORD windowBufferSize;
    COORD windowWriteCoord;
    CHAR_INFO *windowBuffer;

    long long perfCountFrequency;
    LARGE_INTEGER lastCounter;
    float targetMSPerFrame;
};

struct win_console_key {
    bool down;
    bool justPressed;
};

struct win_console_input {
    win_console_key upKey;
    win_console_key downKey;
    win_console_key leftKey;
    win_console_key rightKey;
    win_console_key actionKey; // Z?
    win_console_key enterKey;
    bool backspaceDown;

    win_console_key pointerState;
    int pointerX;
    int pointerY;

    char typedText[10];
    int numTypedChars;
};

struct win_console_window {
    int width;
    int height;
    char *buffer;
    char *colors;
};

void initWinConsole (win_console *winConsole, float targetMSPerFrame) {
    winConsole->inputHandle = 
        CreateFileA("CONIN$",
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ,
                    0,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    0);
    winConsole->standardOutputHandle = 
        CreateFileA("CONOUT$",
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_WRITE,
                    0,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    0);
    winConsole->outputHandle = 
        CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  0,
                                  CONSOLE_TEXTMODE_BUFFER,
                                  0);
    CONSOLE_CURSOR_INFO cursorInfo = {};
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(winConsole->outputHandle, &cursorInfo);

    // turn off quick edit
    DWORD previousConsoleMode;
    GetConsoleMode(winConsole->inputHandle, &previousConsoleMode); 
    SetConsoleMode(winConsole->inputHandle, previousConsoleMode & ~ENABLE_QUICK_EDIT_MODE);

    LARGE_INTEGER perfCountFrequencyResult;
    QueryPerformanceFrequency(&perfCountFrequencyResult);
    winConsole->perfCountFrequency = perfCountFrequencyResult.QuadPart;
    winConsole->targetMSPerFrame = targetMSPerFrame;
}

void setWinConsoleOutputActive (win_console *winConsole) {
    SetConsoleActiveScreenBuffer(winConsole->outputHandle);
}

void setWinConsoleStandardOutputActive (win_console *winConsole) {
    SetConsoleActiveScreenBuffer(winConsole->standardOutputHandle);
}

int initWinConsoleBuffer (win_console *winConsole, int width, int height) {
    winConsole->windowDimensions = {};
    winConsole->windowDimensions.Left = 0;
    winConsole->windowDimensions.Top = 0;
    winConsole->windowDimensions.Bottom = (SHORT)height - 1;
    winConsole->windowDimensions.Right = (SHORT)width - 1;

    winConsole->windowBufferSize = {};
    winConsole->windowBufferSize.X = (SHORT)width;
    winConsole->windowBufferSize.Y = (SHORT)height;

    winConsole->windowWriteCoord = {};
    winConsole->windowWriteCoord.X = 0;
    winConsole->windowWriteCoord.Y = 0;
    
    CONSOLE_SCREEN_BUFFER_INFOEX screenBufferInfo;

    ZeroMemory(&screenBufferInfo, sizeof(CONSOLE_SCREEN_BUFFER_INFOEX));
    screenBufferInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    
    GetConsoleScreenBufferInfoEx(winConsole->outputHandle, &screenBufferInfo);
    
    screenBufferInfo.dwCursorPosition = winConsole->windowWriteCoord;
    screenBufferInfo.dwSize = winConsole->windowBufferSize;

    SetConsoleScreenBufferInfoEx(winConsole->outputHandle, &screenBufferInfo);

    // It seems you need to set the buffer size before setting the window size, otherwise the new window
    // size might not be reflected (updating the buffer seems to update the max window size)
    GetConsoleScreenBufferInfoEx(winConsole->outputHandle, &screenBufferInfo);
    screenBufferInfo.srWindow.Left = 0;
    screenBufferInfo.srWindow.Top = 0;
    screenBufferInfo.srWindow.Right = (SHORT)width;
    screenBufferInfo.srWindow.Bottom = (SHORT)height;
    SetConsoleScreenBufferInfoEx(winConsole->outputHandle, &screenBufferInfo);

    return sizeof(CHAR_INFO) * width * height;
} 

void copyLStr (WCHAR *buffer, WCHAR *string) {
    int index = 0;
    while (string[index] != 0 && index < LF_FACESIZE) {
        buffer[index] = string[index];
        ++index;
    }
}

void winConsoleSetFont (win_console *winConsole, int sizeX, int sizeY, WCHAR *fontName, int fontWeight) {
    CONSOLE_FONT_INFOEX fontInfo = {};
    fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);

    GetCurrentConsoleFontEx(winConsole->outputHandle, false, &fontInfo);

    copyLStr(fontInfo.FaceName, fontName);
    fontInfo.dwFontSize.X = (SHORT)sizeX;
    fontInfo.dwFontSize.Y = (SHORT)sizeY;

    fontInfo.FontFamily = FF_DONTCARE;
    fontInfo.FontWeight = fontWeight;

    SetCurrentConsoleFontEx(winConsole->outputHandle, false, &fontInfo);

    CONSOLE_SCREEN_BUFFER_INFOEX screenBufferInfo = {};
    screenBufferInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    
    // resize window
    // TODO(ebuchholz): move to separate function?
    GetConsoleScreenBufferInfoEx(winConsole->outputHandle, &screenBufferInfo);

    screenBufferInfo.srWindow.Right = (SHORT)(winConsole->windowBufferSize.X);
    screenBufferInfo.srWindow.Bottom = (SHORT)(winConsole->windowBufferSize.Y);

    SetConsoleScreenBufferInfoEx(winConsole->outputHandle, &screenBufferInfo);

    //SetConsoleScreenBufferSize(winConsole->outputHandle, winConsole->windowBufferSize);
}

void beginPreciseWindowsTiming () {
    timeBeginPeriod(1);
}

void endPreciseWindowsTiming () {
    timeEndPeriod(1);
}

void winConsoleClearWindow (win_console *winConsole) {
    // blank out the buffer to start
    for (int i = 0; i < winConsole->windowBufferSize.Y; ++i) {
        for (int j = 0; j < winConsole->windowBufferSize.X; ++j) {
            CHAR_INFO charInfo = {};
            charInfo.Char.AsciiChar = ' ';
            charInfo.Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
            winConsole->windowBuffer[i * winConsole->windowBufferSize.X + j] = charInfo;
        }
    }
}

void setWindowBufferMemory (win_console *winConsole, void *memory) {
    winConsole->windowBuffer = (CHAR_INFO *)memory;

    winConsoleClearWindow(winConsole);
}

void winConsoleGetWindowDimensions (win_console *winConsole, int *width, int *height) {
    CONSOLE_SCREEN_BUFFER_INFOEX screenBufferInfo = {};
    screenBufferInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(winConsole->outputHandle, &screenBufferInfo);

    *width = screenBufferInfo.dwSize.X;
    *height = screenBufferInfo.dwSize.Y;
}


void startWinConsoleFrame (win_console *winConsole) {
    QueryPerformanceCounter(&winConsole->lastCounter);
}

// TODO(ebuchholz): more precise timer: this tends to sleep too long
void waitForEndOfWinConsoleFrame (win_console *winConsole) {
    LARGE_INTEGER workCounter;
    QueryPerformanceCounter(&workCounter);
    long long counterElapsed = workCounter.QuadPart - winConsole->lastCounter.QuadPart;
    float elapsedMS = (1000.0f * (float)counterElapsed) / (float)winConsole->perfCountFrequency;

    if (elapsedMS < winConsole->targetMSPerFrame) {
        DWORD sleepMS = (DWORD)(winConsole->targetMSPerFrame - elapsedMS);
        if (sleepMS > 0) {
            Sleep(sleepMS);
        }
    }
}

void renderWinConsole (win_console *winConsole) {
    WriteConsoleOutput(winConsole->outputHandle,
                       winConsole->windowBuffer,       
                       winConsole->windowBufferSize,   
                       winConsole->windowWriteCoord,   
                       &winConsole->windowDimensions); 
}

void winConsoleWriteText (win_console *winConsole, char *text, int length, int x, int y) {
    for (int i = 0; i < length; ++i) {
        char letter = text[i];
        if (letter == 0) {
            break;
        }
        CHAR_INFO *charInfo = &winConsole->windowBuffer[y * winConsole->windowBufferSize.X + x + i];
        charInfo->Char.AsciiChar = letter;
    }
}

// assumes the characters buffer is the right size
void writeWinConsoleCharsToRect (win_console *winConsole, char *characters, 
                       int x, int y, int width, int height) 
{
    // TODO(ebuchholz)
}

void writeWinConsoleCharAtXY (win_console *winConsole, char character, int x, int y) {
    CHAR_INFO *charInfo = &winConsole->windowBuffer[y * winConsole->windowBufferSize.X + x];
    charInfo->Char.AsciiChar = character;
}

void writeWinConsoleCharsToWholeScreen (win_console *winConsole, char *characters, char *colors) {
    for (int i = 0; i < winConsole->windowBufferSize.Y; ++i) {
        for (int j = 0; j < winConsole->windowBufferSize.X; ++j) {
            CHAR_INFO *charInfo = &winConsole->windowBuffer[i * winConsole->windowBufferSize.X + j];
            charInfo->Char.AsciiChar = characters[i * winConsole->windowBufferSize.X + j];
            charInfo->Attributes = colors[i * winConsole->windowBufferSize.X + j];
        }
    }
}

void releaseWinConsoleHandles (win_console *winConsole) {
    CloseHandle(winConsole->outputHandle);
    CloseHandle(winConsole->standardOutputHandle);
    CloseHandle(winConsole->inputHandle);
}

void getWinConsoleInput (win_console *winConsole, win_console_input *input) {
    INPUT_RECORD inputs[100];
    DWORD numInputsRead;
    PeekConsoleInput(winConsole->inputHandle, inputs, 100, &numInputsRead);

    for (unsigned int i = 0; i < numInputsRead; ++i) {
        INPUT_RECORD inputRecord = inputs[i];
        if (inputRecord.EventType == KEY_EVENT) {
            KEY_EVENT_RECORD keyEvent = inputRecord.Event.KeyEvent;
            switch (keyEvent.wVirtualKeyCode) {
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':
                case VK_SPACE: {
                   if (input->numTypedChars < 10 && keyEvent.bKeyDown) {
                       input->typedText[input->numTypedChars++] = (char)keyEvent.wVirtualKeyCode;
                   }
                   if (keyEvent.wVirtualKeyCode == 'Z') {
                       if (keyEvent.bKeyDown) {
                           if (!input->actionKey.down) {
                               input->actionKey.justPressed = true;
                           }
                           input->actionKey.down = true;
                       }
                       else { 
                           input->actionKey.down = false;
                       }
                   }
                } break;
                case VK_RETURN: {
                    if (keyEvent.bKeyDown) {
                        if (!input->enterKey.down) {
                            input->enterKey.justPressed = true;
                        }
                        input->enterKey.down = true;
                    }
                    else { 
                        input->enterKey.down = false;
                    }
                } break;
                case VK_BACK: {
                    if (keyEvent.bKeyDown) {
                        input->backspaceDown = true;
                    }
                } break;
                case VK_UP: {
                    if (keyEvent.bKeyDown) {
                        if (!input->upKey.down) {
                            input->upKey.justPressed = true;
                        }
                        input->upKey.down = true;
                    }
                    else { 
                        input->upKey.down = false;
                    }
                } break;
                case VK_DOWN: {
                    if (keyEvent.bKeyDown) {
                        if (!input->downKey.down) {
                            input->downKey.justPressed = true;
                        }
                        input->downKey.down = true;
                    }
                    else { 
                        input->downKey.down = false;
                    }
                } break;
                case VK_LEFT: {
                    if (keyEvent.bKeyDown) {
                        if (!input->leftKey.down) {
                            input->leftKey.justPressed = true;
                        }
                        input->leftKey.down = true;
                    }
                    else { 
                        input->leftKey.down = false;
                    }
                } break;
                case VK_RIGHT: {
                    if (keyEvent.bKeyDown) {
                        if (!input->rightKey.down) {
                            input->rightKey.justPressed = true;
                        }
                        input->rightKey.down = true;
                    }
                    else { 
                        input->rightKey.down = false;
                    }
                } break;
            }
        }
        else if (inputRecord.EventType == MOUSE_EVENT) {
            int t = 0;
            MOUSE_EVENT_RECORD mouseEvent = inputRecord.Event.MouseEvent;
            if (mouseEvent.dwEventFlags == MOUSE_MOVED) {
                input->pointerX = mouseEvent.dwMousePosition.X;
                input->pointerY = mouseEvent.dwMousePosition.Y;
            }
            if ((mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) == 1) {
                if (!input->pointerState.down) {
                    input->pointerState.justPressed = true;
                }
                input->pointerState.down = true;
            }
            else if ((mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) == 0) {
                input->pointerState.down = false;
            }
        }
    }

    FlushConsoleInputBuffer(winConsole->inputHandle);
}

void winConsoleResetInput (win_console_input *input) {
    input->upKey.justPressed = false;
    input->downKey.justPressed = false;
    input->leftKey.justPressed = false;
    input->rightKey.justPressed = false;
    input->backspaceDown = false;
    input->enterKey.justPressed = false;
    input->actionKey.justPressed = false;
    input->pointerState.justPressed = false;
    input->numTypedChars = 0;
}


