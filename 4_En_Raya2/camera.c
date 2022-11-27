#include "camera.h"

#include <cglm/cglm.h>
#include <math.h>

void updateCamVectors(Camera* cam);
void updateCamOrbitPosition(Camera* cam);

const float PHI = -90.f;
const float THETA = 0.f;
const vec3 WORLDUP = { 0.f, 1.f, 0.f };
const float SENSITIVITY = 0.1f;
const float MOVEMENTSPEED = 2.5f;

void cameraInitialize(Camera* cam, const vec3 position, const vec3 orbitCenter)
{
    glm_vec3_copy((float*)position, cam->position);
    glm_vec3_copy((float*)WORLDUP, cam->worldUp);

    cam->phi = PHI;
    cam->phiTarget = cam->phi;
    cam->theta = THETA;
    cam->thetaTarget = cam->theta;

    glm_vec3_copy((float*)orbitCenter, cam->orbitCenter);
    vec3 temp;
    glm_vec3_sub(cam->position, cam->orbitCenter, temp);
    cam->orbitRadius = glm_vec3_norm(temp);
    cam->orbitRadiusTarget = cam->orbitRadius;

    cam->movementSpeed = MOVEMENTSPEED;
    cam->mouseSensitivity = SENSITIVITY;

    updateCamVectors(cam);
}

void updateCamVectors(Camera* cam)
{
    cam->front[0] = cosf(glm_rad(cam->phi)) * cosf(glm_rad(cam->theta));
    cam->front[1] = sinf(glm_rad(cam->theta));
    cam->front[2] = sinf(glm_rad(cam->phi)) * cosf(glm_rad(cam->theta));

    glm_vec3_crossn(cam->front, cam->worldUp, cam->right);
    glm_vec3_cross(cam->right, cam->front, cam->up);

}

void updateCamOrbitPosition(Camera* cam)
{
    float phiOrb = 180.f + cam->phi;
    float thetaOrb = -cam->theta;

    cam->position[0] = cam->orbitCenter[0] + cam->orbitRadius * cosf(glm_rad(phiOrb)) * cosf(glm_rad(thetaOrb));
    cam->position[1] = cam->orbitCenter[1] + cam->orbitRadius * sinf(glm_rad(thetaOrb));
    cam->position[2] = cam->orbitCenter[2] + cam->orbitRadius * sinf(glm_rad(phiOrb)) * cosf(glm_rad(thetaOrb));
}

void cameraProcessMouseMovement(Camera* cam, float xoffset, float yoffset)
{   
    xoffset *= cam->mouseSensitivity;
    yoffset *= cam->mouseSensitivity;
    
    cam->phi = fmodf(cam->phi + xoffset, 360.f);
    cam->theta += yoffset;

    if (cam->theta > 89.f)
    {
        cam->theta = 89.f;
    }
    if (cam->theta < -89.f)
    {
        cam->theta = -89.f;
    }

    updateCamOrbitPosition(cam);
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

void cameraProcessScroll(Camera* cam, float yoffset)
{
    cam->orbitRadiusTarget -= yoffset; // Scroll cap a dalt = zoom
    if (cam->orbitRadiusTarget < 1.f)
    {
        cam->orbitRadiusTarget = 1.f;
    }
}

void cameraLerpToTarget(Camera* cam, float deltaTime)
{
    cam->phi = glm_lerpc(cam->phi, cam->phiTarget, 10.f * deltaTime);
    cam->theta = glm_lerpc(cam->theta, cam->thetaTarget, 10.f * deltaTime);
    cam->orbitRadius = glm_lerpc(cam->orbitRadius, cam->orbitRadiusTarget, 10.f * deltaTime);

    updateCamOrbitPosition(cam);
    updateCamVectors(cam);
}


void cameraGetViewMatrix(Camera* cam, mat4 dest)
{
    vec3 sum;
    glm_vec3_add(cam->position, cam->front, sum);
    glm_lookat(cam->position, sum, cam->up, dest);
}