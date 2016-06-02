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
    int cellsCount;
    struct {
        int top;
        int bottom;
        int left;
        int right;
    } padding;
};

//int is_snake_body(int row, int col, int colCount, int *snakePieces, int snakeLength)
int is_snake_body(int *snakePieces, int snakeLength, int cell)
{
    for (int i = 0; i < snakeLength; i++) {
        if (snakePieces[i] == cell) {
            return true;
        }
    }

    return false;
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
bool snake_is_try_to_reverse(key direction)
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

bool snake_hit_wall(struct Board board, key direction, int snakeHead)
{
    // direction == KEY_UP
    if (snakeHead < 0) {
        return true;
    }

    // KEY_DOWN
    if (snakeHead > board.innerWidth * board.innerHeight) {
        return true;
    }

    if (direction == KEY_RIGHT && snakeHead % board.innerWidth == 0) {
        return true;
    }

    if (direction == KEY_LEFT && (snakeHead + 1) % board.innerWidth == 0) {
        return true;
    }

    return false;
}

bool snake_move(int *snakePieces, int *snakeLength, key direction, struct Board board)
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

    for (int i = *snakeLength - 1; i; i--) {
        snakePieces[i] = snakePieces[i - 1];
    }

    switch (direction) {
        case KEY_UP:
            snakePieces[0] = snakePieces[0] - board.innerWidth;
        break;
        case KEY_DOWN:
            snakePieces[0] = snakePieces[0] + board.innerWidth;
        break;
        case KEY_RIGHT:
            snakePieces[0] = snakePieces[0] + 1;
        break;
        case KEY_LEFT:
            snakePieces[0] = snakePieces[0] - 1;
        break;
    }

    if (snake_hit_wall(board, direction, snakePieces[0])) {
        if (!isWrapAround) {
            return false;
        }

        switch (direction) {
            case KEY_UP:
                snakePieces[0] = snakePieces[0] + (board.innerWidth * board.innerHeight);
            break;
            case KEY_DOWN:
                snakePieces[0] = snakePieces[0] - (board.innerWidth * board.innerHeight);
            break;
            case KEY_RIGHT:
                snakePieces[0] = snakePieces[0] - board.innerWidth;
            break;
            case KEY_LEFT:
                snakePieces[0] = snakePieces[0] + board.innerWidth;
            break;
        }
    }

    return true;
}

void draw_board(int width, int height, int *snakePieces, int snakeLength)
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
            bool is_snake = is_snake_body(
                snakePieces,
                snakeLength,
                ((i * innerWidth) + j));

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
