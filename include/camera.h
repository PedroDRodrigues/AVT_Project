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

    Camera();
    void updatePosition(float alpha, float beta, float r);
    void followBoat(std::array<float, 3> boatPosition, std::array<float, 3> boatDirection, bool topCamera);
};

#endif