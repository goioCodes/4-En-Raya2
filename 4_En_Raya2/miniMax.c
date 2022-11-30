#include "miniMax.h"
#include "board.h"

#include <float.h>
#include <stdlib.h>
#include <stdio.h>

bool hardMode; // Variable global nom�s per no haver de passar per par�metre durant tot l'arbre

int miniMaxGetPlay(Board* board, int maxdepth, bool difficulty)
{
    hardMode = difficulty;
    Node* root = createRootNodeFromBoard(board);
    int bestPlay;
    if (nextMiniMaxLevel(root, maxdepth, -DBL_MAX, DBL_MAX, &bestPlay) == DBL_MAX)
    {
        printf("Error minimax.\n");
        eraseTree(root);
        return -1;
    }
    //traverseTree(root, maxdepth);
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
    root->childs = malloc(root->numChilds * sizeof(Node*)); // Pel node arrel sempre hi haura alguna opcio o ja s'hauria acabat la partida

    return root;
}

int getNumChilds(Node* node)
{
    return getFreeColumnsCount(&node->board);
}

Node* createNode(Node* parent, int col, double alpha, double beta, int depth)
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
        node->value = - 50 * NUM_COLS * NUM_ROWS + node->board.turnCount; // Puntuaci� m�nima doncs el jugador actual ha perdut.
    }                                                                     // Sumem el nombre de torns per afavorir perdre el m�s tard possible.
    else if (boardIsFull(&node->board)) // Empat
    {
        node->numChilds = 0;
        node->value = 0;
    }
    else if (depth == 0) // Valoraci� general
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

        node->value = nextMiniMaxLevel(node, depth, alpha, beta, &node->bestPlay);
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

double nextMiniMaxLevel(Node* parent, int depth, double alpha, double beta, int* bestPlay)
{
    // Aquesta funci� genera la seg�ent tanda de jugades i retorna la puntuaci� m�xima entre totes les possibilitats.
    // Recordem que les puntuacions de cada node estan calculades des del punt de vista de qui juga en aquell moment.
    // Aparentment aquesta variaci� de l'algoritme minimax es diu negamax.
    double bestScore = -50 * NUM_COLS * NUM_ROWS - 1;
    for (int i = 0; i < parent->numChilds; i++)
    {
        parent->childs[i] = createNode(parent, parent->childCols[i], -beta, -alpha, depth - 1);
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
        alpha = max(alpha, bestScore);
        if (alpha >= beta)
        {
            parent->numChilds = i + 1;
            break;
        }
        // Implementar alpha-beta pruning d'aquesta forma �s una mica m�s conf�s. Suposem que partim des d'un node de la m�quina on
        // hem acabat d'explorar la primera branca i el node fill t� un valor de 2 (per l'hum�). Fent la negaci�,
        // aix� �s una puntuaci� de -2 per la m�quina. A partir d'ara aquesta es la nostra puntuaci� m�nima garantitzada de la m�quina
        // alpha.
        // Passem ara a explorar el segon node, al que li comunicarem que pot parar d'explorar si la seva puntuaci� supera 2, doncs
        // fent la negaci�, aquesta puntuaci� �s pitjor que -2 i per tant la jugada queda descartada en favor de la ja explorada.
        // Per fer-ho, passem el valor de -alpha (2) com a cota superior al node fill, i mirem que el valor del node mai superi
        // aquest valor. Aix� �s el mateix que dir que -alpha = beta2 pel node fill i deixar de buscar si alpha2 >= beta2. Aix�
        // doncs beta2 �s la puntuaci� m�xima garantitzada de l'hum� (�s a dir, qualsevol puntuaci� superior ser� descartada pel contrari).
        // Si baixem un nivell m�s, el node pare va actualitzant alpha2 amb les puntuacions m�ximes, que tornen a estar invertides,
        // i ara s'ha d'indicar als nodes fill que la seva puntuaci� no pot superar -alpha2.
        // Per veure que passa el mateix amb beta, i que li hem de passar -beta = alpha2 al node fill, fixem-nos que la puntuaci�
        // m�xima garantitzada de la m�quina beta, quan canviem el signe de les valoracions, passa a ser la putuaci� m�nima garantitzada
        // per l'hum� alpha2 en un node superior, i per tant qualsevol puntuaci� inferior no afectar� al resultat del node superior.
    }

    return bestScore;
}

void traverseTree(Node* node, int depth)
{
    for (int i = depth; i > 0; i--) {
        printf("    ");
    }
    printf("%d\n", (int)node->value);
    for (int i = 0; i < node->numChilds; i++) {
        traverseTree(node->childs[i], depth - 1);
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
    // Perdre la partida tindr� un valor menys negatiu quant m�s tard es perdi. De forma similar, guanyar ser�
    // m�s positiu quant m�s aviat es guanyi. Diferenciem els nodes que tenen un estat final de la partida
    // donant-lis un valor molt extrem, com �s el cas de 50 * NUM_COLS * NUM_ROWS. Aix� no hi ha possibilitat de que
    // aquestes puntuacions es solapin amb les valoracions heur�stiques de m�s endevant.

    // Comprovem si la partida es pot guanyar en el pr�xim moviment. Llavors la puntuaci� ser� m�xima
    // penalitzada pel temps que es trigui en guanyar.

    for (int j = 0; j < NUM_COLS; j++)
    {
        if (moveWouldWin(&node->board, j))
        {
            // Volem escollir el moviment que ens permeti guanyar m�s r�pid. Per tant restem el n�mero de torns.
            return 50 * NUM_COLS * NUM_ROWS - node->board.turnCount;
        }
    }

    // Mirem ara les posicions on es pot perdre. Per fer-ho avancem el torn de forma que la pe�a col�locada sigui la
    // del contrari
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
    if (count >= 2) // Si existeixen 2 columnes on el contrari pot fer 4 en ratlla, partida perduda.
    {
        return -50 * NUM_COLS * NUM_ROWS + node->board.turnCount + 1;
    }
    else if (count == 1) // Si nom�s existeix una, col�locar pe�a per bloquejar-la pot provocar que es perdi de totes formes
    {
        Board temp = node->board;
        placeToken(&temp, dangerCol);
        if (moveWouldWin(&temp, dangerCol))
        {
            return -50 * NUM_COLS * NUM_ROWS + node->board.turnCount + 1;
        }
    }

    if (hardMode) // Valoracions del tauler
        return getWeightedSum(&node->board) + allPossibleLinesSum(&node->board);
    else
        return getWeightedSum(&node->board);
}