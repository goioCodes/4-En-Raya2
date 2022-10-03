#include <cglm/call.h>

const float YAW = -90.f;
const float PITCH = 0.f;
const float SENSITIVITY = 0.1f;
const float MOVEMENTSPEED = 2.5f;
const vec3 FRONT = { 0.f, 0.f, -1.f };

typedef enum camera_movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
} Camera_Movement;

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

void updateCamVectors(Camera* cam);

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
    cam->movementSpeed = MOVEMENTSPEED;
    cam->mouseSensitivity = SENSITIVITY;
    updateCamVectors(cam);
}

void updateCamVectors(Camera* cam)
{
    cam->front[0] = cosf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));
    cam->front[1] = sinf(glm_rad(cam->pitch));
    cam->front[2] = sinf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));
    glm_vec3_normalize(cam->front);

    glm_vec3_cross(cam->front, cam->worldUp, cam->right);
    glm_vec3_normalize(cam->right);
    glm_vec3_cross(cam->right, cam->front, cam->up);
    glm_vec3_normalize(cam->up);

}

void processMouseMovement(Camera* cam, float xoffset, float yoffset)
{   
    xoffset *= cam->mouseSensitivity;
    yoffset *= cam->mouseSensitivity;

    cam->yaw += xoffset;
    cam->pitch += yoffset;

    if (cam->pitch > 89.f)
    {
        cam->pitch = 89.f;
    }
    if (cam->pitch < -89.f)
    {
        cam->pitch = -89.f;
    }

    updateCamVectors(cam);
}

void processKeyborad(Camera* cam, Camera_Movement direction, float deltaTime)
{
    float velocity = cam->movementSpeed * deltaTime;
    vec3 temp;
    if (direction == FORWARD)
    {
        glm_vec3_scale(cam->front, velocity, temp);
        glm_vec3_add(cam->position, temp, cam->position);
    }
    if (direction == BACKWARD)
    {
        glm_vec3_scale(cam->front, velocity, temp);
        glm_vec3_sub(cam->position, temp, cam->position);
    }
    if (direction == LEFT)
    {
        glm_vec3_scale(cam->right, velocity, temp);
        glm_vec3_sub(cam->position, temp, cam->position);
    }
    if (direction == RIGHT)
    {
        glm_vec3_scale(cam->right, velocity, temp);
        glm_vec3_add(cam->position, temp, cam->position);
    }
}