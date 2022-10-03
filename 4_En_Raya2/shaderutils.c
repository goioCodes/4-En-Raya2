#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <glad/glad.h>

char* readShaderSource(const char* filepath)
{
    // Funció per llegir el codi GLSL d'un arxiu i guardar-ho a un string
    char* buffer;
    long length;

    // El compilador de MSVC obliga a utilitzar fopen_s, canviar a fopen en cas d'utilitzar un altre compilador
    //FILE* sourceFile = fopen(filepath, "rb"); // Obrim en mode binari
    FILE* sourceFile;
    fopen_s(&sourceFile, filepath, "rb");
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
    if (buffer == NULL)
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

unsigned int loadShader(const char* filepath, GLenum shaderType)
{
    unsigned int shader;

    char* shaderSource = readShaderSource(filepath);
    if (!shaderSource)
    {
        fprintf(stderr, "No s'ha pogut carregar el codi font del shader.\n");
        return 0;
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

void setUniformi(unsigned int program, const char* name, int value)
{
    glUniform1i(glGetUniformLocation(program, name), value);
}

void setUniformf(unsigned int program, const char* name, float value)
{
    glUniform1f(glGetUniformLocation(program, name), value);
}

void setUniformMat4f(unsigned int program, const char* name, bool transpose, float* mat)
{
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, transpose, mat);
}