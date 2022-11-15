#include "connect4.h"
#include "camera.h"
#include "shaderutils.h"
#include "board.h"
#include "boardModel.h"
#include "cylinderModel.h"

#include <cglm/cglm.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

const char vertexShaderPath[] = "shaders/vertshader.vert";
const char boardFragmentShaderPath[] = "shaders/board.frag";
const char fragmentShaderPath[] = "shaders/fragshader.frag";
const char lightCubeFragShaderPath[] = "shaders/fragshaderlight.frag";

#define SCR_WIDTH_INIT 800
#define SCR_HEIGHT_INIT 600

unsigned int SCR_WIDTH = SCR_WIDTH_INIT;
unsigned int SCR_HEIGHT = SCR_HEIGHT_INIT;

float lastX = SCR_WIDTH_INIT / 2.0f;
float lastY = SCR_HEIGHT_INIT / 2.0f;

Camera camera;
const vec3 initialCameraPos = { 0.f, 1.f, 3.f };

float deltaTime = 0.f;
float lastFrame = 0.f;

vec3 lightDir = { -0.2f, -1.0f, -0.3f };

bool threedmode = true;

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
	shaders[0] = loadShader(vertexShaderPath, GL_VERTEX_SHADER, 0, NULL, NULL);
	if (shaders[0] == 0)
	{
		printf("Creacio del vertex shader fallida.\n");
		return 0;
	}

	//FRAGMENT SHADER BOARD
	char* names[] = { "NCENTERS" };
	int vals[] = { NUM_COLS * NUM_ROWS };
	shaders[1] = loadShader(boardFragmentShaderPath, GL_FRAGMENT_SHADER, 1, names, vals);
	if (shaders[1] == 0)
	{
		printf("Creacio del fragment shader fallida.\n");
		glDeleteShader(shaders[0]);
		return 0;
	}

	// Shader program board
	unsigned int shaderProgramBoard = linkProgram(shaders, 2);

	glDeleteShader(shaders[1]);

	if (shaderProgramBoard == 0)
	{
		printf("Error de link.\n");
		glDeleteShader(shaders[0]);
		return 0;
	}

	//FRAGMENT SHADER GENERAL
	shaders[1] = loadShader(fragmentShaderPath, GL_FRAGMENT_SHADER, 0, NULL, NULL);
	if (shaders[1] == 0)
	{
		printf("Creacio del fragment shader fallida.\n");
		glDeleteShader(shaders[0]);
		glDeleteProgram(shaderProgramBoard);
		return 0;
	}

	unsigned int shaderProgram = linkProgram(shaders, 2);

	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);

	if (shaderProgram == 0)
	{
		printf("Error de link.\n");
		glDeleteProgram(shaderProgramBoard);
		return 0;
	}

	/*
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
	*/
	//unsigned int indices[] = {  // note that we start from 0!
	//	0, 1, 3,   // first triangle
	//	1, 2, 3    // second triangle
	//};

	float vertices[54];
	generateVertices(vertices);

	// Creem el Vertex Array Object
	// El VAO guardarà tota la configuració de vertex attributes que farem a continuació, i el EBO que vinculem
	unsigned int VBO;
	glGenBuffers(1, &VBO); // Creem un vertex buffer a la GPU

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int VAO1, VAO2;
	glGenVertexArrays(1, &VAO1);
	glGenVertexArrays(1, &VAO2);

	glBindVertexArray(VAO1);

	// Posicions dels vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0); // Especifiquem com les dades del vertex buffer han de ser interpretades per la GPU
	glEnableVertexAttribArray(0); // Necessari, no activat per defecte
	// Vectors normals a cada vertex. Nomes volem les normals positives
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	glBindVertexArray(VAO2);

	// Posicions dels vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0); // Especifiquem com les dades del vertex buffer han de ser interpretades per la GPU
	glEnableVertexAttribArray(0);
	// Vectors normals a cada vertex. Ara nomes agafem les normals negatives
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	Cylinder* cylinder = generateCylinder(0.1f, 0.09f, 10);


	vec2 centers[NUM_ROWS][NUM_COLS];
	getCenters(centers);
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

		glClearColor(0.8f, 0.8f, 0.8f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// PRIMERA CARA
		glUseProgram(shaderProgramBoard);
		mat4 view = GLM_MAT4_IDENTITY_INIT;
		cameraGetViewMatrix(&camera, view);
		setUniformMat4(shaderProgramBoard, "view", GL_FALSE, (float*)view);

		mat4 projection = GLM_MAT4_ZERO_INIT;
		glm_perspective(glm_rad(60.f), (float)SCR_WIDTH / (float)SCR_HEIGHT, .1f, 100.f, projection);
		setUniformMat4(shaderProgramBoard, "projection", GL_FALSE, (float*)projection);

		mat4 model = GLM_MAT4_IDENTITY_INIT;
		glm_translate(model, (vec3) { 0.f, 0.f, 0.05f });
		glm_scale(model, (vec3) { 0.5f, 0.5f, 1.f });
		setUniformMat4(shaderProgramBoard, "model", GL_FALSE, (float*)model);

		setUniformVec3(shaderProgramBoard, "material.ambient", (vec3){ 0.f, 0.6f, 1.f });
		setUniformVec3(shaderProgramBoard, "material.diffuse", (vec3) { 0.f, 0.6f, 1.f });
		setUniformVec3(shaderProgramBoard, "material.specular", (vec3) { 0.5f, 0.5f, 0.5f });
		setUniformf(shaderProgramBoard, "material.shininess", 32.f);

		setUniformVec3(shaderProgramBoard, "dirLight.ambient", (vec3) { 0.4f, 0.4f, 0.4f });
		setUniformVec3(shaderProgramBoard, "dirLight.diffuse", (vec3) { 0.7f, 0.7f, 0.7f });
		setUniformVec3(shaderProgramBoard, "dirLight.specular", (vec3) { 1.0f, 1.0f, 1.0f });
		
		vec3 lightViewDir;
		glm_mat4_mulv3(view, lightDir, 0.0f, lightViewDir);
		setUniformVec3(shaderProgramBoard, "dirLight.direction", lightViewDir);

		int maxDigits = numDigits(NUM_COLS * NUM_ROWS);
		char* uniformName = malloc((maxDigits + strlen("viewCenters[]") + 1) * sizeof(char));
		if (!uniformName)
		{
			printf("Error de memoria.\n");
			return;
		}
		vec3 viewCenter;
		for (int i = 0; i < NUM_ROWS; i++)
		{
			for (int j = 0; j < NUM_COLS; j++)
			{
				sprintf(uniformName, "viewCenters[%d]", i*NUM_COLS + j);
				glm_mat4_mulv3(model, (vec3) { centers[i][j][0], centers[i][j][1], 0.0f }, 1.f, viewCenter);
				glm_mat4_mulv3(view, viewCenter, 1.f, viewCenter);
				setUniformVec3(shaderProgramBoard, uniformName, viewCenter);
			}
		}

		setUniformf(shaderProgramBoard, "radius", 0.1f);

		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		*/

		glBindVertexArray(VAO1);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// SEGONA CARA

		glm_mat4_identity(model);
		glm_translate(model, (vec3) { 0.f, 0.f, -0.05f });
		glm_scale(model, (vec3) { 0.5f, 0.5f, 1.0f });
		setUniformMat4(shaderProgramBoard, "model", GL_FALSE, (float*)model);

		for (int i = 0; i < NUM_ROWS; i++)
		{
			for (int j = 0; j < NUM_COLS; j++)
			{
				sprintf(uniformName, "viewCenters[%d]", i * NUM_COLS + j);
				glm_mat4_mulv3(model, (vec3) { centers[i][j][0], centers[i][j][1], 0.0f }, 1.f, viewCenter);
				glm_mat4_mulv3(view, viewCenter, 1.f, viewCenter);
				setUniformVec3(shaderProgramBoard, uniformName, viewCenter);
			}
		}

		free(uniformName);

		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		glUseProgram(shaderProgram);
		glm_mat4_identity(model);
		setUniformMat4(shaderProgram, "model", GL_FALSE, (float*)model);
		setUniformMat4(shaderProgram, "projection", GL_FALSE, (float*)projection);
		setUniformMat4(shaderProgram, "view", GL_FALSE, (float*)view);

		setUniformVec3(shaderProgram, "material.ambient", (vec3) { 1.f, 0.0f, 0.f });
		setUniformVec3(shaderProgram, "material.diffuse", (vec3) { 1.f, 0.0f, 0.f });
		setUniformVec3(shaderProgram, "material.specular", (vec3) { 0.5f, 0.5f, 0.5f });
		setUniformf(shaderProgram, "material.shininess", 32.f);

		setUniformVec3(shaderProgram, "dirLight.ambient", (vec3) { 0.4f, 0.4f, 0.4f });
		setUniformVec3(shaderProgram, "dirLight.diffuse", (vec3) { 0.7f, 0.7f, 0.7f });
		setUniformVec3(shaderProgram, "dirLight.specular", (vec3) { 1.0f, 1.0f, 1.0f });

		setUniformVec3(shaderProgram, "dirLight.direction", lightViewDir);
		drawCylinder(cylinder);

		/*
		glUseProgram(lightCubeProgram);
		setUniformMat4(lightCubeProgram, "view", GL_FALSE, (float*)view);
		setUniformMat4(lightCubeProgram, "projection", GL_FALSE, (float*)projection);
		glm_mat4_identity(model);
		glm_translate(model, (vec3) { 0.7f, 0.2f, 2.0f });
		glm_scale(model, (vec3){ .2f, .2f, .2f });
		setUniformMat4(lightCubeProgram, "model", GL_FALSE, (float*)model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		*/

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