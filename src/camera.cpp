#include "include/Camera.h"
#include "include/utils.h"
#include "array"
#include <cmath>

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

    yaw = 0.0f;
    pitch = 0.0f;

    mcTargetX = 0.0f;
    mcTargetY = 0.0f;
    mcTargetZ = 0.0f;
}

void Camera::followBoat(std::array<float, 3> boatPosition, std::array<float, 3> boatDirection, bool topCamera = false, bool mouseMoving = false) {

    if (topCamera) {
        //camPos[0] = boatPosition[0];
        //camPos[2] = boatPosition[2];

        glutPostRedisplay();
        return;
    }

    float distance = 30.0f;

    std::array<float, 3> translatedPosition = {
        boatPosition[0] - boatDirection[0] * distance,
        40.0f,
        boatPosition[2] - boatDirection[2] * distance,
    };

    camPos[0] = translatedPosition[0];
    camPos[1] = translatedPosition[1];
    camPos[2] = translatedPosition[2];

    if (!mouseMoving) {
        camTarget[0] = boatPosition[0];
        camTarget[1] = boatPosition[1];
        camTarget[2] = boatPosition[2];
    }
    else {
        camTarget[0] = mcTargetX;
        camTarget[1] = mcTargetY;
        camTarget[2] = mcTargetZ;
    }

    glutPostRedisplay();
}

void Camera::updateTarget(float dx, float dy, float dz) {
    float newTargetX = camPos[0] + dx;
    float newTargetY = camPos[1] + dy;
    float newTargetZ = camPos[2] + dz;

    mcTargetX = newTargetX;
    mcTargetY = newTargetY;
    mcTargetZ = newTargetZ;
}

void Camera::computeCameraAngles() {
    std::array<float, 3> direction = {
        camTarget[0] - camPos[0],
        camTarget[1] - camPos[1],
        camTarget[2] - camPos[2]
    };

    float length = sqrt(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);

    std::array<float, 3> directionNormalized = {
        direction[0] / length,
        direction[1] / length,
        direction[2] / length
    };

    yaw = radToDeg(atan2(directionNormalized[2], directionNormalized[0]));
    pitch = radToDeg(asin(directionNormalized[1]));
}

float Camera::distanceFromCamera(std::array<float, 3> point) {
    float dx = camPos[0] - point[0];
    float dy = camPos[1] - point[1];
    float dz = camPos[2] - point[2];

    float distance = sqrt(dx * dx + dy * dy + dz * dz);
    return distance;
}
