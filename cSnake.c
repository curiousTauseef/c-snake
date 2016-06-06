#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>

#include "board.h"
#include "console.h"

// To compile: gcc -x c cSnake.c -o cSnake -lpthread

void     INThandler(int);

int width = 20;
int height = 10;

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

    int delay = 500000;

    struct Board board;
    board_setup(&board, width, height);

    int snakePieces[board.cellsCount];

    int snakeLength = 10;
    for (int i = 0; i < snakeLength; i++) {
        snakePieces[i] = (snakeLength - i) + 2;
    }

    console_disable_char();
    key input = 0;
    key inputNew = input;

    console_get_key_listen_start(&input);

    bool snakeGrew = false;
    int foodPosition = board_create_food_position(board, snakePieces, snakeLength);
    snake_move(&snakePieces[0], &snakeLength, KEY_RIGHT, board, foodPosition, &snakeGrew);

    console_show_cursor(false);

    while (true) {
        if (snakeGrew) {
            foodPosition = board_create_food_position(board, snakePieces, snakeLength);
        }
        board_draw(board, snakePieces, snakeLength, foodPosition);
        console_cursor_move_by(width, height);

        if (input > 0) {
            console_get_key_listen_stop();

            if (input == KEY_ESCAPE) {
                pre_quit();
                return 0;
            }

            inputNew = input;
            input = 0;
            console_get_key_listen_start(&input);
        }

        usleep(delay);

        snakeGrew = false;
        if (!snake_move(&snakePieces[0], &snakeLength, inputNew, board, foodPosition, &snakeGrew)) {
            printf("Game Over!\n");
            console_cursor_move_by(0, 1);
            usleep(5 * 1000000);
            pre_quit();
            return 0;
        }

        if (snakeGrew) {
            delay = delay - 40000;
            if (delay < 1000) {
                delay = 1000;
            }
        }
    }

    return 0;
}

void INThandler(int sig)
{
    pre_quit();
    exit(1);
}
