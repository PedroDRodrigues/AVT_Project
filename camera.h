#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>
#include <GL/glut.h>

enum CameraType { PERSPECTIVE, ORTHOGONAL };

class Camera {
public:
    float camPos[3];
    float camTarget[3];
    CameraType type;

    Camera();
    void updatePosition(float alpha, float beta, float r);
};

#endif