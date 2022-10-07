#include <glad/glad.h>

unsigned int loadShader(const char* filepath, GLenum shaderType);
unsigned int linkProgram(unsigned int* shaders, int n);
void setUniformi(unsigned int program, const char* name, int value);
void setUniformf(unsigned int program, const char* name, float value);
void setUniformMat4(unsigned int program, const char* name, bool transpose, float* mat);
void setUniformVec3(unsigned int program, const char* name, vec3 vec);