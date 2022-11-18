#include "connect4.h"
#include "camera.h"
#include "shaderutils.h"
#include "board.h"
#include "boardModel.h"
#include "cylinderModel.h"
#include "textRenderer.h"

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

const char vertexShaderPath[] = "Shaders/vertshader.vert";
const char boardFragmentShaderPath[] = "Shaders/board.frag";
const char fragmentShaderPath[] = "Shaders/fragshader.frag";
const char textVertexShaderPath[] = "Shaders/textshader.vert";
const char textFragmentShaderPath[] = "Shaders/textshader.frag";

#define SCR_WIDTH_INIT 800
#define SCR_HEIGHT_INIT 600

const vec3 initialCameraPos = { 0.f, 1.f, 3.f };

#define lightDir (vec3){ -0.2f, -1.0f, -0.3f }
#define boardScale (vec3){ 0.5f, 0.5f, 0.5f }

typedef struct {
	int cursorPos;

	// Internal game
	Board board;
	Token currentPlayer;
	int turnCount;
} GameState;

typedef struct {
	Camera camera;
	TextRenderer textRender;

	// Window size
	unsigned int scr_width;
	unsigned int scr_height;

	// Mouse position
	float lastX;
	float lastY;
	
	// Framerate
	float deltaTime;
	float lastFrame;

	// Game info
	GameState gameState;
} Resources;


Material boardMat = {
		.diffuse = {0.f, 0.6f, 1.f},
		.specular = {.5f,.5f,.5f},
		.shininess = 32.f
};

Material p1Mat = {
	.diffuse = {1.f, 0.f, 0.f},
	.specular = {.5f,.5f,.5f},
	.shininess = 32.f
};

Material p2Mat = {
	.diffuse = {1.f, 1.f, 0.f},
	.specular = {.5f,.5f,.5f},
	.shininess = 32.f
};

Material cursorMat = {
	.diffuse = {.5f, 1.f, .5f},
	.specular = {0.f,0.f,0.f},
	.shininess = 32.f
};

