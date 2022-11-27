#include "textRenderer.h"
#include "shaderutils.h"

#include <string.h>
#include <cglm/cglm.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>

void initTextRenderer(TextRenderer* ren, float scr_width, float scr_height)
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		printf("ERROR::FREETYPE: Could not init FreeType Library\n");
		return;
	}

	FT_Face face;
	if (FT_New_Face(ft, "Fonts/arial.ttf", 0, &face))
	{
		printf("ERROR::FREETYPE: Failed to load font");
		return;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	FT_GlyphSlot slot = face->glyph;

	for (unsigned char c = 32; c < 128; c++)
	{
		// load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			printf("ERROR::FREETYTPE: Failed to load Glyph");
			continue;
		}

		FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);

		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			.textureID = texture,
			.size = {face->glyph->bitmap.width, face->glyph->bitmap.rows},
			.bearing = {face->glyph->bitmap_left, face->glyph->bitmap_top},
			.advance = face->glyph->advance.x
		};
		ren->characterSet[c - 32] = character;
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glm_ortho(0.0f, scr_width, 0.0f, scr_height, -2.f, 2.f, ren->projectionOrtho);

	float verts[] = {
		0.f, 1.f,  0.f, 0.f,
		0.f, 0.f,  0.f, 1.f,
		1.f, 0.f,  1.f, 1.f,

		0.f, 1.f,  0.f, 0.f,
		1.f, 0.f,  1.f, 1.f,
		1.f, 1.f,  1.f, 0.f
	}; // Conte els vertexs d'un quad 2D i les texture coordinates

	glGenBuffers(1, &(ren->VBO));
	glBindBuffer(GL_ARRAY_BUFFER, ren->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glGenVertexArrays(1, &(ren->VAO));
	glBindVertexArray(ren->VAO);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void textUpdateScreenSize(TextRenderer* ren, float scr_width, float scr_height)
{
	glm_ortho(0.0f, scr_width, 0.0f, scr_height, -2.f, 2.f, ren->projectionOrtho);
}

void renderTextUI(unsigned int program, const char* text, float x, float y, float scale, vec3 color, vec3 outline, TextRenderer* ren)
{
	glDepthMask(GL_FALSE);
    // activate corresponding render state
    setUniformVec3(program, "textColor", color);
	setUniformVec3(program, "outlineColor", outline);
    setUniformMat4(program, "projection", false, ren->projectionOrtho);
	mat4 view = GLM_MAT4_IDENTITY_INIT;
	setUniformMat4(program, "view", false, view);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(ren->VAO);

	mat4 model;
    // iterate through all charactersstd::string::const_iterator c;
    for (const char* c = text; *c ; c++)
    {
        Character ch = ren->characterSet[*c - 32];

        float xpos = x + ch.bearing[0] * scale;
        float ypos = y - (ch.size[1] - ch.bearing[1]) * scale;

        float w = ch.size[0] * scale;
        float h = ch.size[1] * scale;
        // update VBO for each character
		glm_translate_make(model, (vec3) { xpos, ypos, 0.f });
		glm_scale(model, (vec3) { w, h, 1.f });
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // update content of VBO memory
		setUniformMat4(program, "model", false, model);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

	glDepthMask(GL_TRUE);
}