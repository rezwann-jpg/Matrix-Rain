#include "console_utils.h"
#include <stdio.h>
#include <stdlib.h>

static HANDLE hConsoleInput;
static HANDLE hConsoleOutput;
static ConsoleColor currentFg = COLOR_WHITE;
static ConsoleColor currentBg = COLOR_BLACK;
static DWORD lastKeyState[256] = { 0 };
static LARGE_INTEGER frequency;
static LARGE_INTEGER lastTime; 

ConsoleSize console_get_size() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsoleOutput, &csbi);
    return (ConsoleSize){ csbi.dwSize.X, csbi.dwSize.Y };
}

bool console_resize(ConsoleSize size) {
    COORD coord = { size.width, size.height };
    if (!SetConsoleScreenBufferSize(hConsoleOutput, coord)) {
        return false;
    }

    SMALL_RECT rect = { 0, 0, size.width - 1, size.height - 1 };
    if (!SetConsoleWindowInfo(hConsoleOutput, TRUE, &rect)) {
        return false;
    }

    return true;
}

void console_init(const wchar_t* title, ConsoleSize size) {
    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);

    SetConsoleTitleW(title);
    
    if (!console_resize(size)) {
        fprintf(stderr, "Failed to resize console\n");
        exit(1);
    }

    DWORD mode = 0;
    GetConsoleMode(hConsoleInput, &mode);
    SetConsoleMode(hConsoleInput, mode & ~ENABLE_ECHO_INPUT & ~ENABLE_LINE_INPUT);

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);
}

void console_cleanup() { 
    cursor_show(true);
    SetConsoleMode(hConsoleInput, ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
}

void cursor_show(bool visible) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsoleOutput, &cursorInfo);
    cursorInfo.bVisible = visible;
    SetConsoleCursorInfo(hConsoleOutput, &cursorInfo);
}

void cursor_set_pos(SHORT x, SHORT y) {
    COORD coord = { x, y };
    SetConsoleCursorPosition(hConsoleOutput, coord);
}

void color_apply(WORD attributes) {
    SetConsoleTextAttribute(hConsoleOutput, attributes);
}

void input_update() {
    for (int i = 0; i < 256; i++) {
        lastKeyState[i] = GetAsyncKeyState(i);
    }
}

bool input_key_pressed(KeyCode key) {
    return (GetAsyncKeyState(key) & 0x8000) && !(lastKeyState[key] & 0x8000);
}

bool input_key_held(KeyCode key) {
    return (GetAsyncKeyState(key) & 0x8000);
}

ScreenBuffer screen_create(ConsoleSize size) {
    ScreenBuffer screen;
    screen.size = size;
    screen.buffer = (CHAR_INFO *)malloc(sizeof(CHAR_INFO) * size.width * size.height);
    return screen;
}

void screen_destroy(ScreenBuffer* screen) {
    free(screen->buffer);
    screen->buffer = NULL;
}

void screen_clear(ScreenBuffer* screen, wchar_t fill_char) {
    for (int y = 0; y < screen->size.height; y++) {
        for (int x = 0; x < screen->size.width; x++) {
            screen_set_cell(screen, x, y, fill_char, COLOR_PAIR(COLOR_WHITE, COLOR_BLACK));
        }
    }
}

void screen_draw(ScreenBuffer* screen) {
    SMALL_RECT rect = { 0, 0, screen->size.width - 1, screen->size.height - 1 };
    WriteConsoleOutputW(hConsoleOutput, screen->buffer, (COORD) { screen->size.width, screen->size.height }, (COORD) { 0, 0 }, &rect);
}

void screen_set_cell(ScreenBuffer* screen, SHORT x, SHORT y, wchar_t ch, WORD attributes) {
    if (x >= 0 && x < screen->size.width && y >= 0 && y < screen->size.height) {
        int index = y * screen->size.width + x;
        screen->buffer[index].Char.UnicodeChar = ch;
        screen->buffer[index].Attributes = attributes;
    }
}

double time_get_delta() {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    double delta = (double)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
    lastTime = currentTime;
    return delta;
}

void time_sleep_ms(DWORD ms) {
    Sleep(ms);
}