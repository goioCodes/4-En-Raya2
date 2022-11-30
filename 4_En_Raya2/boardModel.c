#include "boardModel.h"
#include "board.h"

#include <glad/glad.h>
#include <stdlib.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>

void getCenters(float cents[][NUM_COLS][2], float colrowsize)
{
    // L'array cents ha de tenir longitud NUM_ROWS*NUM_COLS*2 o be ser un array de vec2 de tamany NUM_ROWS*NUM_COLS
    for (int i = 0; i < NUM_ROWS; i++)
    {
        for (int j = 0; j < NUM_COLS; j++)
        {
            cents[i][j][0] = colrowsize * j - colrowsize/2 * NUM_COLS + colrowsize/2;
            cents[i][j][1] = colrowsize * NUM_ROWS - colrowsize * i - colrowsize/2;
        }
    }
}

float* genDistanceField(float cents[][NUM_COLS][2], float tabWidth, float tabHeight, int textureWidth, int textureHeight)
{
    // Un distance field es una textura on cada component conté la distància a la vora d'una imatge. En aquest cas
    // conté la distància al centre del tauler més proper. Els pixels massa propers a un centre seran descartats pel fragment shader.

    float* arr = malloc(textureWidth * textureHeight * sizeof(float));
    if (!arr)
    {
        printf("Error de memoria.\n");
        return NULL;
    }

    float xStep = tabWidth / textureWidth;
    float yStep = tabHeight / textureHeight;
    vec2 realPos;
    float minDistanceInit = tabWidth + tabHeight;
    float minDistance;
    float distance;
    for (int i = 0; i < textureHeight; i++)
    {
        for (int j = 0; j < textureWidth; j++)
        {
            realPos[0] = -tabWidth / 2 + j * xStep;
            realPos[1] = i * yStep;
            minDistance = minDistanceInit;
            for (int k = 0; k < NUM_ROWS; k++)
            {
                for (int l = 0; l < NUM_COLS; l++)
                {
                    if ((distance = glm_vec2_distance(realPos, cents[k][l])) < minDistance)
                    {
                        minDistance = distance;
                    }
                }
            }
            arr[i * textureWidth + j] = minDistance;
        }
    }

    return arr;
}

