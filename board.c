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

#define BOARD_WIDTH_DEFAULT  50;
#define BOARD_HEIGHT_DEFAULT 20;

#define MANUAL_DESCRIPTION "Usage: cSnake [options]\n\
\n\
Options:\n\
  -h, --height <lines>   The height of the game (bounding box)\n\
  -w, --width <columns>  The width of the game (bounding box)\n\
  -a, --wrap-around      Whether the snake can run through the walls\n\
  -e, --help             Displays this help\n"

struct Board {
    bool wrapAround;
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

void board_setup(struct Board *board, int width, int height, bool wrapAround)
{
    board->width = width;
    board->height = height;
    board->wrapAround = wrapAround;
    board->padding.top = 1;
    board->padding.bottom = 1;
    board->padding.left = 1;
    board->padding.right = 1;

    board->innerWidth = board->width - board->padding.left - board->padding.right;
    board->innerHeight = board->height - board->padding.top - board->padding.bottom;

    board->cellsCount = board->innerWidth * board->innerHeight;
}

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

/*
 * Returns true to indicate that the game is not over after this move
 */
bool snake_move(int *snakePieces, int *snakeLength, key direction, struct Board board, int foodPosition, bool *snakeGrew)
{
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

    int headMoveBy;
    switch (direction) {
        case KEY_UP:
            headMoveBy = - board.innerWidth;
        break;
        case KEY_DOWN:
            headMoveBy = board.innerWidth;
        break;
        case KEY_RIGHT:
            headMoveBy = 1;
        break;
        case KEY_LEFT:
            headMoveBy = - 1;
        break;
    }

    int newHeadPosition = snakePieces[0] + headMoveBy;

    if (foodPosition == newHeadPosition) {
        *snakeLength = *snakeLength + 1;
        *snakeGrew = true;
    }

    for (int i = *snakeLength - 1; i; i--) {
        snakePieces[i] = snakePieces[i - 1];
    }

    if (foodPosition != newHeadPosition) {
        bool hitTail = is_snake_body(
            snakePieces,
            *snakeLength,
            newHeadPosition
        );

        if (hitTail) {
            return false;
        }
    }

    snakePieces[0] = newHeadPosition;

    if (snake_hit_wall(board, direction, snakePieces[0])) {
        // If we hit a wall and this is a wrap-around game
        // then the game isn't over, the below code will figure out where to draw the head
        if (!board.wrapAround) {
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

int board_create_food_position(struct Board board, int *snakePieces, int snakeLength)
{
    // Seems to get a nice random-ish seed, better than time
    srand (clock());

    for (int i = 0; i < 1000000; i++) {
        int position = rand() % (board.cellsCount + 1);

        bool isSnake = is_snake_body(
            snakePieces,
            snakeLength,
            position
        );

        if (!isSnake) {
            return position;
        }
    }

    return 0;
}

/*
 * Called for each tick of the main draw loop
 * will redraw the board and will also put food and snake in
 * Does not store a 1:1 view of the output, it just prints each char by
 * calculating what each grid square should be
 */
void board_draw(struct Board board, int *snakePieces, int snakeLength, int foodPosition)
{
    // Non wrap around
    char outline[6][4] = {
        "\u2554", // |``   // OUTLINE_TOP_LEFT
        "\u2550", // --    // OUTLINE_HORIZONTAL
        "\u2557", // ``|   // OUTLINE_TOP_RIGHT
        "\u2551", // |     // OUTLINE_VERTICAL
        "\u255A", // |_    // OUTLINE_BOTTOM_LEFT
        "\u255D"  // _|    // OUTLINE_BOTTOM_RIGHT
    };

    if (board.wrapAround) {
        strcpy(outline[0], "\u256D"); // |``   // OUTLINE_TOP_LEFT
        strcpy(outline[1], "\u2500"); // --    // OUTLINE_HORIZONTAL
        strcpy(outline[2], "\u256E"); // ``|   // OUTLINE_TOP_RIGHT
        strcpy(outline[3], "\u2502"); // |     // OUTLINE_VERTICAL
        strcpy(outline[4], "\u2570"); // |_    // OUTLINE_BOTTOM_LEFT
        strcpy(outline[5], "\u256F"); // _|    // OUTLINE_BOTTOM_RIGHT
    }

    char *body = "\u2588";
    char *food = "\u25AA";

    printf("%s", outline[OUTLINE_TOP_LEFT]);
    for (int i = 0; i < board.innerWidth; i++) {
        printf("%s", outline[OUTLINE_HORIZONTAL]);
    }

    printf("%s\n", outline[OUTLINE_TOP_RIGHT]);

    for (int i = 0; i < board.innerHeight; i++) {
        printf("%s", outline[OUTLINE_VERTICAL]);
        for (int j = 0; j < board.innerWidth; j++) {
            bool isSnake = is_snake_body(
                snakePieces,
                snakeLength,
                ((i * board.innerWidth) + j)
            );

            if (isSnake) {
                printf("%s", body);
            } else if ((i * board.innerWidth) + j == foodPosition) {
                printf("%s", food);
            } else {
                printf(" ");
            }
        }

        printf("%s\n", outline[OUTLINE_VERTICAL]);
    }

    printf("%s", outline[OUTLINE_BOTTOM_LEFT]);
    for (int i = 0; i < board.innerWidth; i++) {
        printf("%s", outline[OUTLINE_HORIZONTAL]);
    }

    printf("%s\n", outline[OUTLINE_BOTTOM_RIGHT]);
}
