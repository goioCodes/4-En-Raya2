#include "connect4.h"
#include "camera.h"
#include "shaderutils.h"
#include "board.h"
#include "boardModel.h"
#include "cylinderModel.h"
#include "tableModel.h"
#include "skyboxModel.h"
#include "textRenderer.h"
#include "miniMax.h"

#include <cglm/cglm.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


const char vertexShaderPath[] = "Shaders/vertshader.vert";
const char boardFragmentShaderPath[] = "Shaders/board.frag";
const char fragmentShaderPath[] = "Shaders/fragshader.frag";
const char textVertexShaderPath[] = "Shaders/text.vert";
const char textFragmentShaderPath[] = "Shaders/text.frag";
const char tableVertexShaderPath[] = "Shaders/table.vert";
const char tableFragmentShaderPath[] = "Shaders/table.frag";
const char skyboxVertexShaderPath[] = "Shaders/skybox.vert";
const char skyboxFragmentShaderPath[] = "Shaders/skybox.frag";

const char tableDiffuseMapPath[] = "Textures/wood_color.jpg";
const char tableNormalMapPath[] = "Textures/wood_normal.png";

const char* skyboxFacesPath[] = {
	"Textures/skybox/right.jpg",
	"Textures/skybox/left.jpg",
	"Textures/skybox/top.jpg",
	"Textures/skybox/bottom.jpg",
	"Textures/skybox/front.jpg",
	"Textures/skybox/back.jpg",
};

const char* skyboxFacesPath2[] = {
	"Textures/skybox2/px.png",
	"Textures/skybox2/nx.png",
	"Textures/skybox2/py.png",
	"Textures/skybox2/ny.png",
	"Textures/skybox2/pz.png",
	"Textures/skybox2/nz.png",
};

const char* skyboxFacesPath3[] = {
	"Textures/skybox3/px.png",
	"Textures/skybox3/nx.png",
	"Textures/skybox3/py.png",
	"Textures/skybox3/ny.png",
	"Textures/skybox3/pz.png",
	"Textures/skybox3/nz.png",
};

#define SCR_WIDTH_INIT 800
#define SCR_HEIGHT_INIT 600

const vec3 initialCameraPos = { 0.f, 1.f, 3.f };
const vec3 cameraOrbitCenter = { 0.f, 1.f, 0.f };

const vec3 textParams = { 25.f, 25.f, 1.f };

#define lightDir (vec3){ -1.5f, -3.0f, -1.5f }
#define lightPos (vec3){ 1.5f, 3.f, 1.5f }
#define boardScale (vec3){ 0.5f, 0.5f, 0.5f }

const float tokenSpawnHeight = 6;

const float gravity = 30.0;

typedef struct {
	int cursorInd;
	float cursorPos;
	vec2 tokenPositions[NUM_ROWS][NUM_COLS];

	bool computerWaiting;
	float computerWaitStartTime;
	float computerWaitDuration;

	bool gameFinished;

	// Internal game
	Board board;
} GameState;

typedef struct {
	Camera camera;
	TextRenderer textRender;
	char textBox[100];

	// Window size
	unsigned int scr_width;
	unsigned int scr_height;

	// Mouse position
	float lastX;
	float lastY;

	bool draggingCamera;

	// Framerate
	float deltaTime;
	float lastFrame;

	bool twoPlayerMode;
	// Game info
	GameState gameState;
} Resources;

void processPhysics(Resources* resources, float centers[][NUM_COLS][2], float fixedDeltaTime);
void placeToken3D(Resources* resources, int col);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(const char** faces);

Material boardMat = {
		.diffuse = {0.f, 0.6f, 1.f},
		.specular = {.5f,.5f,.5f},
		.shininess = 128.f
};

Material p1Mat = {
	.diffuse = {1.f, 0.f, 0.f},
	.specular = {.5f,.5f,.5f},
	.shininess = 128.f
};

Material p2Mat = {
	.diffuse = {1.f, 1.f, 0.f},
	.specular = {.5f,.5f,.5f},
	.shininess = 128.f
};

Material cursorMat = {
	.diffuse = {.5f, 1.f, .5f},
	.specular = {0.f,0.f,0.f},
	.shininess = 32.f
};

DirLight dirLight = {
	.ambient = { 0.2f, 0.2f, 0.2f },
	.diffuse = { 1.f, 1.f, 1.f },
	.specular = { 1.0f, 1.0f, 1.0f },
};

const int fixedPhysicsStepsPS = 120;

