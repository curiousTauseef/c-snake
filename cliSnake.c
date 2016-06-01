#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#include "board.h"
#include "console.h"

void     INThandler(int);

int width = 40;
int height = 20;

int main() {
    signal(SIGINT, INThandler);
    int delay = 1; // second

    int innerWidth = width - 2;

    int snakePieces[(width - 2) * (height - 2)];

    snakePieces[0] = (innerWidth * 3) + 2;
    snakePieces[1] = (innerWidth * 3) + 3;
    snakePieces[2] = (innerWidth * 3) + 4;
    snakePieces[3] = (innerWidth * 3) + 5;
    snakePieces[4] = -1;

    console_disable_char();
    key input = 0;

    console_get_key_listen_start(&input);

    int g = 3;
    console_show_cursor(false);
    while (true) {
        snakePieces[0] = (innerWidth * g) + 2;
        snakePieces[1] = (innerWidth * g) + 3;
        snakePieces[2] = (innerWidth * g) + 4;
        snakePieces[3] = (innerWidth * g) + 5;
        snakePieces[4] = -1;

        draw_board(width, height, snakePieces);
        console_cursor_move_by(width, height);

        if (input > 0) {
            console_get_key_listen_stop();

            if (input == KEY_ESCAPE) {
                destroy_board(width, height);
                console_cursor_move_by(width, height);
                console_enable_char();
                console_show_cursor(true);
                return 0;
            }

            input = 0;
            console_get_key_listen_start(&input);
            g--;
        } else {
            g++;
        }

        usleep(delay * 100000);
    }

    return 0;
}

void INThandler(int sig)
{
    console_get_key_listen_cancel();
    destroy_board(width, height);
    console_cursor_move_by(width, height);
    console_enable_char();
    console_show_cursor(true);

    exit(1);
}
