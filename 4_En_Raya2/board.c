#include "board.h"
#include "colors.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define lastPlayer(board) (board->turnCount % 2 == 0) + 1
#define currentPlayer(board) board->turnCount % 2 + 1

static void printHBar();

Token getLastPlayer(Board* board)
{
    if (board->firstPlayer == PLAYER2) return currentPlayer(board);
    return lastPlayer(board);
}

Token getCurrentPlayer(Board* board)
{
    if (board->firstPlayer == PLAYER2) return lastPlayer(board);
    return currentPlayer(board);
}

int placeToken(Board* board, int col)
{
    Token user = getCurrentPlayer(board);
    for (int i = NUM_ROWS - 1; i >= 0; i--)
    {
        if (board->m[i][col] == EMPTY)
        {
            board->m[i][col] = user;
            board->turnCount++;
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
            count++;
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

Token checkWinFull(Board* board)
{
    // Aquesta funcio comprova tot el tauler i retorna el jugador que ha guanyat si n'hi ha cap, 0 en cas contrari.
    // Utilitza un algoritme diferent de les funcions d'adalt per optimitzar millor aquest cas.
    Token currentToken;
    int count;

    // Verticals. Comprovem totes les columnes.
    for (int j = 0; j < NUM_COLS; j++)
    {
        currentToken = EMPTY;
        count = 0;
        for (int i = 0; i < NUM_ROWS; i++)
        {
            if (board->m[i][j] == currentToken)
            {
                count++;
            }
            else
            {
                currentToken = board->m[i][j];
                count = 1;
            }

            if (count >= 4 && currentToken != EMPTY)
            {
                return currentToken;
            }
        }
    }

    // Horitzontals
    for (int i = 0; i < NUM_ROWS; i++)
    {
        currentToken = EMPTY;
        count = 0;
        for (int j = 0; j < NUM_COLS; j++)
        {
            if (board->m[i][j] == currentToken)
            {
                count++;
            }
            else
            {
                currentToken = board->m[i][j];
                count = 1;
            }

            if (count >= 4 && currentToken != EMPTY)
            {
                return currentToken;
            }
        }
    }

    // Diagonals \ i / que comencen al costat esquerre
    for (int i = 0; i < NUM_ROWS; i++)
    {
        //Diagonals \ 
        currentToken = EMPTY;
        count = 0;
        for (int k = 0; k < min(NUM_ROWS - i, NUM_COLS); k++)
        {
            if (board->m[i + k][k] == currentToken)
            {
                count++;
            }
            else
            {
                currentToken = board->m[i + k][k];
                count = 1;
            }
            if (count >= 4 && currentToken != EMPTY)
            {
                return currentToken;
            }
        }

        // Diagonals /
        currentToken = EMPTY;
        count = 0;
        for (int k = 0; k < min(i + 1, NUM_COLS); k++)
        {
            if (board->m[i - k][k] == currentToken)
            {
                count++;
            }
            else
            {
                currentToken = board->m[i - k][k];
                count = 1;
            }

            if (count >= 4 && currentToken != EMPTY)
            {
                return currentToken;
            }
        }
    }
    // Ens saltem la primera diagonal que ja hem comprovat
    for (int j = 1; j < NUM_COLS; j++)
    {
        //Diagonals \ que comencen al limit superior
        currentToken = EMPTY;
        count = 0;
        for (int k = 0; k < min(NUM_ROWS, NUM_COLS - j); k++)
        {
            if (board->m[k][j + k] == currentToken)
            {
                count++;
            }
            else
            {
                currentToken = board->m[k][j + k];
                count = 1;
            }
            if (count >= 4 && currentToken != EMPTY)
            {
                return currentToken;
            }
        }

        // Diagonals / que comencen al limit inferior
        currentToken = EMPTY;
        count = 0;
        for (int k = 0; k < min(NUM_ROWS, NUM_COLS - j); k++)
        {
            if (board->m[NUM_ROWS - 1 - k][j + k] == currentToken)
            {
                count++;
            }
            else
            {
                currentToken = board->m[NUM_ROWS - 1 - k][j + k];
                count = 1;
            }

            if (count >= 4 && currentToken != EMPTY)
            {
                return currentToken;
            }
        }
    }

    return EMPTY;
}

bool boardIsFull(Board* board)
{
    return board->turnCount == NUM_COLS * NUM_ROWS;
}

int getFreeColumnsCount(Board* board)
{
    int count = 0;
    for (int j = 0; j < NUM_COLS; j++)
    {
        if (board->m[0][j] == EMPTY)
        {
            count++;
        }
    }
    return count;
}

int* getFreeColumnsArray(Board* board, int freeCount)
{
    if (!freeCount) return NULL;
    int* res = malloc(freeCount * sizeof(int));
    if (!res)
    {
        printf("Error de memoria.\n");
        return NULL;
    }

    for (int j = 0, i = 0; j < NUM_COLS; j++)
    {
        if (board->m[0][j] == EMPTY)
        {
            res[i++] = j;
        }
    }

    return res;
}

/*
int* getFreeColumnsArray(Board* board, int* freeCount)
{
    int buff[NUM_COLS];
    int count = 0;
    for (int j = 0; j < NUM_COLS; j++) // Per cada columna, comprova si està buida i en aquest cas guarda-la al següent lloc lliure
    {
        if (board->m[0][j] == EMPTY)
        {
            buff[count++] = j;
        }
    }
    *freeCount = count;

    if (!count) return NULL;
    int* res = malloc(count * sizeof(int));
    if (!res)
    {
        printf("Error de memoria.\n");
        *freeCount = -1;
        return NULL;
    }

    for (int i = 0; i < count; i++)
    {
        res[i] = buff[i];
    }

    return res;
}
*/

int getWeightedSum(Board* board)
{
    Token current = getCurrentPlayer(board);
    int weight;
    int sumCurr = 0;
    int sumOpo = 0;
    for (int j = 0; j < NUM_COLS; j++)
    {
        if (NUM_COLS % 2 == 0)
        {
            if (j == NUM_COLS / 2 || j == NUM_COLS / 2 + 1)
            {
                weight = 2;
            }
            else if (j == NUM_COLS / 2 - 1 || j == NUM_COLS / 2 + 2)
            {
                weight = 1;
            }
            else
            {
                continue;
            }
        }
        else
        {
            if (j == NUM_COLS / 2)
            {
                weight = 2;
            }
            else if (j == NUM_COLS / 2 + 1 || j == NUM_COLS / 2 - 1)
            {
                weight = 1;
            }
            else
            {
                continue;
            }
        }

        for (int i = 0; i < NUM_ROWS; i++)
        {
            if (board->m[i][j] == EMPTY)
            {
                continue;
            }
            if (board->m[i][j] == current)
            {
                sumCurr += weight;
            }
            else
            {
                sumOpo += weight;
            }
        }
    }

    return sumCurr - sumOpo;
}

bool moveWouldWin(Board* board, int col)
{
    Board board2 = *board;
    int row = placeToken(&board2, col);
    if (row != -1)
    {
        return checkWin(&board2, row, col);
    }
    return false;
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

void initializeBoard(Board* board, Token firstPlayer)
{
    board->firstPlayer = firstPlayer;
    board->turnCount = 0;
    memset(board->m, 0, NUM_COLS * NUM_ROWS * sizeof(int));
}