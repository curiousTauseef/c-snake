#include "keys.h"

typedef int bool;
#define true  1
#define false 0

#define OUTLINE_TOP_LEFT     0
#define OUTLINE_HORIZONTAL   1
#define OUTLINE_TOP_RIGHT    2
#define OUTLINE_VERTICAL     3
#define OUTLINE_BOTTOM_LEFT  4
#define OUTLINE_BOTTOM_RIGHT 5

#define PADDING_SIDES 2
#define PADDING_ENDS  2

struct Board {
    int width;
    int innerWidth;
    int height;
    int innerHeight;
};

int is_snake_body(int row, int col, int colCount, int *snakePieces)
{
    int i = 0;
    while (true) {
        if (snakePieces[i] == -1) {
            return false;
        }

        if (snakePieces[i] == (row * colCount) + col) {
            return true;
        }
        i++;
    }
}

void destroy_board(int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf(" ");
        }
        printf("\n");
    }
}

key lastDirection;

void snake_set_last_position(key direction)
{
    lastDirection = direction;
}

bool snake_is_try_to_reverse(int direction)
{
    if (lastDirection == KEY_UP && direction == KEY_DOWN) {
        return true;
    }

    if (lastDirection == KEY_DOWN && direction == KEY_UP) {
        return true;
    }

    if (lastDirection == KEY_LEFT && direction == KEY_RIGHT) {
        return true;
    }

    if (lastDirection == KEY_RIGHT && direction == KEY_LEFT) {
        return true;
    }

    return false;
}

bool snake_move(int *snakePieces, key direction, struct Board board)
{
    bool isWrapAround = false;

    switch (direction) {
        case KEY_UP:
        case KEY_DOWN:
        case KEY_LEFT:
        case KEY_RIGHT:
            if (!snake_is_try_to_reverse(direction)) {
                lastDirection = direction;
                break;
            }
        default: // not matched - or trying to reverse
            direction = lastDirection;
    }

    snakePieces[3] = snakePieces[2];
    snakePieces[2] = snakePieces[1];
    snakePieces[1] = snakePieces[0];
    switch (direction) {
        case KEY_UP:
            snakePieces[0] = snakePieces[0] - board.innerWidth;

            if (!isWrapAround) {
                if (snakePieces[0] < 0) {
                    return false;
                }
            }
        break;
        case KEY_DOWN:
            snakePieces[0] = snakePieces[0] + board.innerWidth;

            if (!isWrapAround) {
                if (snakePieces[0] > board.innerWidth * board.innerHeight) {
                    return false;
                }
            }
        break;
        case KEY_RIGHT:
            snakePieces[0] = snakePieces[0] + 1;

            if (!isWrapAround) {
                if (snakePieces[0] % board.innerWidth == 0) {
                    return false;
                }
            }
        break;
        case KEY_LEFT:
            snakePieces[0] = snakePieces[0] - 1;

            if (!isWrapAround) {
                if ((snakePieces[0] + 1) % board.innerWidth == 0) {
                    return false;
                }
            }
        break;
    }

    return true;
}

void draw_board(int width, int height, int *snakePieces)
{
    // (unsigned)strlen(s) == 3 (+1)
    char outline[6][4] = {
        "\u256D", // |``   // OUTLINE_TOP_LEFT
        "\u2500", // --    // OUTLINE_HORIZONTAL
        "\u256E", // ``|   // OUTLINE_TOP_RIGHT
        "\u2502", // |     // OUTLINE_VERTICAL
        "\u2570", // |_    // OUTLINE_BOTTOM_LEFT
        "\u256F"  // _|    // OUTLINE_BOTTOM_RIGHT
    };

    char *body = "\u2588";

    int innerWidth = width - PADDING_SIDES;
    int innerHeight = height - PADDING_ENDS;

    printf("%s", outline[OUTLINE_TOP_LEFT]);
    for (int i = 0; i < innerWidth; i++) {
        printf("%s", outline[OUTLINE_HORIZONTAL]);
    }

    printf("%s\n", outline[OUTLINE_TOP_RIGHT]);

    for (int i = 0; i < innerHeight; i++) {
        printf("%s", outline[OUTLINE_VERTICAL]);
        for (int j = 0; j < innerWidth; j++) {
            bool is_snake = is_snake_body(i, j, innerWidth, snakePieces);
            printf("%s", (is_snake) ? body : " ");
        }
        printf("%s\n", outline[OUTLINE_VERTICAL]);
    }

    printf("%s", outline[OUTLINE_BOTTOM_LEFT]);
    for (int i = 0; i < innerWidth; i++) {
        printf("%s", outline[OUTLINE_HORIZONTAL]);
    }

    printf("%s\n", outline[OUTLINE_BOTTOM_RIGHT]);
}
