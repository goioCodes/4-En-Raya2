#ifndef __CYLINDER_H_
#define __CYLINDER_H_

typedef struct{
    int numVertBase;
    int numVertSide;
    float* verticesBase;
    float* verticesTop;
    float* verticesSide;
    unsigned int VAO;
    unsigned int VBO;
} Cylinder;

Cylinder* generateCylinder(float radius, float height, int sectors);
void drawCylinder(Cylinder* cylinder);

#endif // !__CYLINDER_H_