BoardModel* generateBoard(float faceSeparation, float colrowSize, float legSize, float legWidth, float holeRadius)
{
    // La funció retorna l'struct que conté els vertexs del tauler, les normals i les texture coordinates, a més de la
    // informació necessària per dibuixar-lo

    // El tauler consta de dues cares principals front/back, dues tapes laterals left/right, una tapa inferior bottom
    // i dues pates legs
    //
    // Cares principals, laterals i inferior:   2--3/4
    // 6 vertexs per cara                       | // |
    //                                          1/6--5
    // Pates:
    // Cilindre de 3 sectors per pata
    //
    BoardModel* board = malloc(sizeof(BoardModel));
    if (!board)
    {
        printf("Error de memoria.\n");
        return NULL;
    }

    board->faceSeparation = faceSeparation;
    board->colrowSize = colrowSize;
    board->holeRadius = holeRadius;
    board->legSeparation = NUM_COLS > 2 ? colrowSize * (NUM_COLS - 2) : colrowSize * NUM_COLS * 0.8f;

    float xcoord = colrowSize/2 * NUM_COLS; // colsize * ncols - colsize/2 * ncols
    float ycoord = colrowSize * NUM_ROWS;
    float zcoord = faceSeparation/2;

    float vertsBoard[] = {
        // FRONT
        -xcoord,  0,       zcoord,   0,  0,  1,  0, 0,/* bottom left   */
        -xcoord,  ycoord,  zcoord,   0,  0,  1,  0, 1,/* top left      */
         xcoord,  ycoord,  zcoord,   0,  0,  1,  1, 1,/* top right     */
         xcoord,  ycoord,  zcoord,   0,  0,  1,  1, 1,/* top right     */
         xcoord,  0,       zcoord,   0,  0,  1,  1, 0,/* bottom right  */
        -xcoord,  0,       zcoord,   0,  0,  1,  0, 0,/* bottom left   */
        // BACK
        -xcoord,  0,      -zcoord,   0,  0, -1,  1, 0,/* bottom left   */
        -xcoord,  ycoord, -zcoord,   0,  0, -1,  1, 1,/* top left      */
         xcoord,  ycoord, -zcoord,   0,  0, -1,  0, 1,/* top right     */
         xcoord,  ycoord, -zcoord,   0,  0, -1,  0, 1,/* top right     */
         xcoord,  0,      -zcoord,   0,  0, -1,  0, 0,/* bottom right  */
        -xcoord,  0,      -zcoord,   0,  0, -1,  1, 0,/* bottom left   */
        // LEFT
        -xcoord,  0,       zcoord,  -1,  0,  0,  0, 0,/* front bottom  */
        -xcoord,  0,      -zcoord,  -1,  0,  0,  0, 1,/* back bottom   */
        -xcoord,  ycoord, -zcoord,  -1,  0,  0,  1, 1,/* back top      */
        -xcoord,  ycoord, -zcoord,  -1,  0,  0,  1, 1,/* back top      */
        -xcoord,  ycoord,  zcoord,  -1,  0,  0,  1, 0,/* front top     */
        -xcoord,  0,       zcoord,  -1,  0,  0,  0, 0,/* front bottom  */
        //RIGHT
         xcoord,  0,       zcoord,   1,  0,  0,  0, 0,/* front bottom  */
         xcoord,  0,      -zcoord,   1,  0,  0,  0, 1,/* back bottom   */
         xcoord,  ycoord, -zcoord,   1,  0,  0,  1, 1,/* back top      */
         xcoord,  ycoord, -zcoord,   1,  0,  0,  1, 1,/* back top      */
         xcoord,  ycoord,  zcoord,   1,  0,  0,  1, 0,/* front top     */
         xcoord,  0,       zcoord,   1,  0,  0,  0, 0,/* front bottom  */
        //BOTTOM
        -xcoord,  0,       zcoord,   0, -1,  0,  0, 0,/* front left    */
        -xcoord,  0,      -zcoord,   0, -1,  0,  0, 1,/* back left     */
         xcoord,  0,      -zcoord,   0, -1,  0,  1, 1,/* back right    */
         xcoord,  0,      -zcoord,   0, -1,  0,  1, 1,/* back right    */
         xcoord,  0,       zcoord,   0, -1,  0,  1, 0,/* front right   */
        -xcoord,  0,       zcoord,   0, -1,  0,  0, 0,/* front left    */
    };

    glGenBuffers(1, &(board->VBO));
    glBindBuffer(GL_ARRAY_BUFFER, board->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertsBoard), vertsBoard, GL_STATIC_DRAW);

    glGenVertexArrays(1, &(board->VAO));
    glBindVertexArray(board->VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    getCenters(board->centers, colrowSize);

    int textureSize = BoardRes4K ? 4096 : 1024;
    float* distanceField = genDistanceField(board->centers, colrowSize * NUM_COLS, colrowSize * NUM_ROWS, textureSize, textureSize);
    if (!distanceField)
    {
        printf("No s'ha pogut generar el distance field.\n");
        return NULL;
    }
    glGenTextures(1, &(board->dfTexture));
    glBindTexture(GL_TEXTURE_2D, board->dfTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureSize, textureSize, 0, GL_RED, GL_FLOAT, distanceField);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(distanceField);

    board->legM = generateCylinder(legSize, legWidth, 3);

    return board;
}

void drawBoard(BoardModel* boardM, unsigned int program, mat4 model, mat4 view, Material* material)
{
    setUniformMat4(program, "model", GL_FALSE, model);
    setUniformMaterial(program, "material", material);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, boardM->dfTexture);

    setUniformf(program, "distThreshold", boardM->holeRadius);
    setUniformBool(program, "checkRadius", true); // Per les cares principals es descarten pixels a dins del radi dels forats
    glBindVertexArray(boardM->VAO); 
    glDrawArrays(GL_TRIANGLES, 0, 12);

    setUniformBool(program, "checkRadius", false);
    glDrawArrays(GL_TRIANGLES, 12, 18);

    mat4 cylRotation = GLM_MAT4_IDENTITY_INIT;
    glm_rotate_y(cylRotation, (float)M_PI_2, cylRotation);
    glm_rotate_z(cylRotation, (float)M_PI_2, cylRotation);

    mat4 cylinderModel;
    glm_translate_make(cylinderModel, (vec3) { -boardM->legSeparation / 2, -0.25f, 0 });
    glm_mat4_mul(cylinderModel, cylRotation, cylinderModel);
    glm_mat4_mul(model, cylinderModel, cylinderModel);
    drawCylinder(boardM->legM, program, cylinderModel, NULL);

    glm_translate_make(cylinderModel, (vec3) { boardM->legSeparation / 2, -0.25f, 0 });
    glm_mat4_mul(cylinderModel, cylRotation, cylinderModel);
    glm_mat4_mul(model, cylinderModel, cylinderModel);
    drawCylinder(boardM->legM, program, cylinderModel, NULL);
}