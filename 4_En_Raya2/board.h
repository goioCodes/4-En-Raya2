#ifndef __BOARD_H_
#define __BOARD_H_
#include <stdbool.h>

#define NUM_ROWS 10
#define NUM_COLS 20

typedef enum {
    EMPTY,
    PLAYER1,
    PLAYER2
} Token;

typedef struct {
    Token m[NUM_ROWS][NUM_COLS];
} Board;

void printBoard(Board* board);
void initializeBoard(Board* board);
int placeToken(Board* board, Token user, int col);
bool boardIsFull(Board* board, int turn);
bool checkWin(Board* board, int row, int col);
bool checkWinDirection(Board* board, int row, int col, int* direction);
#endif