#include "connect4.h"
#include "colors.h"
#include "board.h"
#include "miniMax.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

int mainConsole(Token firstPlayer, bool twoplayers, int maxdepth, bool difficulty)
{
    Board board;
    initializeBoard(&board, firstPlayer);
    printBoard(&board);

    while (true)
    {
        Token currentPlayer = getCurrentPlayer(&board);
        // Torn d'un usuari
        int col, row;
        if (!twoplayers && currentPlayer == PLAYER2)
        {
            col = miniMaxGetPlay(&board, maxdepth, difficulty);
            if (col == -1)
                return -1;
            row = placeToken(&board, col);
        }
        else
        {
            while (true)
            {
                // Agafem la columna on l'usuari vol colocar
                printf("\nJugador %s %d " ANSI_COLOR_RESET ", columna?\n", COLOR(currentPlayer), currentPlayer);
                getUserInput(&col, 1, NUM_COLS, true);
                // Coloquem la fitxa i mirem que no estigui a una columna plena, si no es torna a repetir el procés
                row = placeToken(&board, col);
                if (row != -1)
                {
                    break;
                }
                else
                {
                    printf("Columna plena!\n");
                }
            }
        }
        clearScr();
        printBoard(&board);
        if (checkWin(&board, row, col))
        {
            printf("El jugador %d ha guanyat!\n", currentPlayer);
            return 0;
        }
        if (boardIsFull(&board))
        {
            printf("Empat! Tothom perd.\n");
            return 0;
        }
    }
}

void getUserInput(int* var, int minVal, int maxVal, bool decrement)
{
    char inputBuffer[64];
    while (true) {
        fgets(inputBuffer, 64, stdin);
#ifdef _MSC_VER
        if (sscanf_s(inputBuffer, "%d", var) == 1 && *var >= minVal && *var <= maxVal) // sscanf_s pel compilador MSVC
#else
        if (sscanf(inputBuffer, "%d", var) == 1 && *var >= minVal && *var <= maxVal)
#endif
        {
            if (decrement) (*var)--;
            break;
        }
        else {
            printf("Introdueix un numero valid\n");
        }
    }
}

void clearScr()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
