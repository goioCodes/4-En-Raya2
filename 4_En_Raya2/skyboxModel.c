#include "skyboxModel.h"

#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

SkyboxModel* generateSkybox()
{
    // Els vertexs d'una skybox són simplement un cub centrat a l'origen.
    // No calen texture coordinates ja que la pròpia posició del vèrtex (interpolada) és l'index del cubemap
    SkyboxModel* skybox = malloc(sizeof(SkyboxModel));
    if (!skybox)
    {
        printf("Error de memoria.\n");
        return NULL;
    }

    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenBuffers(1, &(skybox->VBO));
    glBindBuffer(GL_ARRAY_BUFFER, skybox->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &(skybox->VAO));
    glBindVertexArray(skybox->VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return skybox;
}

void drawSkybox(SkyboxModel* skyboxM, unsigned int program, unsigned int cubemap)
{
    // L'skybox no requereix model matrix. Els vèrtexs sempre són a l'origen, i donat que la view matrix que s'utilitza
    // no té components de traslació, provoca que la càmara també sigui a l'origen.
    glDepthMask(GL_FALSE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    glBindVertexArray(skyboxM->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthMask(GL_TRUE);
}