#include "cylinderModel.h"

#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

Cylinder* generateCylinder(float radius, float height, int sectors)
{
    // La funcio retorna l'array de vertex i normals, i retorna la longitud de l'array pel parametre len
    // 
    // Un cilindre té 2 tapes (bases) i segments rectangulars allargats verticalment entre les dues tapes (laterals).
    // Numero de vertex per cada array (notem que si un vertex te un vector normal diferent, es guarda per separat
    // encara que sigui la mateixa posicio. Tambe repetim el primer i ultim vertex per
    // tancar el GL_TRIANGLE_FAN o el GL_TRIANGLE_STRIP):
    //                                                                                                      6/2
    // Base/Tapa del cilindre:                                                                              /|\
    // A mes de tenir tants vecters com sectors, repetim l'ultim i sumem el vertex central del cercle      5-1-3
    // (sectors + 1 + 1)                                                                                    \|/
    //                                                                                                       4
    int numVertBase = sectors + 2;
    // 4 vertex per segment rectangular, on els 2 primers es comparteixen amb el triangle anterior         2 4 6
    // i els 2 ultims es comparteixen amb el següent                                                       |\|\|...
    // (2 * sectors + 2)                                                                                   1 3 5
    int numVertSide = 2 * (sectors + 1);


    Cylinder* cyl = malloc(sizeof(Cylinder));
    if (!cyl)
    {
        printf("Error de memoria.\n");
        return NULL;
    }

    cyl->verticesBase = malloc(6 * numVertBase * sizeof(float));
    if (!cyl->verticesBase)
    {
        printf("Error de memoria.\n");
        free(cyl);
        return NULL;
    }
    cyl->verticesTop = malloc(6 * numVertBase * sizeof(float));
    if (!cyl->verticesTop)
    {
        printf("Error de memoria.\n");
        free(cyl->verticesBase);
        free(cyl);
        return NULL;
    }
    cyl->verticesSide = malloc(6 * numVertSide * sizeof(float));
    if (!cyl->verticesSide)
    {
        printf("Error de memoria.\n");
        free(cyl->verticesBase);
        free(cyl->verticesTop);
        free(cyl);
        return NULL;
    }

    cyl->numVertBase = numVertBase;
    cyl->numVertSide = numVertSide;

    float angleStep = (float)(2 * M_PI) / sectors;

    float angle;
    float x;
    float y;
    // Vertex central base
    cyl->verticesBase[0] = 0;
    cyl->verticesBase[1] = 0;
    cyl->verticesBase[2] = 0;

    cyl->verticesBase[3] = 0;
    cyl->verticesBase[4] = 0;
    cyl->verticesBase[5] = -1;

    // Vertex central top
    cyl->verticesTop[0] = 0;
    cyl->verticesTop[1] = 0;
    cyl->verticesTop[2] = height;

    cyl->verticesTop[3] = 0;
    cyl->verticesTop[4] = 0;
    cyl->verticesTop[5] = 1;
    for (int i = 1; i <= sectors + 1; i++)
    {
        angle = angleStep * (i-1);

        x = radius * cosf(angle);
        y = radius * sinf(angle);

        // Vertex base
        cyl->verticesBase[i * 6] = x;
        cyl->verticesBase[i * 6 + 1] = y;
        cyl->verticesBase[i * 6 + 2] = 0;
        // Normals base
        cyl->verticesBase[i * 6 + 3] = 0;
        cyl->verticesBase[i * 6 + 4] = 0;
        cyl->verticesBase[i * 6 + 5] = -1;

        // Vertex top
        cyl->verticesTop[i * 6] = x;
        cyl->verticesTop[i * 6 + 1] = y;
        cyl->verticesTop[i * 6 + 2] = height;
        // Normals top
        cyl->verticesTop[i * 6 + 3] = 0;
        cyl->verticesTop[i * 6 + 4] = 0;
        cyl->verticesTop[i * 6 + 5] = 1;

        // Vertex side 1 (base)
        cyl->verticesSide[i * 12] = x;
        cyl->verticesSide[i * 12 + 1] = y;
        cyl->verticesSide[i * 12 + 2] = 0;
        // Normals side 1
        cyl->verticesSide[i * 12 + 3] = x;
        cyl->verticesSide[i * 12 + 4] = y;
        cyl->verticesSide[i * 12 + 5] = 0;
        // Vertex side 2 (top)
        cyl->verticesSide[i * 12] = x;
        cyl->verticesSide[i * 12 + 1] = y;
        cyl->verticesSide[i * 12 + 2] = height;
        // Normals side 2
        cyl->verticesSide[i * 12 + 3] = x;
        cyl->verticesSide[i * 12 + 4] = y;
        cyl->verticesSide[i * 12 + 5] = 0;
    }

    glGenBuffers(1, &(cyl->VBO));
    glBindBuffer(GL_ARRAY_BUFFER, cyl->VBO);
    glBufferData(GL_ARRAY_BUFFER, (numVertBase * 2 + numVertSide) * 6 * sizeof(float), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numVertBase * 6 * sizeof(float), cyl->verticesBase);
    glBufferSubData(GL_ARRAY_BUFFER, numVertBase * 6 * sizeof(float), numVertBase * 6 * sizeof(float), cyl->verticesTop);
    glBufferSubData(GL_ARRAY_BUFFER, 2 * (numVertBase * 6 * sizeof(float)), numVertSide * 6 * sizeof(float), cyl->verticesSide);

    glGenVertexArrays(1, &(cyl->VAO));
    glBindVertexArray(cyl->VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return cyl;
}

void drawCylinder(Cylinder* cylinder)
{
    glBindVertexArray(cylinder->VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, cylinder->numVertBase * 6 * sizeof(float));
    glDrawArrays(GL_TRIANGLE_FAN, cylinder->numVertBase * 6 * sizeof(float), cylinder->numVertBase * 6 * sizeof(float));
    glDrawArrays(GL_TRIANGLE_STRIP, 2 * (cylinder->numVertBase * 6 * sizeof(float)), cylinder->numVertSide * 6 * sizeof(float));
    glBindVertexArray(0);
}