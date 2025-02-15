#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

#pragma once

#include <stdbool.h>
#include <windows.h>

typedef struct {
    SHORT width;
    SHORT height;
} ConsoleSize;

typedef enum {
    COLOR_BLACK        = 0,
    COLOR_DARK_BLUE    = 1,
    COLOR_DARK_GREEN   = 2,
    COLOR_DARK_CYAN    = 3,
    COLOR_DARK_RED     = 4,
    COLOR_DARK_MAGENTA = 5,
    COLOR_DARK_YELLOW  = 6,
    COLOR_GRAY         = 7,
    COLOR_DARK_GRAY    = 8,
    COLOR_BLUE         = 9,
    COLOR_GREEN        = 10,
    COLOR_CYAN         = 11,
    COLOR_RED          = 12,
    COLOR_MAGENTA      = 13,
    COLOR_YELLOW       = 14,
    COLOR_WHITE        = 15
} ConsoleColor;

#define FG(color) ((WORD)(color))
#define BG(color) ((WORD)(color) << 4)
#define COLOR_PAIR(fg, bg) (FG(fg) | BG(bg))

void console_init(const wchar_t* title, ConsoleSize size);
void console_cleanup();
ConsoleSize console_get_size();
bool console_resize(ConsoleSize size);

void cursor_show(bool visible);
void cursor_set_pos(SHORT x, SHORT y);

void color_apply(WORD attributes);

typedef enum {
    KEY_UP    = VK_UP,
    KEY_DOWN  = VK_DOWN,
    KEY_LEFT  = VK_LEFT,
    KEY_RIGHT = VK_RIGHT,
    KEY_ESC   = VK_ESCAPE,
    KEY_SPACE = VK_SPACE,
    KEY_ENTER = VK_RETURN
} KeyCode;

bool input_key_pressed(KeyCode key);
bool input_key_held(KeyCode key);
void input_update();

typedef struct {
    CHAR_INFO* buffer;
    ConsoleSize size;
} ScreenBuffer;

ScreenBuffer screen_create(ConsoleSize size);
void screen_destroy(ScreenBuffer* screen);
void screen_clear(ScreenBuffer* screen, wchar_t fill_char);
void screen_draw(ScreenBuffer* screen);
void screen_set_cell(ScreenBuffer* screen, SHORT x, SHORT y, wchar_t ch, WORD attributes);

double time_get_delta();
void time_sleep_ms(DWORD ms);

#endif