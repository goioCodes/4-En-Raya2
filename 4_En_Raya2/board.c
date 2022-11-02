#include "board.h"
#include "colors.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static void printHBar();

int placeToken(Board* board, Token user, int col)
{
    for (int i = NUM_ROWS - 1; i >= 0; i--)
    {
        if (board->m[i][col] == EMPTY)
        {
            board->m[i][col] = user;
            return i;
        }
    }
    return -1;
}

bool checkWin(Board* board, int row, int col)
{
    // La funcio comprova si l'ultim moviment ha guanyat la partida. Nomes es comproven les 4 direccions (2 ortogonals, 2 diagonals)
    // al voltant de l'ultima fitxa colocada, i fins a 3 caselles en cada direccio (maxim de 7 caselles)

    return checkWinDirection(board, row, col, (int[2]) { 1, 0 }) || checkWinDirection(board, row, col, (int[2]) { 0, 1 }) ||
        checkWinDirection(board, row, col, (int[2]) { 1, 1 }) || checkWinDirection(board, row, col, (int[2]) { -1, 1 });
}

bool checkWinDirection(Board* board, int row, int col, int* direction)
{
    // direction es un vector de 2 components que indica en quina direccio ens desplaçarem per trobar 4 en linea
    // direction pot ser (1,0), (0,1), (1,1), (-1,1), (1, -1) o (-1, -1)
    // Recorden que la coordenada y es mesura desde el costat superior cap abaix.

    // En realitat només necesitem 4 direccions ja que primer tirem endarrere (direcció inversa al parámetre) fins a 3 caselles
    // o bé fins al límit del tauler, i després avancem cap endevant contant quantes fitxes en linia hi ha fins a un maxim de
    // 6 caselles o be fins a trobar un limit del tauler.
    // Per tant les direccions inverses comproven el mateix tros de tauler.
    Token token = board->m[row][col];

    int verticalBackLimit = direction[1] == 1 ? row : direction[1] == 0 ? max(NUM_COLS, NUM_ROWS) : NUM_ROWS - row - 1;
    int horizontalBackLimit = direction[0] == 1 ? col : direction[0] == 0 ? max(NUM_COLS, NUM_ROWS) : NUM_COLS - col - 1;
    int backLimit = min(min(verticalBackLimit, horizontalBackLimit), 3);
    int startRow = row - direction[1] * backLimit;
    int startCol = col - direction[0] * backLimit;

    int verticalFrontLimit = direction[1] == 1 ? NUM_ROWS - row - 1 : direction[1] == 0 ? max(NUM_COLS, NUM_ROWS) : row;
    int horizontalFrontLimit = direction[0] == 1 ? NUM_COLS - col - 1 : direction[0] == 0 ? max(NUM_COLS, NUM_ROWS) : col;
    int frontLimit = min(min(verticalFrontLimit, horizontalFrontLimit), 3);

    int count = 0;
    for (int k = 0; k <= backLimit + frontLimit; k++)
    {
        if (board->m[startRow + direction[1] * k][startCol + direction[0] * k] == token)
        {
            count += 1;
        }
        else
        {
            count = 0;
        }
        if (count >= 4)
        {
            return true;
        }
    }
    return false;
}

bool boardIsFull(Board* board, int turn)
{
    // sizeof(board->m) retorna el numero de bytes, entre 4 per obtenir el numero d'ints.
    return sizeof(board->m) / 4 == turn;
}


void printBoard(Board* board)
{
    Token token;
    printHBar();
    for (int i = 0; i < NUM_ROWS; i++)
    {
        printf("|");
        for (int j = 0; j < NUM_COLS; j++)
        {
            token = board->m[i][j];
            printf("%s %d " ANSI_COLOR_RESET "|", COLOR(token), token);
        }
        printf("\n");
        printHBar();
    }
    printf("|");
    for (int j = 0; j < NUM_COLS; j++)
    {
        printf(ANSI_BACKGROUND_BLUE " %d " ANSI_COLOR_RESET "|", j + 1);
    }
    printf("\n");
}

static void printHBar()
{
    for (int j = 0; j < NUM_COLS; j++)
    {
        printf("____");
    }
    printf("\n");
}

void initializeBoard(Board* board)
{
    memset(board, 0, NUM_COLS * NUM_ROWS * sizeof(int));
}