#version 460 core
#define NCENTERS 1

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec2 centers[NCENTERS];

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(view * model * vec4(aPos, 1.f));
    Normal = mat3(transpose(inverse(view * model))) * aNormal; // Transformem normals i posicio del llum a view coords
}