#include <cglm/call.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "shaderutils.h"
#include "camera.h"
#include "connect4.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

const char vertexShaderPath[] = "shaders/vertshader.vert";
const char fragmentShaderPath[] = "shaders/fragshader.frag";
const char lightCubeFragShaderPath[] = "shaders/fragshaderlight.frag";

#define SCR_WIDTH_INIT 800
#define SCR_HEIGHT_INIT 600

unsigned int SCR_WIDTH = SCR_WIDTH_INIT;
unsigned int SCR_HEIGHT = SCR_HEIGHT_INIT;

float lastX = SCR_WIDTH_INIT / 2.0f;
float lastY = SCR_HEIGHT_INIT / 2.0f;
bool firstMouse = true;

Camera camera;
const vec3 initialCameraPos = { 0.f, 0.f, 3.f };

float deltaTime = 0.f;
float lastFrame = 0.f;

vec3 lightPos = { 1.2f, 1.f, 2.f };

bool threedmode = false;

int main()
{
	if (!threedmode)
	{
		connect4Main();
		return 0;
	}
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
	glEnable(GL_DEPTH_TEST);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


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

	glDeleteShader(shaders[1]);

	if (shaderProgram == 0)
	{
		printf("Error de link.\n");
		glDeleteShader(shaders[0]);
		return 0;
	}

	//FRAGMENT SHADER LIGHT
	shaders[1] = loadShader(lightCubeFragShaderPath, GL_FRAGMENT_SHADER);
	if (shaders[1] == 0)
	{
		printf("Creacio del fragment shader fallida.\n");
		glDeleteShader(shaders[0]);
		return 0;
	}

	unsigned int lightCubeProgram = linkProgram(shaders, 2);

	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);

	if (lightCubeProgram == 0)
	{
		printf("Error de link.\n");
		return 0;
	}

	// Triangles
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	//unsigned int indices[] = {  // note that we start from 0!
	//	0, 1, 3,   // first triangle
	//	1, 2, 3    // second triangle
	//};

	// Creem el Vertex Array Object
	// El VAO guardarà tota la configuració de vertex attributes que farem a continuació, i el EBO que vinculem
	unsigned int VBO;
	glGenBuffers(1, &VBO); // Creem un vertex buffer a la GPU

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Especifiquem com les dades del vertex buffer han de ser interpretades per la GPU
	glEnableVertexAttribArray(0); // Necessari, no activat per defecte
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Especifiquem com les dades del vertex buffer han de ser interpretades per la GPU
	glEnableVertexAttribArray(0);
	

	// Creem el Element Buffer Array
	//unsigned int EBO;
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	//// Creacio de textures
	//stbi_set_flip_vertically_on_load(true);
	////Textura 1
	//unsigned int texture1;
	//glGenTextures(1, &texture1);
	//glBindTexture(GL_TEXTURE_2D, texture1);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//int width, height, nrChannels;
	//unsigned char* data = stbi_load("Textures/container.jpg", &width, &height, &nrChannels, 0);
	//if (data)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//else
	//{
	//	printf("Error carregant textura.\n");
	//}
	//stbi_image_free(data);

	//unsigned int texture2;
	//glGenTextures(1, &texture2);
	//glBindTexture(GL_TEXTURE_2D, texture2);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//data = stbi_load("Textures/awesomeface.png", &width, &height, &nrChannels, 0);
	//if (data)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//else
	//{
	//	printf("Error carregant textura.\n");
	//}
	//stbi_image_free(data);

	//setUniformi(shaderProgram, "texture1", 0);
	//setUniformi(shaderProgram, "texture2", 1);
	
	cameraInitialize(&camera, initialCameraPos);
	glfwSetCursorPos(window, lastX, lastY);
	// Render loop
	// +------[+]------+------[+]------+------[+]------+------[+]------+------[+]------+------[+]------+------[+]------+------[+]------+
	//[+]------+------[+]------+------[+]------+------[+]------+------[+]------+------[+]------+------[+]------+------[+]------+------[+]
	// +------[+]------+------[+]------+------[+]------+------[+]------+------[+]------+------[+]------+------[+]------+------[+]------+
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		glfwPollEvents();
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightPos[0] = sin(glfwGetTime()) * 2.0f;
		lightPos[1] = sin(glfwGetTime() / 2.0f);
		lightPos[2] = cos(glfwGetTime()) * 2.0f;

		glUseProgram(shaderProgram);
		setUniformVec3(shaderProgram, "objectColor", (vec3){ 1.0f, 0.5f, 0.31f });
		setUniformVec3(shaderProgram, "lightColor", (vec3){ 1.0f, 1.0f, 1.0f });
		setUniformVec3(shaderProgram, "lightPos", lightPos);

		mat4 view = GLM_MAT4_IDENTITY_INIT;
		cameraGetViewMatrix(&camera, view);
		setUniformMat4(shaderProgram, "view", GL_FALSE, (float*)view);

		mat4 projection = GLM_MAT4_ZERO_INIT;
		glm_perspective(glm_rad(60.f), (float)SCR_WIDTH / (float)SCR_HEIGHT, .1f, 100.f, projection);
		setUniformMat4(shaderProgram, "projection", GL_FALSE, (float*)projection);

		mat4 model = GLM_MAT4_IDENTITY_INIT;
		setUniformMat4(shaderProgram, "model", GL_FALSE, (float*)model);

		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		*/

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glUseProgram(lightCubeProgram);
		setUniformMat4(lightCubeProgram, "view", GL_FALSE, (float*)view);
		setUniformMat4(lightCubeProgram, "projection", GL_FALSE, (float*)projection);
		glm_mat4_identity(model);
		glm_translate(model, lightPos);
		glm_scale(model, (vec3){ .2f, .2f, .2f });
		setUniformMat4(lightCubeProgram, "model", GL_FALSE, (float*)model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


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

void mouse_callback(GLFWwindow* window, double xposd, double yposd)
{
	float xpos = (float)xposd;
	float ypos = (float)yposd;

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // les coordenades es prenen des del costat superior esquerra
	lastX = xpos;
	lastY = ypos;

	cameraProcessMouseMovement(&camera, xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraProcessKeyborad(&camera, FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraProcessKeyborad(&camera, BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraProcessKeyborad(&camera, LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraProcessKeyborad(&camera, RIGHT, deltaTime);
	}
}