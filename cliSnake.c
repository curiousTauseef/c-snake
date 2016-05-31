#include <stdio.h>
#include <string.h>
#include "board.h"
#include "console.h"

typedef int key;

int main() {
    int width = 40;
    int height = 20;

    int innerWidth = width - 2;

    int snakePieces[(width - 2) * (height - 2)];

    snakePieces[0] = (innerWidth * 3) + 2;
    snakePieces[1] = (innerWidth * 3) + 3;
    snakePieces[2] = (innerWidth * 3) + 4;
    snakePieces[3] = (innerWidth * 3) + 5;
    snakePieces[4] = -1;

    console_disable_char();
    while (true) {
        draw_board(width, height, snakePieces);
        console_cursor_move_by(width, height);
        key input = console_get_key();

        if (input == KEY_ESCAPE) {
            destroy_board(width, height);
            console_cursor_move_by(width, height);
            console_enable_char();
            return 0;
        }




        snakePieces[0] = (innerWidth * 4) + 2;
        snakePieces[1] = (innerWidth * 4) + 3;
        snakePieces[2] = (innerWidth * 4) + 4;
        snakePieces[3] = (innerWidth * 4) + 5;
        snakePieces[4] = (innerWidth * 4) + 6;
        snakePieces[5] = -1;
    }

    return 0;
}
