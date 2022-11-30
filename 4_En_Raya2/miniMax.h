#ifndef __MINIMAX_H_
#define __MINIMAX_H_
#include "board.h"

typedef struct node{
    Board board;
    int numChilds;
    struct node** childs;
    int* childCols;
    double value;
    int bestPlay;
} Node;

int miniMaxGetPlay(Board* board, int maxdepth, bool difficulty);
Node* createRootNodeFromBoard(Board* board);
int getNumChilds(Node* node);

Node* createNode(Node* parent, int col, double alpha, double beta, int depth);
double nextMiniMaxLevel(Node* parent, int depth, double alpha, double beta, int* bestPlay);

void traverseTree(Node* node, int depth);
void eraseTree(Node* parent);

double heuristicFunction(Node* node);
#endif