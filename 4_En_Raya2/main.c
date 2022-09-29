#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shaderutils.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

const char vertexShaderPath[] = "shaders/vertshader.vert";
const char fragmentShaderPath[] = "shaders/fragshader.frag";

int main()
{
	// Inicialització de GLFW
	glfwInit();
	// Configuració de GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creació de la finestra
	GLFWwindow* window = glfwCreateWindow(800, 600, "Epic window", NULL, NULL);
	if (window == NULL)
	{
		printf("No s'ha pogut crear la finestra");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Carregar totes les funcions de OpenGL a traves de GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("No s'ha pogut inicialitzar GLAD");
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	unsigned int shaders[2];
	// VERTEX SHADER
	shaders[0] = loadShader(vertexShaderPath, GL_VERTEX_SHADER);
	if (shaders[0] == 0)
	{
		printf("Creacio del vertex shader fallida.\n");
		return 0;
	}

	//FRAGMENT SHADER
	shaders[1] = loadShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
	if (shaders[1] == 0)
	{
		printf("Creacio del fragment shader fallida.\n");
		glDeleteShader(shaders[0]);
		return 0;
	}

	// Shader program
	unsigned int shaderProgram = linkProgram(shaders, 2);

	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);

	if (shaderProgram == 0)
	{
		printf("Error de link.\n");
		return 0;
	}

	// Triangles
	float vertices1[] = {
		// positions         // colors
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
	};  // right
	// segon triangle
	float vertices2[] = {
	 0.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
	 0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,  // bottom left
	 1.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top left 
	};
	float* vertices[] = { vertices1, vertices2 };

	/*
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};*/
	// Creem el Vertex Array Object
	unsigned int VAOs[2];
	glGenVertexArrays(2, VAOs);

	// El VAO guardarà tota la configuració de vertex attributes que farem a continuació, i el EBO que vinculem
	unsigned int VBOs[2];
	glGenBuffers(2, VBOs); // Creem un vertex buffer a la GPU

	for (int i = 0; i < 2; i++)
	{
		glBindVertexArray(VAOs[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]); // Vinculem el vertex buffer
		glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), vertices[i], GL_STATIC_DRAW); // Copiem les dades al buffer
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Especifiquem com les dades del vertex buffer han de ser interpretades per la GPU
		glEnableVertexAttribArray(0); // Necessari, no activat per defecte
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	/*
	// Creem el Element Buffer Array
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	*/
	

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();
		processInput(window);

		// Donem valor al uniform del shader
		glUseProgram(shaderProgram);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(VAOs[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		int i;
		glGetIntegerv(GL_POLYGON_MODE, &i);
		glPolygonMode(GL_FRONT_AND_BACK, i == GL_FILL ? GL_LINE : GL_FILL);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}