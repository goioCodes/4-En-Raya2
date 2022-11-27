#ifndef __MINIMAX_H_
#define __MINIMAX_H_
#include "board.h"

#define MAXDEPTH 6

typedef struct node{
    Board board;
    int numChilds;
    struct node** childs;
    int* childCols;
    double value;
    int bestPlay;
} Node;

int miniMaxGetPlay(Board* board);
Node* createRootNodeFromBoard(Board* board);
int getNumChilds(Node* node);
Node* createNode(Node* parent, int col, int depth);
double nextMiniMaxLevel(Node* parent, int depth, int* bestPlay);
void traverseTree(Node* node, int depth);
void eraseTree(Node* parent);
double heuristicFunction(Node* node);
#endif