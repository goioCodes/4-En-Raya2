#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "connect4.h"

void connect4Main()
{
    Board board;
    initializeBoard(&board);
    printBoard(&board);
}

void printBoard(Board* board)
{
    printHBar();
    for (int i = 0; i < NUM_ROWS; i++)
    {
        printf("|");
        for (int j = 0; j < NUM_COLS; j++)
        {
            printf("% i |", board->m[i][j]);
        }
        printf("\n");
        printHBar();
    }
}

void printHBar()
{
    for (int j = 0; j < NUM_COLS; j++)
    {
        printf("____");
    }
    printf("\n");
}

void initializeBoard(Board* board)
{
    memset(board->m, 0, NUM_COLS * NUM_ROWS * sizeof(int));
}