DirLight dirLight = {
	.ambient = { 0.4f, 0.4f, 0.4f },
	.diffuse = { 0.7f, 0.7f, 0.7f },
	.specular = { 1.0f, 1.0f, 1.0f },
};


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

	glViewport(0, 0, SCR_WIDTH_INIT, SCR_HEIGHT_INIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//--------GLOBAL RESOURCES---------
	Resources resources = {
		.scr_width = SCR_WIDTH_INIT,
		.scr_height = SCR_HEIGHT_INIT,

		.lastX = SCR_WIDTH_INIT / 2.0f,
		.lastY = SCR_HEIGHT_INIT / 2.0f,

		.lastFrame = 0.0f,
		.deltaTime = 0.0f,

		.gameState = {	
						.cursorPos = NUM_COLS / 2,
						.currentPlayer = PLAYER1,
						.turnCount = 1
					 }
	};
	initializeBoard(&resources.gameState.board);

	//------------CAMERA---------------
	cameraInitialize(&resources.camera, initialCameraPos);

	//-------------TEXT----------------
	initTextRenderer(&resources.textRender, (float)SCR_WIDTH_INIT, (float)SCR_HEIGHT_INIT);

	//---------------------------------
	glfwSetWindowUserPointer(window, &resources);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//------------SHADERS--------------

	unsigned int shaders[2];
	// VERTEX SHADER GENERAL
	shaders[0] = loadShader(vertexShaderPath, GL_VERTEX_SHADER, 0, NULL, NULL);
	if (!shaders[0])
	{
		printf("Creacio del vertex shader general fallida.\n");
		return -1;
	}

	//FRAGMENT SHADER BOARD
	char* names[] = { "NCENTERS" };
	int vals[] = { NUM_COLS * NUM_ROWS };
	shaders[1] = loadShader(boardFragmentShaderPath, GL_FRAGMENT_SHADER, 1, names, vals);
	if (!shaders[1])
	{
		printf("Creacio del fragment shader board fallida.\n");
		glDeleteShader(shaders[0]);
		return -1;
	}

	// Shader program board
	unsigned int shaderProgramBoard = linkProgram(shaders, 2);

	glDeleteShader(shaders[1]);

	if (!shaderProgramBoard)
	{
		printf("Error de link.\n");
		glDeleteShader(shaders[0]);
		return -1;
	}

	//FRAGMENT SHADER GENERAL
	shaders[1] = loadShader(fragmentShaderPath, GL_FRAGMENT_SHADER, 0, NULL, NULL);
	if (!shaders[1])
	{
		printf("Creacio del fragment shader general fallida.\n");
		glDeleteShader(shaders[0]);
		glDeleteProgram(shaderProgramBoard);
		return -1;
	}

	// Shader program general
	unsigned int shaderProgram = linkProgram(shaders, 2);

	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);

	if (!shaderProgram)
	{
		printf("Error de link.\n");
		glDeleteProgram(shaderProgramBoard);
		return -1;
	}

	// VERTEX SHADER TEXT
	shaders[0] = loadShader(textVertexShaderPath, GL_VERTEX_SHADER, 0, NULL, NULL);
	if (!shaders[0])
	{
		printf("Creacio del vertex shader text fallida.\n");
		glDeleteProgram(shaderProgramBoard);
		glDeleteProgram(shaderProgram);
		return -1;
	}

	//FRAGMENT SHADER TEXT
	shaders[1] = loadShader(textFragmentShaderPath, GL_FRAGMENT_SHADER, 0, NULL, NULL);
	if (!shaders[1])
	{
		printf("Creacio del fragment shader text fallida.\n");
		glDeleteShader(shaders[0]);
		glDeleteProgram(shaderProgramBoard);
		glDeleteProgram(shaderProgram);
		return -1;
	}

	// Shader program text
	unsigned int shaderProgramText = linkProgram(shaders, 2);

	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);

	if (!shaderProgramText)
	{
		printf("Error de link.\n");
		glDeleteProgram(shaderProgramBoard);
		glDeleteProgram(shaderProgram);
		return -1;
	}

	//-------------MODELS--------------

	BoardModel* boardM = generateBoard(0.2f, 0.5f, 0.5, 0.1f, 0.2f);
	if (!boardM)
	{
		printf("No s'ha pogut generar el tauler.\n");
		return -1;
	}

	CylinderModel* cylinderM = generateCylinder(0.25f, 0.19f, 10);
	if (!cylinderM)
	{
		printf("No s'han pogut generar els vertexs del cilindre.\n");
		return -1;
	}

	CylinderModel* cursorM = generateCylinder(0.2f, 0.1f, 3);
	if (!cursorM)
	{
		printf("No s'han pogut generar els vertexs de les fletxes.\n");
		return -1;
	}

	int cursorPosition = NUM_COLS/2;

	cameraInitialize(&resources.camera, initialCameraPos);
	glfwSetCursorPos(window, resources.lastX, resources.lastY);

	// Render loop
	// +------/!\------+------/!\------+------/!\------+------/!\------+------/!\------+------/!\------+------/!\------+------/!\------+
	//|!|------+------|!|------+------|!|------+------|!|------+------|!|------+------|!|------+------|!|------+------|!|------+------|!|
	// +------\!/------+------\!/------+------\!/------+------\!/------+------\!/------+------\!/------+------\!/------+------\!/------+
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		resources.deltaTime = currentFrame - resources.lastFrame;
		resources.lastFrame = currentFrame;
		
		glfwPollEvents();
		processInput(window);

		glClearColor(0.8f, 0.8f, 0.8f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// VIEW MATRIX
		mat4 view;
		cameraGetViewMatrix(&resources.camera, view);

		// PROJECTION MATRIX
		mat4 projection;
		glm_perspective(glm_rad(60.f), (float)resources.scr_width / (float)resources.scr_height, .1f, 100.f, projection);

		// LIGHT DIRECTION TO VIEW SPACE
		glm_mat4_mulv3(view, lightDir, 0.0f, dirLight.direction);

		// MODEL MATRIX
		mat4 model;


		// DIBUIXANT: TAULER
		glUseProgram(shaderProgramBoard);
		// Uniforms del shader:
		// Vertex:                   Fragment:
		// model, view, projection   material, dirLight, viewCenters, radius, checkRadius
		setUniformMat4(shaderProgramBoard, "projection", GL_FALSE, projection);
		setUniformMat4(shaderProgramBoard, "view", GL_FALSE, view);
		glm_scale_make(model, boardScale);

		setUniformDirLight(shaderProgramBoard, "dirLight", &dirLight);
		
		// Aquesta funcio configura els uniforms restants
		// model, material, radius, checkRadius, viewCenters
		drawBoard(boardM, shaderProgramBoard, model, view, &boardMat);


		// DIBUIXANT: FITXES
		glUseProgram(shaderProgram);
		// Uniforms del shader:
		// Vertex:                   Fragment:
		// model, view, projection   material, dirLight
		setUniformMat4(shaderProgram, "projection", GL_FALSE, projection);
		setUniformMat4(shaderProgram, "view", GL_FALSE, view);
		setUniformDirLight(shaderProgram, "dirLight", &dirLight);

		Token ptoken;
		for (int i = 0; i < NUM_ROWS; i++)
		{
			for (int j = 0; j < NUM_COLS; j++)
			{
				ptoken = resources.gameState.board.m[i][j];
				if (ptoken == EMPTY)
				{
					continue;
				}

				glm_scale_make(model, boardScale);
				glm_translate(model, (vec3) {
					boardM->centers[i][j][0],
					boardM->centers[i][j][1],
					0.f
				});

				// La funció drawCylinder assigna els uniforms model i material
				drawCylinder(cylinderM, shaderProgram, model, ptoken == PLAYER1 ? &p1Mat : &p2Mat);
			}
		}


		// DIBUIXANT: CURSOR
		glm_scale_make(model, boardScale);
		glm_translate(model, (vec3) { boardM->centers[0][resources.gameState.cursorPos][0], boardM->centers[0][0][1] + boardM->colrowSize, 0 });
		glm_rotate_z(model, -(float)M_PI_2, model);
		glm_scale(model, (vec3) { 0.7f, 1.f, 1.f });

		drawCylinder(cursorM, shaderProgram, model, &cursorMat);


		// DIBUIXANT: TEXT UI
		glDisable(GL_DEPTH_TEST);
		glUseProgram(shaderProgramText);
		renderTextUI(shaderProgramText, "Epico texto guau", 25.f, 25.f, 1.f, (vec3) { 0.f, 0.f, 0.f }, & resources.textRender);

		glEnable(GL_DEPTH_TEST);
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
	
	Resources* resources = glfwGetWindowUserPointer(window);
	if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		if (resources->gameState.cursorPos > 0)
		{
			resources->gameState.cursorPos--;
		}
	}
	if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		if (resources->gameState.cursorPos < NUM_COLS-1)
		{
			resources->gameState.cursorPos++;
		}
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		int row = placeToken(&resources->gameState.board, resources->gameState.currentPlayer, resources->gameState.cursorPos);
		if (row != -1)
		{
			resources->gameState.currentPlayer = resources->gameState.currentPlayer == PLAYER1 ? PLAYER2 : PLAYER1;
			return;
		}
		else
		{
			printf("Columna plena!\n");
		}
	}
}

void mouse_callback(GLFWwindow* window, double xposd, double yposd)
{
	Resources* resources =  glfwGetWindowUserPointer(window);
	float xpos = (float)xposd;
	float ypos = (float)yposd;

	float xoffset = xpos - resources->lastX;
	float yoffset = resources->lastY - ypos; // les coordenades es prenen des del costat superior esquerra
	resources->lastX = xpos;
	resources->lastY = ypos;

	cameraProcessMouseMovement(&resources->camera, xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Resources* resources = glfwGetWindowUserPointer(window);
	glViewport(0, 0, width, height);
	resources->scr_width = width;
	resources->scr_height = height;
	textUpdateScreenSize(&resources->textRender, (float)width, (float)height);
}

void processInput(GLFWwindow* window)
{
	Resources* resources = glfwGetWindowUserPointer(window);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	/*
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraProcessKeyborad(&resources->camera, FORWARD, resources->deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraProcessKeyborad(&resources->camera, BACKWARD, resources->deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraProcessKeyborad(&resources->camera, LEFT, resources->deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraProcessKeyborad(&resources->camera, RIGHT, resources->deltaTime);
	}
	*/
}