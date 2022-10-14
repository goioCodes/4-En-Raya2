#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform vec3 lightPos; // Posicio del llum en world coordinates

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(view * model * vec4(aPos, 1.f));
    Normal = mat3(transpose(inverse(view * model))) * aNormal; // Transformem normals i posicio del llum a view coords
}