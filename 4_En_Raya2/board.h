#ifndef __BOARD_H_
#define __BOARD_H_
#include <stdbool.h>

#define NUM_ROWS 6
#define NUM_COLS 7

typedef enum {
    EMPTY,
    PLAYER1,
    PLAYER2
} Token;

typedef struct {
    Token firstPlayer;
    int turnCount;
    Token m[NUM_ROWS][NUM_COLS];
} Board;

void printBoard(Board* board);

Token getCurrentPlayer(Board* board);
Token getLastPlayer(Board* board);

void initializeBoard(Board* board, Token firstPlayer);
int placeToken(Board* board, int col);

bool boardIsFull(Board* board);
bool checkWin(Board* board, int row, int col);
int countLinesDirection(Board* board, int row, int col, int* direction, bool checkBack, bool breakOnColorChange, bool resetOnEmpty, bool breakOnWall, Token* detectedPlayer);

int getFreeColumnsCount(Board* board);
int* getFreeColumnsArray(Board* board, int freeCount);

int getWeightedSum(Board* board);
int allPossibleLinesSum(Board* board);
bool moveWouldWin(Board* board, int col);
#endif