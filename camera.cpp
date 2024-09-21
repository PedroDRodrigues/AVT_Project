#include "Camera.h"

#define DEG2RAD 3.14/180.0f

Camera::Camera() {
    camPos[0] = 0.0f;
    camPos[1] = 0.0f;
    camPos[2] = 0.0f;

    camTarget[0] = 0.0f;
    camTarget[1] = 0.0f;
    camTarget[2] = 0.0f;

    type = PERSPECTIVE;
}

void Camera::updatePosition(float alpha, float beta, float r) {
    camPos[0] = r * sin(alpha * DEG2RAD) * cos(beta * DEG2RAD);
    camPos[2] = r * cos(alpha * DEG2RAD) * cos(beta * DEG2RAD);
    camPos[1] = r * sin(beta * DEG2RAD);

    glutPostRedisplay();
}