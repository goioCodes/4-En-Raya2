#include "shaderutils.h"

#include <glad/glad.h>
#include <cglm/cglm.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char* readShaderSource(const char* filepath)
{
    // Funció per llegir el codi GLSL d'un arxiu i guardar-ho a un string
    char* buffer;
    long length;

    // El compilador de MSVC obliga a utilitzar fopen_s, canviar a fopen en cas d'utilitzar un altre compilador
#ifdef _MSC_VER
    FILE* sourceFile;
    fopen_s(&sourceFile, filepath, "rb"); // Obrim en mode binari
#else
    FILE* sourceFile = fopen(filepath, "rb");
#endif
    if (!sourceFile)
    {
        fprintf(stderr, "No s'ha pogut obrir l'arxiu amb cami:\n%s\n", filepath);
        return NULL;
    }
    if (fseek(sourceFile, 0, SEEK_END)) // Ens movem al final del arxiu i comptem el tamany
    {
        fprintf(stderr, "Error de fseek. SEEK_END no suportat?\n");
        fclose(sourceFile);
        return NULL;
    }
    length = ftell(sourceFile);
    if (length == -1)
    {
        fprintf(stderr, "Error de ftell. Arxiu massa gran?\n");
        fclose(sourceFile);
        return NULL;
    }
    fseek(sourceFile, 0, SEEK_SET); // I tornem al principi

    buffer = malloc((size_t)length + 1); // Afegim 1 pel null terminator
    if (!buffer)
    {
        fprintf(stderr, "No s'ha pogut alocar memoria.\n");
        fclose(sourceFile);
        return NULL;
    }
    if (fread(buffer, 1, length, sourceFile) != length)
    {
        fprintf(stderr, "Error llegint l'arxiu. No s'ha completat.\n");
        free(buffer);
        fclose(sourceFile);
        return NULL;
    }
    buffer[length] = '\0';
    fclose(sourceFile);
    return buffer;
}

int numDigits(int n) {
    if (n < 0) n = (n == INT_MIN) ? INT_MAX : -n;
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    /*      2147483647 es 2^31-1 */
    return 10;
}

char* strreplace(char* orig, const char* rep, const char* with)
{
    // Reemplaça nomes la primera ocurrencia de rep en orig, amb with
    char* result; // the return string
    char* ins;    // the next insert point
    char* tmp;    // varies
    size_t len_rep;  // length of rep (the string to remove)
    size_t len_with; // length of with (the string to replace rep with)
    size_t len_front; // distance between rep and end of last rep

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) + 1);

    if (!result)
        return NULL;

    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    ins = strstr(orig, rep);
    if (ins)
    {
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to "end of rep"
    }
    else
    {
        printf("Warning: no match.\n");
    }

    strcpy(tmp, orig);
    return result;
}

void insertConstantInSource(char** shaderSource, char* constName, int constValue)
{
    char define[] = "#define ";
    char placeholder[] = " 1";
    char* oldstr = malloc((strlen(define) + strlen(constName) + strlen(placeholder) + 1) * sizeof(char));
    char* newstr = malloc((strlen(define) + strlen(constName) + 1 + numDigits(constValue) + 2) * sizeof(char)); // S'afegeix un espai i una altra plaça en cas que constValue sigui negatiu
    if (!oldstr || !newstr)
    {
        printf("Error alocacio memoria.\n");
        return;
    }
    sprintf(oldstr, "%s%s%s", define, constName, placeholder);
    sprintf(newstr, "%s%s %d", define, constName, constValue);
    char* newShaderSource = strreplace(*shaderSource, oldstr, newstr);
    free(oldstr);
    free(newstr);
    if (!newShaderSource)
    {
        printf("Error en el canvi de substrings.\n");
        return;
    }
    free(*shaderSource);
    *shaderSource = newShaderSource;
}

bool checkShaderCompilation(unsigned int shader)
{
    int  success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        int logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        char* infoLog = malloc(logLength);
        if (!infoLog)
        {
            fprintf(stderr, "ERROR::SHADER::COMPILATION_FAILED\n");
        }
        else
        {
            glGetShaderInfoLog(shader, logLength, NULL, infoLog);
            fprintf(stderr, "ERROR::SHADER::COMPILATION_FAILED\n%s\n", infoLog);
            free(infoLog);
        }
        return false;
    }
    return true;
}

