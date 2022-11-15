#include "cylinder.h"
#include <stdio.h>
#include <stdlib.h>

float* generateVertices(float radius, float height, int sectors, int stacks, int* len)
{
    // La funcio retorna l'array de vertex i normals, i retorna la longitud de l'array pel parametre len
    // 
    // Un cilindre té 2 tapes i segments rectangulars allargats verticalment entre les dues tapes.
    // El nombre de segments rectangulars depen de la variable stacks, que permet crear vertex intermitjos.
    // Numero de vertex total (notem que si un vertex te un vector normal diferent, es guarda per separat):
    // (sectors) vertex per cara
    // 2 cares => (2 * sectors)
    // 
    // (stacks) segments rectangulars entre tapes
    // 4 vertex per segment rectangular, amb vertex intemitjos compartits entre segments
    // (stacks * 2 + 2) vertex per segment vertical complet
    int numVertex = 2 * (stacks + sectors) + 2;

    // A mes cada vertex va amb la seva normal (6 floats per vertex)
    float* vertexArr = malloc(numVertex * 6 * sizeof(float));

}