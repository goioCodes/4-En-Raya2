#include "miniMax.h"
#include "board.h"

#include <float.h>
#include <stdlib.h>
#include <stdio.h>

bool hardMode; // Variable global només per no haver de passar per paràmetre durant tot l'arbre

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


    // Comprovem si es node fulla (3 situacions), en aquest cas cridem la funcio heuristica (alguns nodes fulla obtenen la puntuació
    // directament, ja que la funció heurística repetiria la mateixa comprovació que s'ha fet per saber si és node fulla)
    if (checkWin(&node->board, row, col)) // Jugada anterior ha guanyat
    {
        node->numChilds = 0;
        node->value = - 50 * NUM_COLS * NUM_ROWS + node->board.turnCount; // Puntuació mínima doncs el jugador actual ha perdut.
    }                                                                     // Sumem el nombre de torns per afavorir perdre el més tard possible.
    else if (boardIsFull(&node->board)) // Empat
    {
        node->numChilds = 0;
        node->value = 0;
    }
    else if (depth == 0) // Valoració general
    {
        node->numChilds = 0;
        node->value = heuristicFunction(node); // Important! La funció heurística calcularà el valor sempre des de la perspectiva
                                               // del jugador que ha de tirar a continuació.
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
    // Aquesta funció genera la següent tanda de jugades i retorna la puntuació màxima entre totes les possibilitats.
    // Recordem que les puntuacions de cada node estan calculades des del punt de vista de qui juga en aquell moment.
    // Aparentment aquesta variació de l'algoritme minimax es diu negamax.
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
        // Com que ho mirem tot des del punt de vista del jugador actual, les puntuacions més beneficioses pel jugador contrari
        // serán les més negatives per a mi. Per tant el jugador contrari vol escollir la jugada amb valoració més negativa des del punt
        // de vista del jugador actual. Així doncs negarem la valoració dels fills, i escollim la màxima.
        // 
        // Dit d'una altra forma, si x,y son les valoracions dels fills, tenim que
        // min(x, y) = -max(-x, -y)
        // i el valor que ens estem quedant como a millor és max(-x, -y)
        // Quan volguem fer el màxim d'aquests valors al nivell superior, ens trobarem
        // max(-max(-x, -y), -max(-z, -w)) = max(min(x, y), min(z, w))
        // Per tant es el mateix que fer minimax però sense haver de comprovar en el torn de qui ens trobem, ni
        // haver d'escriure una funció heurística que tingui en compte a qui li toca.
        alpha = max(alpha, bestScore);
        if (alpha >= beta)
        {
            parent->numChilds = i + 1;
            break;
        }
        // Implementar alpha-beta pruning d'aquesta forma és una mica més confús. Suposem que partim des d'un node de la màquina on
        // hem acabat d'explorar la primera branca i el node fill té un valor de 2 (per l'humà). Fent la negació,
        // això és una puntuació de -2 per la màquina. A partir d'ara aquesta es la nostra puntuació mínima garantitzada de la màquina
        // alpha.
        // Passem ara a explorar el segon node, al que li comunicarem que pot parar d'explorar si la seva puntuació supera 2, doncs
        // fent la negació, aquesta puntuació és pitjor que -2 i per tant la jugada queda descartada en favor de la ja explorada.
        // Per fer-ho, passem el valor de -alpha (2) com a cota superior al node fill, i mirem que el valor del node mai superi
        // aquest valor. Això és el mateix que dir que -alpha = beta2 pel node fill i deixar de buscar si alpha2 >= beta2. Així
        // doncs beta2 és la puntuació màxima garantitzada de l'humà (és a dir, qualsevol puntuació superior serà descartada pel contrari).
        // Si baixem un nivell més, el node pare va actualitzant alpha2 amb les puntuacions màximes, que tornen a estar invertides,
        // i ara s'ha d'indicar als nodes fill que la seva puntuació no pot superar -alpha2.
        // Per veure que passa el mateix amb beta, i que li hem de passar -beta = alpha2 al node fill, fixem-nos que la puntuació
        // màxima garantitzada de la màquina beta, quan canviem el signe de les valoracions, passa a ser la putuació mínima garantitzada
        // per l'humà alpha2 en un node superior, i per tant qualsevol puntuació inferior no afectarà al resultat del node superior.
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
    // Perdre la partida tindrà un valor menys negatiu quant més tard es perdi. De forma similar, guanyar serà
    // més positiu quant més aviat es guanyi. Diferenciem els nodes que tenen un estat final de la partida
    // donant-lis un valor molt extrem, com és el cas de 50 * NUM_COLS * NUM_ROWS. Així no hi ha possibilitat de que
    // aquestes puntuacions es solapin amb les valoracions heurístiques de més endevant.

    // Comprovem si la partida es pot guanyar en el pròxim moviment. Llavors la puntuació serà màxima
    // penalitzada pel temps que es trigui en guanyar.

    for (int j = 0; j < NUM_COLS; j++)
    {
        if (moveWouldWin(&node->board, j))
        {
            // Volem escollir el moviment que ens permeti guanyar més ràpid. Per tant restem el número de torns.
            return 50 * NUM_COLS * NUM_ROWS - node->board.turnCount;
        }
    }

    // Mirem ara les posicions on es pot perdre. Per fer-ho avancem el torn de forma que la peça col·locada sigui la
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
    else if (count == 1) // Si només existeix una, col·locar peça per bloquejar-la pot provocar que es perdi de totes formes
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