#ifndef __QUADMODEL_H_
#define __QUADMODEL_H_
#include "shaderutils.h"
#include <cglm/cglm.h>

typedef struct {
    unsigned int VAO;
    unsigned int VBO;
} TableModel;

TableModel* generateTable();
void drawTable(TableModel* table, unsigned int program, mat4 model, TexturedMaterial* material);
#endif
