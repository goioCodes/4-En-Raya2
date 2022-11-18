#ifndef __SHADERUTILS_H_
#define __SHADERUTILS_H_
#include <cglm/cglm.h>
#include <glad/glad.h>

typedef struct {
    vec3 diffuse;
    vec3 specular;
    float shininess;
} Material;

typedef struct {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} DirLight;

int numDigits(int n);
unsigned int loadShader(const char* filepath, GLenum shaderType, int len, char** constantNames, int* constantValues);
unsigned int linkProgram(unsigned int* shaders, int n);
void setUniformBool(unsigned int program, const char* name, bool value);
void setUniformi(unsigned int program, const char* name, int value);
void setUniformf(unsigned int program, const char* name, float value);
void setUniformMat4(unsigned int program, const char* name, bool transpose, mat4 mat);
void setUniformVec3(unsigned int program, const char* name, vec3 vec);
void setUniformVec2(unsigned int program, const char* name, vec2 vec);
void setUniformMaterial(unsigned int program, const char* name, Material* mat);
void setUniformDirLight(unsigned int program, const char* name, DirLight* dirLight);
#endif
