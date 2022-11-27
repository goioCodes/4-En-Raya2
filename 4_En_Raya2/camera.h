#ifndef __CAMERA_H_
#define __CAMERA_H
#include <cglm/cglm.h>

typedef struct {
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 worldUp;

    float phi; // Angle azimutal
    float theta; // Angle latitud (-pi/2, pi/2)

    vec3 orbitCenter;
    float orbitRadius;

    float orbitRadiusTarget;
    float phiTarget;
    float thetaTarget;

    float movementSpeed;
    float mouseSensitivity;
} Camera;

typedef enum {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
} Camera_Movement;

void cameraInitialize(Camera* cam, const vec3 position, const vec3 orbitCenter);
void cameraProcessMouseMovement(Camera* cam, float xoffset, float yoffset);
void cameraProcessKeyborad(Camera* cam, Camera_Movement direction, float deltaTime);
void cameraProcessScroll(Camera* cam, float yoffset);
void cameraGetViewMatrix(Camera* cam, mat4 dest);
void cameraLerpToTarget(Camera* cam, float deltaTime);
#endif