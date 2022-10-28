#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "connect4.h"

void connect4Main()
{
    Board board;
    int currentPlayer = 1;
    int turnCount = 1;
    initializeBoard(&board);
    printBoard(&board);

    while (true)
    {
        // Torn d'un usuari
        int col, row;
        while (true)
        {
            // Agafem la columna on l'usuari vol colocar
            printf("Columna?\n");
            getUserInput(&col);
            printf("Intentant colocar a %d\n", col);
            // Coloquem la fitxa i mirem que no estigui a una columna plena, si no es torna a repetir el proces
            row = placeToken(&board, currentPlayer, col);
            if (row != -1)
            {
                break;
            }
            else
            {
                printf("Columna plena!\n");
            }
        }
        printBoard(&board);
        if (checkWin(&board, row, col))
        {
            printf("El jugador %d ha guanyat!\n", currentPlayer);
            break;
        }
        if (boardIsFull(&board, turnCount))
        {
            printf("Empat! Tothom perd.\n");
            break;
        }
        turnCount++;
        currentPlayer = currentPlayer == 1 ? 2 : 1;
    }
}

int placeToken(Board* board, int user, int col)
{
    for (int i = NUM_ROWS - 1; i >= 0; i--)
    {
        if (board->m[i][col] == EMPTY)
        {
            board->m[i][col] = user == 1 ? TOKEN1 : TOKEN2;
            return i;
        }
    }
    return -1;
}

bool checkWin(Board* board, int row, int col)
{
    // La funcio comprova si l'ultim moviment ha guanyat la partida. Nomes es comproven les 4 direccions (2 ortogonals, 2 diagonals)
    // al voltant de l'ultima fitxa colocada, i fins a 4 caselles en cada direccio (maxim de 8 caselles)

    return checkWinDirection(board, row, col, (int[2]) { 1, 0 }) || checkWinDirection(board, row, col, (int[2]) { 0, 1 }) ||
        checkWinDirection(board, row, col, (int[2]) { 1, 1 }) || checkWinDirection(board, row, col, (int[2]) { -1, 1 });
}

bool checkWinDirection(Board* board, int row, int col, int* direction)
{
    // direction es un vector de 2 components que indica en quina direccio ens desplaçarem per trobar 4 en linea
    // direction pot ser (1,0), (0,1), (1,1), (-1,1), (1, -1) o (-1, -1)
    // Recorden que la coordenada y es mesura desde el costat superior cap abaix.
    Token token = board->m[row][col];

    int verticalBackLimit = direction[1] == 1 ? row : direction[1] == 0 ? max(NUM_COLS, NUM_ROWS) : NUM_ROWS - row - 1;
    int horizontalBackLimit = direction[0] == 1 ? col : direction[0] == 0 ? max(NUM_COLS, NUM_ROWS) : NUM_COLS - col - 1;
    int backLimit = min(min(verticalBackLimit, horizontalBackLimit), 3);
    int startRow = row - direction[0] * backLimit;
    int startCol = col - direction[0] * backLimit;

    int verticalFrontLimit = direction[1] == 1 ? NUM_ROWS - row - 1 : direction[1] == 0 ? max(NUM_COLS, NUM_ROWS) : row;
    int horizontalFrontLimit = direction[0] == 1 ? NUM_COLS - col - 1 : direction[0] == 0 ? max(NUM_COLS, NUM_ROWS) : col;
    int frontLimit = min(min(verticalFrontLimit, horizontalFrontLimit), 3);

    int count = 0;
    for (int k = 0; k <= backLimit + frontLimit; k++)
    {
        if ((startRow + direction[1] * k >= NUM_ROWS) || (startCol + direction[0] * k >= NUM_COLS))
        {
            printf("ERROR ACCEDIENDO FUERA DE ARRAY, POSICION [%d][%d]\n", startRow + direction[1] * k, startCol + direction[0] * k);
        }
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
    return sizeof(board->m)/4 == turn;
}

void getUserInput(int* var)
{
    char inputBuffer[64];
    while (1) {
        fgets(inputBuffer, 64, stdin);
        if (sscanf_s(inputBuffer, "%d", var) == 1 && *var >= 1 && *var <= NUM_COLS) // sscanf_s pel compilador MSVC, es pot canviar per sscanf
        {
            (*var)--;
            break;
        }
        else {
            printf("Introdueix un numero valid\n");
        }
    }
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