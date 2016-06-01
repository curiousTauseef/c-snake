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
