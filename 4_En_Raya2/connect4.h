#define NUM_ROWS 6
#define NUM_COLS 7

typedef struct {
    int m[NUM_ROWS][NUM_COLS];
} Board;


void connect4Main();
void printBoard(Board* board);
void initializeBoard(Board* board);
void printHBar();