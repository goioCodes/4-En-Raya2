#include <cglm/call.h>

const float YAW = -90.f;
const float PITCH = 0.f;
const vec3 FRONT = { 0.f, 0.f, -1.f };

typedef struct camera {
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 front;
    vec3 right;
    vec3 worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;
} Camera;

void initializeCamera(Camera* cam, vec3 position, vec3 up)
{
    for (int i = 0; i < 3; i++)
    {
        cam->position[i] = position[i];
        cam->worldUp[i] = up[i];
        cam->front[i] = FRONT[i];
    }
    cam->yaw = YAW;
    cam->pitch = PITCH;
    updateCamVectors(cam);
}

void updateCamVectors(Camera* cam)
{
    cam->front[0] = cosf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));
    cam->front[1] = sinf(glm_rad(cam->pitch));
    cam->front[2] = sinf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));

    glm_vec3_cross(cam->front, cam->worldUp, cam->right);
    glm_vec3_normalize(cam->right);
    glm_vec3_cross(cam->right, cam->front, cam->up);

}

void processMouseEvent(Camera* cam, float xoffset, float yoffset)
{   
    cam->yaw += xoffset;
    cam->pitch += yoffset;

    if (cam->pitch >= 89.f)
    {
        cam->pitch = 89.f;
    }
    else if (cam->pitch <= -89.f)
    {
        cam->pitch = -89.f;
    }
}

void processKeyboradEvent(Camera* cam, float xd);