bool checkProgramLink(unsigned int shaderProgram)
{
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        int logLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
        char* infoLog = malloc(logLength);
        if (!infoLog)
        {
            fprintf(stderr, "ERROR::SHADER::PROGRAM::LINK_FAILED\n");
            return 0;
        }
        else
        {
            glGetProgramInfoLog(shaderProgram, logLength, NULL, infoLog);
            fprintf(stderr, "ERROR::SHADER::PROGRAM::LINK_FAILED\n%s\n", infoLog);
            free(infoLog);
        }
        return false;
    }

    return true;
}

unsigned int loadShader(const char* filepath, GLenum shaderType, int len, char** constantNames, int* constantValues)
{
    unsigned int shader;

    char* shaderSource = readShaderSource(filepath);
    if (!shaderSource)
    {
        fprintf(stderr, "No s'ha pogut carregar el codi font del shader.\n");
        return 0;
    }

    if (len >= 1 && constantNames && constantValues)
    {
        for (int i = 0; i < len; i++)
        {
            insertConstantInSource(&shaderSource, constantNames[i], constantValues[i]);
        }
    }

    shader = glCreateShader(shaderType); // Creem un objecte shader
    glShaderSource(shader, 1, &shaderSource, NULL); // Afegeix el codi al objecte del shader
    glCompileShader(shader); // Compila i a continuacio comprovem el resultat
    free(shaderSource);

    // Comprovació
    if (!checkShaderCompilation(shader))
    {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

unsigned int linkProgram(unsigned int* shaders, int n)
{
    unsigned int shaderProgram = glCreateProgram();

    for (int i = 0; i < n; i++)
    {
        glAttachShader(shaderProgram, shaders[i]);
    }
    glLinkProgram(shaderProgram);

    // Comprovació
    if (!checkProgramLink(shaderProgram))
    {
        glDeleteProgram(shaderProgram);
        return 0;
    }

    return shaderProgram;
}

void setUniformBool(unsigned int program, const char* name, bool value)
{
    glUniform1i(glGetUniformLocation(program, name), value);
}

void setUniformi(unsigned int program, const char* name, int value)
{
    glUniform1i(glGetUniformLocation(program, name), value);
}

void setUniformf(unsigned int program, const char* name, float value)
{
    glUniform1f(glGetUniformLocation(program, name), value);
}

void setUniformMat4(unsigned int program, const char* name, bool transpose, mat4 mat)
{
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, transpose, (float*)mat);
}

void setUniformVec3(unsigned int program, const char* name, vec3 vec)
{
    glUniform3f(glGetUniformLocation(program, name), vec[0], vec[1], vec[2]);
}

void setUniformVec2(unsigned int program, const char* name, vec2 vec)
{
    glUniform2f(glGetUniformLocation(program, name), vec[0], vec[1]);
}

void setUniformMaterial(unsigned int program, const char* name, Material* mat)
{
    char diffuse[] = "name.diffuse";
    char specular[] = "name.specular";
    char shininess[] = "name.shininess";

    char* rdiffuse = strreplace(diffuse, "name", name);
    char* rspecular = strreplace(specular, "name", name);
    char* rshininess = strreplace(shininess, "name", name);

    if (!rdiffuse || !rspecular || !rshininess)
    {
        printf("Error de memoria.");
        return;
    }

    setUniformVec3(program, rdiffuse, mat->diffuse);
    setUniformVec3(program, rspecular, mat->specular);
    setUniformf(program, rshininess, mat->shininess);

    free(rdiffuse);
    free(rspecular);
    free(rshininess);
}

void setUniformDirLight(unsigned int program, const char* name, DirLight* dirLight)
{
    char direction[] = "name.direction";
    char ambient[] = "name.ambient";
    char diffuse[] = "name.diffuse";
    char specular[] = "name.specular";

    char* rdirection = strreplace(direction, "name", name);
    char* rambient = strreplace(ambient, "name", name);
    char* rdiffuse = strreplace(diffuse, "name", name);
    char* rspecular = strreplace(specular, "name", name);

    if (!rambient || !rdiffuse || !rspecular || !rdirection)
    {
        printf("Error de memoria.");
        return;
    }

    setUniformVec3(program, rdirection, dirLight->direction);
    setUniformVec3(program, rambient, dirLight->ambient);
    setUniformVec3(program, rdiffuse, dirLight->diffuse);
    setUniformVec3(program, rspecular, dirLight->specular);

    free(rdirection);
    free(rambient);
    free(rdiffuse);
    free(rspecular);
}