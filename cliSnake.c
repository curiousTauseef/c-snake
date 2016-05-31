#include <stdio.h>
#include <string.h>
#include "board.h"

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

    draw_board(width, height, snakePieces);
}