bool threedmode = true;

int main()
{
	if (!threedmode)
	{
		connect4Main();
		return 0;
	}

	//printf("%lld\n", sizeof(Node));

	// Inicialitzaci� de GLFW
	glfwInit();
	// Configuraci� de GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creaci� de la finestra
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH_INIT, SCR_HEIGHT_INIT, "Epic window", NULL, NULL);
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
	glEnable(GL_DEPTH_TEST);

	//--------GLOBAL RESOURCES---------
	Resources resources = {
		.textBox = "Torn del jugador 1.",

		.scr_width = SCR_WIDTH_INIT,
		.scr_height = SCR_HEIGHT_INIT,

		.lastX = SCR_WIDTH_INIT / 2.0f,
		.lastY = SCR_HEIGHT_INIT / 2.0f,

		.twoPlayerMode = false,
		.gameState = {
						.cursorInd = NUM_COLS / 2,
					 }
	};
	initializeBoard(&resources.gameState.board);

	//------------CAMERA---------------
	cameraInitialize(&resources.camera, initialCameraPos, cameraOrbitCenter);

	//-------------TEXT----------------
	initTextRenderer(&resources.textRender, (float)SCR_WIDTH_INIT, (float)SCR_HEIGHT_INIT);

	//---------------------------------
	glfwSetWindowUserPointer(window, &resources);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	shaders[1] = loadShader(boardFragmentShaderPath, GL_FRAGMENT_SHADER, 0, NULL, NULL);
	if (!shaders[1])
	{
		printf("Creacio del fragment shader board fallida.\n");
		return -1;
	}

	// Shader program board
	unsigned int shaderProgramBoard = linkProgram(shaders, 2);

	glDeleteShader(shaders[1]);

	if (!shaderProgramBoard)
	{
		printf("Error de link.\n");
		return -1;
	}

	//FRAGMENT SHADER GENERAL
	shaders[1] = loadShader(fragmentShaderPath, GL_FRAGMENT_SHADER, 0, NULL, NULL);
	if (!shaders[1])
	{
		printf("Creacio del fragment shader general fallida.\n");
		return -1;
	}

	// Shader program general
	unsigned int shaderProgram = linkProgram(shaders, 2);

	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);

	if (!shaderProgram)
	{
		printf("Error de link.\n");
		return -1;
	}

	// VERTEX SHADER TEXT
	shaders[0] = loadShader(textVertexShaderPath, GL_VERTEX_SHADER, 0, NULL, NULL);
	if (!shaders[0])
	{
		printf("Creacio del vertex shader text fallida.\n");
		return -1;
	}

	//FRAGMENT SHADER TEXT
	shaders[1] = loadShader(textFragmentShaderPath, GL_FRAGMENT_SHADER, 0, NULL, NULL);
	if (!shaders[1])
	{
		printf("Creacio del fragment shader text fallida.\n");
		return -1;
	}

	// Shader program text
	unsigned int shaderProgramText = linkProgram(shaders, 2);

	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);

	if (!shaderProgramText)
	{
		printf("Error de link.\n");
		return -1;
	}

	// VERTEX SHADER TABLE
	shaders[0] = loadShader(tableVertexShaderPath, GL_VERTEX_SHADER, 0, NULL, NULL);
	if (!shaders[0])
	{
		printf("Creacio del vertex shader table fallida.\n");
		return -1;
	}

	//FRAGMENT SHADER TABLE
	shaders[1] = loadShader(tableFragmentShaderPath, GL_FRAGMENT_SHADER, 0, NULL, NULL);
	if (!shaders[1])
	{
		printf("Creacio del fragment shader table fallida.\n");
		return -1;
	}

	// Shader program table
	unsigned int shaderProgramTable = linkProgram(shaders, 2);

	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);

	if (!shaderProgramTable)
	{
		printf("Error de link.\n");
		return -1;
	}

	// VERTEX SHADER SKYBOX
	shaders[0] = loadShader(skyboxVertexShaderPath, GL_VERTEX_SHADER, 0, NULL, NULL);
	if (!shaders[0])
	{
		printf("Creacio del vertex shader skybox fallida.\n");
		return -1;
	}

	//FRAGMENT SHADER SKYBOX
	shaders[1] = loadShader(skyboxFragmentShaderPath, GL_FRAGMENT_SHADER, 0, NULL, NULL);
	if (!shaders[1])
	{
		printf("Creacio del fragment shader skybox fallida.\n");
		return -1;
	}

	// Shader program skybox
	unsigned int shaderProgramSkybox = linkProgram(shaders, 2);

	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);

	if (!shaderProgramSkybox)
	{
		printf("Error de link.\n");
		return -1;
	}

	//----------SHADER SETUP-----------

	glUseProgram(shaderProgramTable);
	setUniformi(shaderProgramTable, "diffuseMap", 0);
	setUniformi(shaderProgramTable, "normalMap", 1);

	glUseProgram(shaderProgramSkybox);
	setUniformi(shaderProgramTable, "skybox", 0);

	//------------MATERIALS------------

	TexturedMaterial tableMat;
	tableMat.shininess = 32.f;
	tableMat.diffuseTex = loadTexture(tableDiffuseMapPath);
	tableMat.normalTex = loadTexture(tableNormalMapPath);

	unsigned int skyboxCubemap = loadCubemap(skyboxFacesPath);

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

	TableModel* tableM = generateTable();
	if (!tableM)
	{
		printf("No s'han pogut generar els vertexs de la taula.\n");
		return -1;
	}

	SkyboxModel* skyboxM = generateSkybox();

	resources.gameState.cursorPos = boardM->centers[0][resources.gameState.cursorInd][0];

	float fixedDeltaTime = 1.0f / fixedPhysicsStepsPS;
	float lastPhysicsTime = 0.0f;


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

		if ((currentFrame - lastPhysicsTime) >= fixedDeltaTime)
		{
			// Passar fixedDeltaTime per evitar salts amb pocs FPS?
			processPhysics(&resources, boardM->centers, fixedDeltaTime);
			lastPhysicsTime = currentFrame;
		}

		if (resources.gameState.computerWaiting && (currentFrame - resources.gameState.computerWaitStartTime) >= resources.gameState.computerWaitDuration)
		{
			placeToken3D(&resources, miniMaxGetPlay(&resources.gameState.board));
			resources.gameState.computerWaiting = false;
		}

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


		// DIBUIXANT: SKYBOX
		// Uniforms del shader:
		// Vertex:                   Fragment:
		// view, projection          skybox
		glUseProgram(shaderProgramSkybox);
		setUniformMat4(shaderProgramSkybox, "projection", GL_FALSE, projection);
		mat3 upperLeftView;
		glm_mat4_pick3(view, upperLeftView);
		mat4 skyView = GLM_MAT4_IDENTITY_INIT;
		glm_mat4_ins3(upperLeftView, skyView);
		setUniformMat4(shaderProgramSkybox, "view", GL_FALSE, skyView);

		drawSkybox(skyboxM, shaderProgramSkybox, skyboxCubemap);


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
					resources.gameState.tokenPositions[i][j][0],
					0.f
				});

				// La funci� drawCylinder assigna els uniforms model i material
				drawCylinder(cylinderM, shaderProgram, model, ptoken == PLAYER1 ? &p1Mat : &p2Mat);
			}
		}

		
		// DIBUIXANT: CURSOR
		glm_scale_make(model, boardScale);
		glm_translate(model, (vec3) { resources.gameState.cursorPos, boardM->centers[0][0][1] + boardM->colrowSize, 0 });
		glm_rotate_z(model, -(float)M_PI_2, model);
		glm_scale(model, (vec3) { 0.7f, 1.f, 1.f });

		drawCylinder(cursorM, shaderProgram, model, &cursorMat);


		// DIBUIXANT: TAULA
		glUseProgram(shaderProgramTable);
		// Uniforms del shader:
		// Vertex:                   Fragment:
		// model, view, projection   diffuseMap, normalMap, shininess, viewPos, lightPos
		setUniformMat4(shaderProgramTable, "projection", GL_FALSE, projection);
		setUniformMat4(shaderProgramTable, "view", GL_FALSE, view);
		glm_scale_make(model, boardScale);
		glm_translate(model, (vec3) { 0.f, 0.5f * cosf(2 * (float)M_PI / 3) - 0.5f * boardScale[1], 0.f });
		glm_rotate_x(model, -(float)M_PI_2, model);
		glm_scale(model, (vec3){ 5.f, 5.f, 1.f });
		setUniformVec3(shaderProgramTable, "viewPos", resources.camera.position);
		setUniformVec3(shaderProgramTable, "lightPos", lightPos);

		drawTable(tableM, shaderProgramTable, model, &tableMat);
		

		// DIBUIXANT: TEXT UI
		glUseProgram(shaderProgramText);
		renderTextUI(shaderProgramText, resources.textBox, textParams[0], textParams[1], textParams[2], (vec3) { 0.f, 0.f, 0.f }, (vec3) { 1.f, 1.f, 1.f }, & resources.textRender);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void processPhysics(Resources* resources,  float centers[][NUM_COLS][2], float fixedDeltaTime)
{
	GameState* gs = &resources->gameState;
	gs->cursorPos = glm_lerp(gs->cursorPos, centers[0][gs->cursorInd][0], 15.f * fixedDeltaTime);
	
	Token ptoken;
	for (int i = 0; i < NUM_ROWS; i++)
	{
		for (int j = 0; j < NUM_COLS; j++)
		{
			ptoken = gs->board.m[i][j];
			if (ptoken == EMPTY)
			{
				continue;
			}

			gs->tokenPositions[i][j][1] -= gravity * fixedDeltaTime;
			gs->tokenPositions[i][j][0] += gs->tokenPositions[i][j][1] * fixedDeltaTime;
			if (gs->tokenPositions[i][j][0] < centers[i][0][1])
			{
				gs->tokenPositions[i][j][1] = fabsf(gs->tokenPositions[i][j][1]) * 0.3f;
				gs->tokenPositions[i][j][0] = centers[i][0][1];
			}
		}
	}

	if (!resources->draggingCamera)
	{
		cameraLerpToTarget(&resources->camera, fixedDeltaTime);
	}
}

