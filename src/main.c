#include "console_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FRAME_DELAY_MS 50
#define MIN_TAIL_LENGTH 5
#define MAX_TAIL_LENGTH 15

typedef struct {
    int pos;
    int tailLength;
    wchar_t* chars;
} MatrixDrop;

int main() {
    srand((unsigned int)time(NULL));

    ConsoleSize size = { 100, 50 };
    console_init(L"Matrix", size);
    cursor_show(false);

    ScreenBuffer screen = screen_create(size);
    if (!screen.buffer) {
        fprintf(stderr, "Failed to allocate screen buffer.\n");
        console_cleanup();
        return 1;
    }

    MatrixDrop* drops = (MatrixDrop*)malloc(sizeof(MatrixDrop) * size.width);
    if (!drops) {
        fprintf(stderr, "Failed to allocate memory for drops.\n");
        screen_destroy(&screen);
        console_cleanup();
        return 1;
    }

    for (int i = 0; i < size.width; i++) {
        drops[i].pos = -(rand() % size.height);
        drops[i].tailLength = (rand() % (MAX_TAIL_LENGTH - MIN_TAIL_LENGTH + 1) + MIN_TAIL_LENGTH);
        drops[i].chars = (wchar_t*)malloc(sizeof(wchar_t) * drops[i].tailLength);
        
        if (!drops[i].chars) {
            fprintf(stderr, "Failed to allocate memory for characters.\n");
            for (int j = 0; j < i; j++) {
                free(drops[j].chars);
            }
            free(drops);
            screen_destroy(&screen);
            console_cleanup();
            return 1;
        }
        
        for (int j = 0; j < drops[i].tailLength; j++) {
            drops[i].chars[j] = (wchar_t)(rand() % 94 + 33);
        }
    }

    while (!input_key_pressed(KEY_ESC)) {
        input_update();
        screen_clear(&screen, L' ');

        for (int x = 0; x < size.width; x++) {
            for (int i = drops[x].tailLength - 1; i > 0; i--) {
                drops[x].chars[i] = drops[x].chars[i - 1];
            }
            drops[x].chars[0] = (wchar_t)(rand() % 94 + 33);

            for (int i = 0; i < drops[x].tailLength; i++) {
                int y = drops[x].pos - i;
                if (y >= 0 && y < size.height) {
                    WORD attr = (i == 0) ? COLOR_PAIR(COLOR_WHITE, COLOR_BLACK) : COLOR_PAIR(COLOR_GREEN, COLOR_BLACK);
                    screen_set_cell(&screen, x, y, drops[x].chars[i], attr);
                }
            }

            drops[x].pos++;

            if (drops[x].pos > size.height + drops[x].tailLength) {
                drops[x].pos = -(rand() % size.height);
                drops[x].tailLength = (rand() % (MAX_TAIL_LENGTH - MIN_TAIL_LENGTH + 1) + MIN_TAIL_LENGTH);
                
                wchar_t* new_chars = (wchar_t*)realloc(drops[x].chars, sizeof(wchar_t) * drops[x].tailLength);
                if (new_chars) {
                    drops[x].chars = new_chars;
                    for (int i = 0; i < drops[x].tailLength; i++) {
                        drops[x].chars[i] = (wchar_t)(rand() % 94 + 33);
                    }
                }
            }
        }

        screen_draw(&screen);
        time_sleep_ms(FRAME_DELAY_MS);
    }

    for (int i = 0; i < size.width; i++) {
        free(drops[i].chars);
    }
    free(drops);
    screen_destroy(&screen);
    console_cleanup();

    return 0;
}