#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <curses.h>
#include <locale.h>

enum {
    N = 10000,
    DY = 5,
    DX = 10
};

int min(int a, int b) {
    return a < b ? a : b;
}

size_t digit_size(size_t i) {
    size_t res = 0;
    while (i > 0) {
        res++;
        i /= 10;
    }

    return res + 3;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        initscr();
        move(LINES/2, COLS/2 - 17);
        printw("PageViewer can`t find a file to show");
        getch();
        endwin();

        return 0;
    }

    FILE* file = fopen(argv[1], "r");

    if (!file) {
        initscr();
        move(LINES/2, COLS/2 - 17);
        printw("PageViewer can`t open a file to show");
        getch();
        endwin();

        return 0;
    }

    char** file_buffer = malloc(N * sizeof(*file_buffer));
    size_t n = 0, size = 255;
    file_buffer[n] = malloc(size * sizeof(*file_buffer[0]));
    int c_count;
    while ((c_count = getline(&file_buffer[n], &size, file)) != -1) {
        file_buffer[n][strlen(file_buffer[n]) - 1] = '\0';
        file_buffer[++n] = malloc(size * sizeof(*file_buffer[0]));
    }

    file_buffer = realloc(file_buffer, n * sizeof(*file_buffer));
    fclose(file);

    WINDOW* win;
    WINDOW* frame;
    char c = 0;

    setlocale(LC_ALL, "");

    initscr();

    size_t
        WIN_Y = LINES - 2*DY,
        WIN_X = COLS - 2*DX,
        FRAME_Y = WIN_Y + 2,
        FRAME_X = WIN_X + 2;

    noecho();
    cbreak();

    move(0, 0);
    printw("File: %s, size: %ld\n", argv[1], n);
    refresh();

    frame = newwin(FRAME_Y, FRAME_X, DY-1, DX-1);
    box(frame, 0, 0);
    wrefresh(frame);

    win = newwin(WIN_Y, WIN_X, DY, DX);
    keypad(win, TRUE);
    scrollok(win, FALSE);

    size_t start_x = 0, start_y = 0;
    for (size_t i = start_x; i - start_x < WIN_Y && i < n; i++) {
        wprintw(win, "%ld: %.*s\n", i+1, min(strlen(&file_buffer[i][start_y]), WIN_X - digit_size(i+1)), &file_buffer[i][start_y]);
    }
    wrefresh(win);

    while((c = wgetch(win)) != 27) {
        werase(win);
        
        switch(c) {
            case 32:
            case 2:
                start_x += (start_x + WIN_Y < n);
                break;
            case 3:
                start_x -= (start_x > 0);
                break;
            case 5:
                start_y += (start_y + WIN_X + 5 < size);
                break;
            case 4:
                start_y -= (start_y > 0);
                break;

        }
        for (size_t i = start_x; i - start_x < WIN_Y && i < n; i++) {
            wprintw(win, "%ld: %.*s\n", i+1, min(strlen(&file_buffer[i][start_y]), WIN_X - digit_size(i+1)), &file_buffer[i][start_y]);
        }
        wrefresh(win);
    }

    delwin(frame);
    delwin(win);
    endwin();
    return 0;
}

