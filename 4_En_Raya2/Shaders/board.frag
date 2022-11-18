#version 460 core
#define NCENTERS 1

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;

uniform sampler2D distanceField;
uniform float distThreshold;
uniform bool checkRadius;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{
    if (checkRadius)
    {
        if (texture(distanceField, TexCoord).r < distThreshold)
        {
            discard;
        }
    }
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(-FragPos);
    
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * material.diffuse;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    return (ambient + diffuse + specular);
}