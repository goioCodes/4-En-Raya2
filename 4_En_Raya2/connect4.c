#include "connect4.h"
#include "colors.h"
#include "board.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

void connect4Main()
{
    Board board;
    Token currentPlayer = PLAYER1;
    initializeBoard(&board);
    printBoard(&board);

    while (true)
    {
        // Torn d'un usuari
        int col, row;
        while (true)
        {
            // Agafem la columna on l'usuari vol colocar
            printf("\nJugador %s %d " ANSI_COLOR_RESET ", columna?\n", COLOR(currentPlayer), currentPlayer);
            getUserInput(&col);
            // Coloquem la fitxa i mirem que no estigui a una columna plena, si no es torna a repetir el proces
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
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        printBoard(&board);
        if (checkWin(&board, row, col))
        {
            printf("El jugador %d ha guanyat!\n", currentPlayer);
            break;
        }
        if (boardIsFull(&board))
        {
            printf("Empat! Tothom perd.\n");
            break;
        }
        currentPlayer = currentPlayer == PLAYER1 ? PLAYER2 : PLAYER1;
    }
}

void getUserInput(int* var)
{
    char inputBuffer[64];
    while (1) {
        fgets(inputBuffer, 64, stdin);
#ifdef _MSC_VER
        if (sscanf_s(inputBuffer, "%d", var) == 1 && *var >= 1 && *var <= NUM_COLS) // sscanf_s pel compilador MSVC
#else
        if (sscanf(inputBuffer, "%d", var) == 1 && *var >= 1 && *var <= NUM_COLS)
#endif
        {
            (*var)--;
            break;
        }
        else {
            printf("Introdueix un numero valid\n");
        }
    }
}
