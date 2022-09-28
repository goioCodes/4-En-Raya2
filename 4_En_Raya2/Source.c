#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

const char* vertexShaderSource = "#version 460 core\n"
	"layout(location = 0) in vec3 aPos;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";

const char* fragmentShaderSource = "#version 460 core\n"
	"out vec4 FragColor; \n"
	"\n"
	"void main()\n"
	"{\n"
	"	FragColor = vec4(1.0f, 0.8f, 0.8f, 1.0f);\n"
	"}";

const char* fragmentShaderSource2 = "#version 460 core\n"
	"out vec4 FragColor; \n"
	"\n"
	"void main()\n"
	"{\n"
	"	FragColor = vec4(1.0f, 1.0f, 0.2f, 1.0f);\n"
	"}";

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


	// Triangles
	float vertices1[] = {
	-1.0f, -0.5f, 0.0f,  // left
	 0.0f,  0.0f, 0.0f,  // top
	-0.5f, -1.0f, 0.0f   // right
	};  // right
	// segon triangle
	float vertices2[] = {
	 0.0f,  0.0f, 0.0f,
	 0.5f,  1.0f, 0.0f,  // bottom left
	 1.0f,  0.5f, 0.0f   // top left 
	};
	float* vertices[] = { vertices1, vertices2 };
	printf("sizeof(vertices) %zi\n", sizeof(vertices));
	printf("sizeof(vertices1) %zi\n", sizeof(vertices[0]));
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
		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), vertices[i], GL_STATIC_DRAW); // Copiem les dades al buffer
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Especifiquem com les dades del vertex buffer han de ser interpretades per la GPU
		glEnableVertexAttribArray(0); // Necessari, no activat per defecte
	}

	/*
	// Creem el Element Buffer Array
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	*/
	

	// VERTEX SHADER
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER); // Creem un vertex shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // Afegeix el codi al objecte del shader
	glCompileShader(vertexShader); // Compila i a continuacio comprovem el resultat
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
	}
	//FRAGMENT SHADER
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Creem el fragment shader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
	}

	//FRAGMENT SHADER 2
	unsigned int fragmentShader2;
	fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER); // Creem el fragment shader
	glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
	glCompileShader(fragmentShader2);
	glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
	}

	// Shader program
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINK_FAILED\n%s\n", infoLog);
	}

	// Shader program 2
	unsigned int shaderProgram2;
	shaderProgram2 = glCreateProgram();
	glAttachShader(shaderProgram2, vertexShader);
	glAttachShader(shaderProgram2, fragmentShader2);
	glLinkProgram(shaderProgram2);
	glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINK_FAILED\n%s\n", infoLog);
	}

	// Neteja
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(fragmentShader2);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();
		processInput(window);

		glUseProgram(shaderProgram);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glUseProgram(shaderProgram2);
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