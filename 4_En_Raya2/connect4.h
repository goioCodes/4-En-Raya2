#define NUM_ROWS 6
#define NUM_COLS 7

typedef enum {
    EMPTY,
    TOKEN1,
    TOKEN2
} Token;

typedef struct {
    Token m[NUM_ROWS][NUM_COLS];
} Board;


void connect4Main();
void printBoard(Board* board);
void initializeBoard(Board* board);
void printHBar();
int placeToken(Board* board, int user, int col);
void getUserInput(int* var);
bool boardIsFull(Board* board, int turn);
bool checkWin(Board* board, int row, int col);
bool checkWinDirection(Board* board, int row, int col, int* direction);