#include "tableModel.h"
#include "shaderutils.h"

#include <cglm/cglm.h>
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

TableModel* generateTable()
{
	TableModel* table = malloc(sizeof(TableModel));
	if (!table)
	{
		printf("Error de memoria.\n");
		return NULL;
	}

	float verts[] = {
		-1, -1,  0,  0, 0, /* bottom left   */
		-1,  1,  0,  0, 1, /* top left      */
		 1,  1,  0,  1, 1, /* top right     */
		 1,  1,  0,  1, 1, /* top right     */
		 1, -1,  0,  1, 0, /* bottom right  */
		-1, -1,  0,  0, 0, /* bottom left   */
	};

	glGenBuffers(1, &(table->VBO));
	glBindBuffer(GL_ARRAY_BUFFER, table->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glGenVertexArrays(1, &(table->VAO));
	glBindVertexArray(table->VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return table;
}

void drawTable(TableModel* table, unsigned int program, mat4 model, TexturedMaterial* material)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material->diffuseTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, material->normalTex);
	setUniformf(program, "shininess", material->shininess);

	setUniformMat4(program, "model", GL_FALSE, model);
	glBindVertexArray(table->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}