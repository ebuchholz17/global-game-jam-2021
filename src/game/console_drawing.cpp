#include "console_drawing.h"

void drawCharAtXY (char c, int x, int y, console_drawer *drawer) {
    x += drawer->windowStartX;
    y += drawer->windowStartY;
    game_window window = drawer->window;
    if (x >= 0 && x < window.width && y >= 0 && y < window.height) {
        window.buffer[(y * window.width + x)] = c;
        window.colors[(y * window.width + x)] = drawer->color;
    }
}

