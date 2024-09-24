#include "include/Camera.h"
#include "array"

#define DEG2RAD 3.14/180.0f

using namespace std;


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

void Camera::followBoat(std::array<float, 3> boatPosition, std::array<float, 3> boatDirection, bool topCamera = false) {

    if (topCamera) {
        camPos[0] = boatPosition[0];
        camPos[2] = boatPosition[2];
        glutPostRedisplay();
        return;
    }

    float distance = 10.0f;

    std::array<float, 3> translatedPosition = {
        boatPosition[0] - boatDirection[0] * distance,
        40.0f,
        boatPosition[2] - boatDirection[2] * distance,
    };

    //for (int i = 0; i < 3; ++i) {
    //    translatedPosition[i] = boatPosition[i] - boatDirection[i] * distance;
    //}
    //camPos[0] = translatedPosition[0];
    //camPos[1] = 40.0f;
    //camPos[2] = translatedPosition[2];

    camTarget[0] = boatPosition[0];
    camTarget[1] = boatPosition[1];
    camTarget[2] = boatPosition[2];

    glutPostRedisplay();
}