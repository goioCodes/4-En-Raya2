#ifndef __BOARD_H_
#define __BOARD_H_
#include <stdbool.h>

#define NUM_ROWS 8
#define NUM_COLS 8

typedef enum {
    EMPTY,
    PLAYER1,
    PLAYER2
} Token;

typedef struct {
    int turnCount;
    Token m[NUM_ROWS][NUM_COLS];
} Board;

void printBoard(Board* board);

Token getCurrentPlayer(Board* board);
Token getLastPlayer(Board* board);

void initializeBoard(Board* board);
int placeToken(Board* board, int col);

bool boardIsFull(Board* board);
bool checkWin(Board* board, int row, int col);
bool checkWinDirection(Board* board, int row, int col, int* direction);
Token checkWinFull(Board* board);

int getFreeColumnsCount(Board* board);
int* getFreeColumnsArray(Board* board, int freeCount);

int getWeightedSum(Board* board);
bool moveWouldWin(Board* board, int col);
#endif