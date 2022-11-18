#ifndef __TEXTRENDERER_H_
#define __TEXTRENDERER_H_

#include <cglm/cglm.h>

typedef struct {
    unsigned int textureID;  // ID handle of the glyph texture
    ivec2   size;       // Size of glyph
    ivec2   bearing;    // Offset from baseline to left/top of glyph
    unsigned int advance;    // Offset to advance to next glyph
} Character;

typedef struct {
    Character characterSet[96];
    unsigned int VAO;
    unsigned int VBO;
    mat4 projectionOrtho;
} TextRenderer;

void initTextRenderer(TextRenderer* ren, float scr_width, float scr_height);
void textUpdateScreenSize(TextRenderer* ren, float scr_width, float scr_height);
void renderTextUI(unsigned int program, const char* text, float x, float y, float scale, vec3 color, TextRenderer* ren);
#endif