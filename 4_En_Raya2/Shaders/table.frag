#version 460 core

in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform float shininess;

uniform vec3 lightPos;
uniform vec3 viewPos;

vec3 CalcDirLight(vec3 lightDir, vec3 normal, vec3 viewDir);

void main()
{   
    vec3 normal = normalize(texture(normalMap, TexCoord).rgb * 2 - 1); // Les normals codificades en RGB van al rang [0,1], cal passar-les a [-1,1]
    normal = vec3(normal.xz, -normal.y);         // Les normals del normal map es giren per que apuntin cap a +y (a la imatge d'origen apunten cap a +z)
    vec3 viewDir = normalize(viewPos - FragPos); // Només serveix per aquest cas específic on la superfície va tirada a terra.
    vec3 lightDir = normalize(lightPos - FragPos);

    vec3 result = CalcDirLight(lightDir, normal, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(vec3 lightDir, vec3 normal, vec3 viewDir)
{
    // diffuse shading
    vec3 color = texture(diffuseMap, TexCoord).rgb;
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    // combine results
    vec3 ambient  = 0.1 * color;
    vec3 diffuse  = diff * color;
    vec3 specular = spec * vec3(0.5);
    return (ambient + diffuse + specular);
}