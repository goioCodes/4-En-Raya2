#ifndef __SKYBOXMODEL_H_
#define __SKYBOXMODEL_H_

typedef struct {
    unsigned int VBO;
    unsigned int VAO;
} SkyboxModel;

SkyboxModel* generateSkybox();
void drawSkybox(SkyboxModel* skyboxM, unsigned int program, unsigned int cubemap);
#endif