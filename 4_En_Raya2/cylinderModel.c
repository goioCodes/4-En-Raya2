#include "cylinderModel.h"
#include "shaderutils.h"

#include <cglm/cglm.h>
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

CylinderModel* generateCylinder(float radius, float height, int sectors)
{
    // La funcio retorna l'array de vertexs i normals, i retorna la longitud de l'array pel parametre len
    // 
    // Un cilindre té 2 tapes (bases) i segments rectangulars allargats verticalment entre les dues tapes (laterals).
    // Numero de vertexs per cada array (notem que si un vertex te un vector normal diferent, es guarda per separat
    // encara que sigui la mateixa posicio. Tambe repetim el primer i ultim vertex per
    // tancar el GL_TRIANGLE_FAN o el GL_TRIANGLE_STRIP):
    //                                                                                                      6/2
    // Base/Tapa del cilindre:                                                                              /|\
    // A mes de tenir tants vertexs com sectors, repetim l'ultim i sumem el vertex central del cercle      5-1-3
    // (sectors + 1 + 1)                                                                                    \|/
    //                                                                                                       4
    int numVertBase = sectors + 2;
    // 4 vertexs per segment rectangular, on els 2 primers es comparteixen amb el triangle anterior        2-4-6
    // i els 2 ultims es comparteixen amb el següent                                                       |\|\|...
    // (2 * sectors + 2)                                                                                   1-3-5
    int numVertSide = 2 * (sectors + 1);


    CylinderModel* cyl = malloc(sizeof(CylinderModel));
    if (!cyl)
    {
        printf("Error de memoria.\n");
        return NULL;
    }

    float* verticesBase = malloc(6 * numVertBase * sizeof(float));
    if (!verticesBase)
    {
        printf("Error de memoria.\n");
        free(cyl);
        return NULL;
    }
    float* verticesTop = malloc(6 * numVertBase * sizeof(float));
    if (!verticesTop)
    {
        printf("Error de memoria.\n");
        free(verticesBase);
        free(cyl);
        return NULL;
    }
    float* verticesSide = malloc(6 * numVertSide * sizeof(float));
    if (!verticesSide)
    {
        printf("Error de memoria.\n");
        free(verticesBase);
        free(verticesTop);
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
    verticesBase[0] = 0;           verticesBase[3] = 0;
    verticesBase[1] = 0;           verticesBase[4] = 0;
    verticesBase[2] = -height / 2; verticesBase[5] = -1;

    // Vertex central top
    verticesTop[0] = 0;          verticesTop[3] = 0;
    verticesTop[1] = 0;          verticesTop[4] = 0;
    verticesTop[2] = height / 2; verticesTop[5] = 1;
    
    for (int i = 0; i <= sectors; i++)
    {
        angle = angleStep * i;

        x = radius * cosf(angle);
        y = radius * sinf(angle);

        // Vertex base                               // Normals base
        verticesBase[(i + 1) * 6]     = x;           verticesBase[(i + 1) * 6 + 3] =  0;
        verticesBase[(i + 1) * 6 + 1] = y;           verticesBase[(i + 1) * 6 + 4] =  0;
        verticesBase[(i + 1) * 6 + 2] = -height / 2; verticesBase[(i + 1) * 6 + 5] = -1;

        // Vertex top                                // Normals top
        verticesTop[(i + 1) * 6]      = x;           verticesTop[(i + 1) * 6 + 3]  =  0;
        verticesTop[(i + 1) * 6 + 1]  = y;           verticesTop[(i + 1) * 6 + 4]  =  0;
        verticesTop[(i + 1) * 6 + 2]  = height / 2;  verticesTop[(i + 1) * 6 + 5]  =  1;

        // Vertex side 1 (base)                 // Normals side 1
        verticesSide[i * 12]     = x;           verticesSide[i * 12 + 3] = x;
        verticesSide[i * 12 + 1] = y;           verticesSide[i * 12 + 4] = y;
        verticesSide[i * 12 + 2] = -height / 2; verticesSide[i * 12 + 5] = 0;

        // Vertex side 2 (top)                 // Normals side 2
        verticesSide[i * 12 + 6] = x;          verticesSide[i * 12 + 9]  = x;
        verticesSide[i * 12 + 7] = y;          verticesSide[i * 12 + 10] = y;
        verticesSide[i * 12 + 8] = height / 2; verticesSide[i * 12 + 11] = 0;
    }

    glGenBuffers(1, &(cyl->VBO));
    glBindBuffer(GL_ARRAY_BUFFER, cyl->VBO);
    glBufferData(GL_ARRAY_BUFFER, (numVertBase * 2 + numVertSide) * 6 * sizeof(float), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numVertBase * 6 * sizeof(float), verticesBase);
    glBufferSubData(GL_ARRAY_BUFFER, numVertBase * 6 * sizeof(float), numVertBase * 6 * sizeof(float), verticesTop);
    glBufferSubData(GL_ARRAY_BUFFER, 2 * (numVertBase * 6 * sizeof(float)), numVertSide * 6 * sizeof(float), verticesSide);
    // Tots els vertexs del cilindre estan guardats consecutivament en un array en format Vertex/Normal i Base/Top/Side

    free(verticesBase);
    free(verticesTop);
    free(verticesSide);

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

void drawCylinder(CylinderModel* cylinder, unsigned int program, mat4 model, Material* material)
{
    if (material)
    {
        setUniformMaterial(program, "material", material);
    }
    setUniformMat4(program, "model", GL_FALSE, model);
    glBindVertexArray(cylinder->VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, cylinder->numVertBase);
    glDrawArrays(GL_TRIANGLE_FAN, cylinder->numVertBase, cylinder->numVertBase);
    glDrawArrays(GL_TRIANGLE_STRIP, 2 * cylinder->numVertBase, cylinder->numVertSide);
}