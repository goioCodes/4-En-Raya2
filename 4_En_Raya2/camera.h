#include <cglm/cglm.h>

typedef struct {
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

typedef enum {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
} Camera_Movement;

void cameraInitialize(Camera* cam, const vec3 position);
void cameraProcessMouseMovement(Camera* cam, float xoffset, float yoffset);
void cameraProcessKeyborad(Camera* cam, Camera_Movement direction, float deltaTime);
void cameraGetViewMatrix(Camera* cam, mat4 dest);