void queueComputerPlay(Resources* resources, float waitTime)
{
	resources->gameState.computerWaiting = true;
	resources->gameState.computerWaitDuration = waitTime;
	resources->gameState.computerWaitStartTime = (float)glfwGetTime();
}

void nextTurn(Resources* resources, int row, int col)
{
	if (checkWin(&resources->gameState.board, row, col))
	{
		sprintf(resources->textBox, "El jugador %d ha guanyat!", getLastPlayer(&resources->gameState.board));
		resources->gameState.gameFinished = true;
		return;
	}
	if (boardIsFull(&resources->gameState.board))
	{
		sprintf(resources->textBox, "Empat! Tothom perd.");
		resources->gameState.gameFinished = true;
		return;
	}
	sprintf(resources->textBox, "Torn del jugador %d.", getCurrentPlayer(&resources->gameState.board));

	if (getCurrentPlayer(&resources->gameState.board) == PLAYER2 && !resources->twoPlayerMode)
	{
		queueComputerPlay(resources, 1.0f);
	}
}

void placeToken3D(Resources* resources, int col)
{
	int row = placeToken(&resources->gameState.board, col);
	if (row != -1)
	{
		resources->gameState.tokenPositions[row][col][0] = tokenSpawnHeight;
		nextTurn(resources, row, col);
		return;
	}
	else
	{
		printf("Columna plena!\n");
	}
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
		if (resources->gameState.cursorInd > 0)
		{
			resources->gameState.cursorInd--;
		}
	}
	if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		if (resources->gameState.cursorInd < NUM_COLS-1)
		{
			resources->gameState.cursorInd++;
		}
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && !resources->gameState.gameFinished && !resources->gameState.computerWaiting)
	{
		placeToken3D(resources, resources->gameState.cursorInd);
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

	//cameraProcessMouseMovement(&resources->camera, xoffset, yoffset);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		cameraProcessMouseMovement(&resources->camera, xoffset, yoffset);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	Resources* resources = glfwGetWindowUserPointer(window);
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		resources->draggingCamera = true;
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		resources->draggingCamera = false;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Resources* resources = glfwGetWindowUserPointer(window);
	cameraProcessScroll(&resources->camera, (float)yoffset);
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

unsigned int loadTexture(const char* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	stbi_set_flip_vertically_on_load(true);
	int width, height, numComponents;
	unsigned char* data = stbi_load(path, &width, &height, &numComponents, 0);
	if (data)
	{
		GLenum format;
		if (numComponents == 1)
			format = GL_RED;
		else if (numComponents == 3)
			format = GL_RGB;
		else if (numComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		printf("No s'ha pogut carregar la textura.\n");
		stbi_image_free(data);
		glDeleteTextures(1, &textureID);
		return 0;
	}
	stbi_set_flip_vertically_on_load(false);

	return textureID;
}

unsigned int loadCubemap(const char** faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, numChannels;
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char* data = stbi_load(faces[i], &width, &height, &numChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			printf("No s'ha pogut carregar la cara %s.\n", faces[i]);
			glDeleteTextures(1, &textureID);
			return 0;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}