#include <glad/glad.h>

unsigned int loadShader(const char* filepath, GLenum shaderType);
unsigned int linkProgram(unsigned int* shaders, int n);