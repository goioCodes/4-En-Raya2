#include "miniMax.h"
#include "board.h"

#include <float.h>
#include <stdlib.h>
#include <stdio.h>

int miniMaxGetPlay(Board* board)
{
    Node* root = createRootNodeFromBoard(board);
    int bestPlay;
    if (nextMiniMaxLevel(root, 0, &bestPlay) == DBL_MAX)
    {
        printf("Error minimax.\n");
        eraseTree(root);
        return -1;
    }
    //traverseTree(root->childs[2]->childs[4], 2);
    eraseTree(root);
    return bestPlay;
}

Node* createRootNodeFromBoard(Board* board)
{
    Node* root = malloc(sizeof(Node));
    if (!root)
    {
        printf("Error de memoria.\n");
        return NULL;
    }

    root->board = *board;
    root->numChilds = getNumChilds(root);
    root->childCols = getFreeColumnsArray(board, root->numChilds);
    root->childs = malloc(root->numChilds * sizeof(Node*)); // Pel node arrel sempre hi haura alguna opcio o ja s'haria acabat la partida

    return root;
}

int getNumChilds(Node* node)
{
    return getFreeColumnsCount(&node->board);
}

Node* createNode(Node* parent, int col, int depth)
{
    Node* node = malloc(sizeof(Node));
    if (!node)
    {
        printf("Error de memoria.\n");
        return NULL;
    }
    node->board = parent->board;
    int row = placeToken(&node->board, col);
    // Aqui acaba la creacio del nou node.


    // Comprovem si es node fulla (3 situacions), en aquest cas cridem la funcio heuristica (alguns nodes fulla obtenen la puntuaci�
    // directament, ja que la funci� heur�stica repetiria la mateixa comprovaci� que s'ha fet per saber si �s node fulla)
    if (checkWin(&node->board, row, col)) // Jugada anterior ha guanyat
    {
        node->numChilds = 0;
        node->value = - 4 * NUM_COLS * NUM_ROWS + node->board.turnCount; // Puntuaci� m�nima doncs el jugador actual ha perdut
    }
    else if (boardIsFull(&node->board)) // Empat
    {
        node->numChilds = 0;
        node->value = 0;
    }
    else if (depth == MAXDEPTH) // Valoraci� general
    {
        node->numChilds = 0;
        node->value = heuristicFunction(node); // Important! La funci� heur�stica calcular� el valor sempre des de la perspectiva
                                               // del jugador que ha de tirar a continuaci�.
    }
    else
    {
        node->numChilds = getFreeColumnsCount(&node->board);
        node->childCols = getFreeColumnsArray(&node->board, node->numChilds);
        node->childs = malloc(node->numChilds * sizeof(Node*));
        if (!node->childs || !node->childCols)
        {
            printf("Error de memoria creant fills al nivell %d.\n", depth);
            return NULL;
        }

        node->value = nextMiniMaxLevel(node, depth, &node->bestPlay);
        if (node->value == DBL_MAX)
        {
            return NULL;
        }
    }
    if (node->numChilds == 0) // Node fulla
    {
        node->childs = NULL;
        node->childCols = NULL;
    }
    
    return node;
}

