#version 460 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
// Passem matriu de projeccio ortografica amb view = 1 per text 2D (UI), posem perspectiva i view per text en 3D

void main()
{
    gl_Position = projection * view * model * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}  