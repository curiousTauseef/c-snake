#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#include "board.h"
#include "console.h"

void     INThandler(int);

int width = 40;
int height = 20;

void pre_quit()
{
    console_get_key_listen_cancel();
    destroy_board(width, height);
    console_cursor_move_by(width, height);
    console_enable_char();
    console_show_cursor(true);
}

int main()
{
    signal(SIGINT, INThandler);
    int delay = 20; // hundredths

    int innerWidth = width - 2;

    struct Board board;
    board.width = width;
    board.height = height;
    board.innerWidth = width - 2;
    board.innerHeight = height - 2;

    int snakePieces[(width - 2) * (height - 2)];

    snakePieces[0] = (innerWidth * 3) + 5;
    snakePieces[1] = (innerWidth * 3) + 4;
    snakePieces[2] = (innerWidth * 3) + 3;
    snakePieces[3] = (innerWidth * 3) + 2;
    snakePieces[4] = -1;

    console_disable_char();
    key input = 0;

    console_get_key_listen_start(&input);
    snake_set_last_position(KEY_RIGHT);
    console_show_cursor(false);

    while (true) {
        draw_board(width, height, snakePieces);
        console_cursor_move_by(width, height);

        if (input > 0) {
            console_get_key_listen_stop();

            if (input == KEY_ESCAPE) {
                pre_quit();
                return 0;
            }

            input = 0;
            console_get_key_listen_start(&input);
        }

        usleep(delay * 10000);
        bool okMove = snake_move(&snakePieces[0], input, board);
        if (!okMove) {
            printf("Game Over!\n");
            console_cursor_move_by(0, 1);
            usleep(5 * 1000000);
            pre_quit();
            return 0;
        }
    }

    return 0;
}

void INThandler(int sig)
{
    pre_quit();
    exit(1);
}