double nextMiniMaxLevel(Node* parent, int depth, int* bestPlay)
{
    // Aquesta funci� genera la seg�ent tanda de jugadas i retorna la puntuaci� m�xima entre totes les possibilitats.
    // Recordem que les puntuacions de cada node estan calculades des del punt de vista de qui juga en aquell moment.
    double bestScore = -4 * NUM_COLS * NUM_ROWS - 1;
    for (int i = 0; i < parent->numChilds; i++)
    {
        parent->childs[i] = createNode(parent, parent->childCols[i], depth + 1);
        if (!parent->childs[i])
        {
            printf("Error creant arbre al nivell %d.\n", depth);
            return DBL_MAX;
        }

        if (-parent->childs[i]->value > bestScore)
        {
            bestScore = -parent->childs[i]->value;
            *bestPlay = parent->childCols[i];
        }
        // Com que ho mirem tot des del punt de vista del jugador actual, les puntuacions m�s beneficioses pel jugador contrari
        // ser�n les m�s negatives per a mi. Per tant el jugador contrari vol escollir la jugada amb valoraci� m�s negativa des del punt
        // de vista del jugador actual. Aix� doncs negarem la valoraci� dels fills, i escollim la m�xima.
        // 
        // Dit d'una altra forma, si x,y son les valoracions dels fills, tenim que
        // min(x, y) = -max(-x, -y)
        // i el valor que ens estem quedant como a millor �s max(-x, -y)
        // Quan volguem fer el m�xim d'aquests valors al nivell superior, ens trobarem
        // max(-max(-x, -y), -max(-z, -w)) = max(min(x, y), min(z, w))
        // Per tant es el mateix que fer minimax per� sense haver de comprovar en el torn de qui ens trobem, ni
        // haver d'escriure una funci� heur�stica que tingui en compte a qui li toca.
    }

    return bestScore;
}

void traverseTree(Node* node, int depth)
{
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    printf("%d\n", (int)node->value);
    for (int i = 0; i < node->numChilds; i++) {
        traverseTree(node->childs[i], depth + 1);
    }
}

void eraseTree(Node* parent)
{
    for (int i = 0; i < parent->numChilds; i++)
    {
        eraseTree(parent->childs[i]);
    }
    free(parent->childs);
    free(parent->childCols);
    free(parent);
}

double heuristicFunction(Node* node)
{
    // Ja hem definit que un node on la partida s'ha perdut t� puntuaci� -NUM_COLS*NUM_ROWS. Perdre la partida en el torn
    // seg�ent tindr� un valor menys negatiu quant m�s tard es perdi. Multipliquem el m�nim per 2 per deixar espai als
    // valors que s'assignen quan no s'ha arribat a cap posici� final. Aix� la distribuci� (no exhaustiva) de valors queda:
    // [- 2 NUM_ROWS*NUM_COLS, - NUM_ROWS*NUM_COLS]: Partida perduda en un dels torns seg�ents.
    // 
    // Abans d'aix� per�, comprovem si la partida es pot guanyar en el pr�xim moviment. Llavors la puntuaci� ser�
    // NUM_COLS*NUM_ROWS penalitzada pel temps que es trigui en guanyar.

    for (int j = 0; j < NUM_COLS; j++)
    {
        if (moveWouldWin(&node->board, j))
        {
            // Volem escollir el moviment que ens permeti guanyar m�s r�pid. Per tant restem el n�mero de torns.
            // Per assegurar-nos que la puntuaci� es positiva i major a la resa de casos (guanyar sempre �s preferible),
            // sumem el nombre de torns m�xims per 2
            return 4 * NUM_COLS * NUM_ROWS - node->board.turnCount;
        }
    }

    node->board.turnCount++;
    int count = 0;
    int dangerCol;
    for (int j = 0; j < NUM_COLS; j++)
    {
        if (moveWouldWin(&node->board, j))
        {
            dangerCol = j;
            count++;
        }
    }
    node->board.turnCount--;
    if (count >= 2)
    {
        return -4 * NUM_COLS * NUM_ROWS + node->board.turnCount + 1;
    }
    else if (count == 1)
    {
        Board temp = node->board;
        placeToken(&temp, dangerCol);
        if (moveWouldWin(&temp, dangerCol))
        {
            return -4 * NUM_COLS * NUM_ROWS + node->board.turnCount + 1;
        }
    }

    // Mirem ara les posicions on es pot perdre. Per fer-ho avancem el torn de forma que la pe�a col�locada sigui la
    // del contrari

    // getWeightedSum pot retornar, en casos extrems, valors en l'interval [-6 * NUM_ROWS, NUM_ROWS]. Aquest interval ser�
    // disjunt amb l'interval dels moviments guanyadors sempre i quan NUM_COLS >= 2, per� aquesta precauci� no �s necess�ria.
    return getWeightedSum(&node->board);
}