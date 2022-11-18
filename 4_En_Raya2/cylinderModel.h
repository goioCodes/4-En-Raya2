#ifndef __CYLINDER_H_
#define __CYLINDER_H_
#include "shaderutils.h"

typedef struct{
    int numVertBase;
    int numVertSide;
    unsigned int VAO;
    unsigned int VBO;
} CylinderModel;

CylinderModel* generateCylinder(float radius, float height, int sectors);
void drawCylinder(CylinderModel* cylinder, unsigned int program, mat4 model, Material* material);

#endif // !__CYLINDER_H_
