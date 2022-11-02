#ifndef __COLORS_H_
#define __COLORS_H_
#include "board.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_BACKGROUND_RED "\x1b[41m"
#define ANSI_BACKGROUND_YELLOW "\x1b[43m"
#define ANSI_BACKGROUND_BLUE "\x1b[44m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define COLOR(token) token == EMPTY ? "" : token == PLAYER1 ? ANSI_BACKGROUND_RED : ANSI_BACKGROUND_YELLOW
#endif