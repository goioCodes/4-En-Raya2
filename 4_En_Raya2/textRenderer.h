#ifndef __TEXTRENDERER_H_
#define __TEXTRENDERER_H_
#include <cglm/cglm.h>

// Per saber el que signifiquen les mètriques de cada lletra, veure https://freetype.org/freetype2/docs/tutorial/step2.html
typedef struct {
    unsigned int textureID;
    ivec2   size;       // Tamany de la lletra
    ivec2   bearing;    // Desplaçament horitzontal/vertical des de l'origen fins al costat esquerre/superior de la lletra
    unsigned int advance;    // Desplaçament de l'origen per la següent lletra
} Character;

typedef struct {
    Character characterSet[96];
    unsigned int VAO;
    unsigned int VBO;
    mat4 projectionOrtho;
} TextRenderer;

void initTextRenderer(TextRenderer* ren, float scr_width, float scr_height);
void textUpdateScreenSize(TextRenderer* ren, float scr_width, float scr_height);
void renderTextUI(unsigned int program, const char* text, float x, float y, float scale, vec3 color, vec3 outline, TextRenderer* ren);
#endif