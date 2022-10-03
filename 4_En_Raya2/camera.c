#include <cglm/call.h>

const vec3 FRONTDEF = { 0.f, 0.f, -1.f };
const float MOVESPEED = 2.5f;
const float SENSITIVITY = 0.1f;

typedef struct camera {
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;
} Camera;

void initializeCamera(Camera* cam, vec3 position, vec3 up, float yaw, float pitch)
{
    for (int i = 0; i < 3; i++)
    {
        cam->position[i] = position[i];
        cam->worldUp[i] = up[i];
        cam->front[i] = FRONTDEF[i];
    }
    cam->yaw = yaw;
    cam->pitch = pitch;
    cam->movementSpeed = MOVESPEED;
    cam->mouseSensitivity = SENSITIVITY;
}

void updateCameraVectors(Camera* cam)
{   

}