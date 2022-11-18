#ifndef __BOARDMODEL_H_
#define __BOARDMODEL_H_
#include "board.h"
#include "cylinderModel.h"

#define BoardRes4K 0

typedef struct {
    float faceSeparation;
    float colrowSize;
    float holeRadius;
    float legSeparation;
    unsigned int VBO;
    unsigned int VAO;
    unsigned int dfTexture;
    float centers[NUM_ROWS][NUM_COLS][2];
    CylinderModel* legM;
} BoardModel;

BoardModel* generateBoard(float faceSeparation, float colrowSize, float legSize, float legWidth, float holeRadius);
void drawBoard(BoardModel* boardM, unsigned int program, mat4 model, mat4 view, Material* material);
#endif
