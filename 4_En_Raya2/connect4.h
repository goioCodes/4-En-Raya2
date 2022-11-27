#ifndef __CONNECT4_H_
#define __CONNECT4_H_
#include "board.h"
#include <stdbool.h>

int mainConsole(Token firstPlayer, bool twoplayers);
void getUserInput(int* var, int minVal, int maxVal, bool decrement);
void clearScr();
#endif