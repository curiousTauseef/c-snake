#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <getopt.h>

#include "board.c"
#include "console.c"

// To compile: gcc -x c cSnake.c -o c-snake -lpthread

void INThandler(int);
struct Board board;

void pre_quit()
{
    console_get_key_listen_cancel();
    destroy_board(board.width, board.height);
    console_cursor_move_by(board.width, board.height);
    console_enable_char();
    console_show_cursor(true);
}

/**
 * Prints out standard unix usage text
 */
void console_usage_print() {
    printf(MANUAL_DESCRIPTION);
}

void options_parse(int argc, char **argv, struct Board *board)
{
    int opt = 0;
    int width = BOARD_WIDTH_DEFAULT;
    int height = BOARD_HEIGHT_DEFAULT;
    bool wrapAround = false;

    //Specifying the expected options
    static struct option long_options[] = {
        // Ags are actually all optional
        {"help",          no_argument,       0,  'e'},
        {"width",         required_argument, 0,  'w'},
        {"height",        required_argument, 0,  'h'},
        {"wrap-around",   no_argument,       0,  'a'},
        {0,               0,                 0,   0 }
    };

    int long_index = 0;
    while ((opt = getopt_long(argc, argv,"w:h:a",
                  long_options, &long_index )) != -1) {
        switch (opt) {
            case 'a':
                wrapAround = true;
                break;
            case 'e':
                console_usage_print();
                exit(0);
                break;
            case 'h' : height = atoi(optarg);
                break;
            case 'w' : width = atoi(optarg);
                break;
            default: console_usage_print();
                exit(EXIT_FAILURE);
        }
    }

    board_setup(board, width, height, wrapAround);
}

int main(int argc, char **argv)
{
    signal(SIGINT, INThandler);
    options_parse(argc, argv, &board);

    int delay = 500000;
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
        console_cursor_move_by(board.width, board.height);

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
            delay = delay - 20000;
            if (delay < 10000) {
                delay = 10000;
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
