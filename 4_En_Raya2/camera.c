#include "camera.h"

#include <cglm/cglm.h>
#include <math.h>

void updateCamVectors(Camera* cam);

const float YAW = -90.f;
const float PITCH = 0.f;
const vec3 WORLDUP = { 0.f, 1.f, 0.f };
const float SENSITIVITY = 0.1f;
const float MOVEMENTSPEED = 2.5f;

void cameraInitialize(Camera* cam, const vec3 position)
{
    glm_vec3_copy((float*)position, cam->position);
    glm_vec3_copy((float*)WORLDUP, cam->worldUp);
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

    glm_vec3_crossn(cam->front, cam->worldUp, cam->right);
    glm_vec3_cross(cam->right, cam->front, cam->up);

}

void cameraProcessMouseMovement(Camera* cam, float xoffset, float yoffset)
{   
    xoffset *= cam->mouseSensitivity;
    yoffset *= cam->mouseSensitivity;
    
    cam->yaw = fmodf(cam->yaw + xoffset, 360.f);
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

void cameraProcessKeyborad(Camera* cam, Camera_Movement direction, float deltaTime)
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

void cameraGetViewMatrix(Camera* cam, mat4 dest)
{
    vec3 sum;
    glm_vec3_add(cam->position, cam->front, sum);
    glm_lookat(cam->position, sum, cam->up, dest);
}