#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>


#include "board.h"
#include "console.h"
#include "game.h"

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
    game_setup_board();
    int delay = 20; // hundredths

    struct Board board;
    board.width = width;
    board.height = height;

    board.padding.top = 1;
    board.padding.bottom = 1;
    board.padding.left = 1;
    board.padding.right = 1;

    board.innerWidth = board.width - board.padding.left - board.padding.right;
    board.innerHeight = board.height - board.padding.top - board.padding.bottom;

    board.cellsCount = board.innerWidth * board.innerHeight;

    int snakePieces[board.cellsCount];

    int snakeLength = 20;
    for (int i = 0; i < snakeLength; i++) {
        snakePieces[i] = (snakeLength - i) + 2;
    }

    console_disable_char();
    key input = 0;

    console_get_key_listen_start(&input);

    snake_move(&snakePieces[0], &snakeLength, KEY_RIGHT, board);

    console_show_cursor(false);

    while (true) {
        draw_board(width, height, snakePieces, snakeLength);
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

        if (!snake_move(&snakePieces[0], &snakeLength, input, board)) {
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
