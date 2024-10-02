#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>
#include <GL/glut.h>
#include "array"

using namespace std;

enum CameraType { PERSPECTIVE, ORTHOGONAL };

class Camera {

public:

    float camPos[3];
    float camTarget[3];
    CameraType type;
    float yaw, pitch;
    float mcTargetX, mcTargetY, mcTargetZ; // mouse control target

    Camera();
    void followBoat(std::array<float, 3> boatPosition, std::array<float, 3> boatDirection, bool topCamera, bool mouseMoving);
    void updateTarget(float dx, float dy, float dz);
    void computeCameraAngles();
    float distanceFromCamera(std::array<float, 3> point);

};

#endif