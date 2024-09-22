#include "include/Boat.h"
#include "include/geometry.h"
#include "include/meshUtils.h"
#include "include/AVTmathLib.h"
#include "include/utils.h"

#include <GL/glew.h>

#include <array>
#include <cmath>
#include <algorithm>
#include <vector>

using std::vector;

extern vector<struct MyMesh> myMeshes;

#define DEG2RAD 3.14f/180.0f

Boat::Boat() {
    position = { 0.0f, 0.0f, 0.0f };
    direction = { 0.0f, 0.0f, 1.0f };
    currentForward = { 0.0f, 0.0f, 1.0f };

    speed = 0.0f;
    maxSpeed = 10.0f;
    acceleration = 1.0f;
    naturalDeceleration = 0.98f;
    paddleDeceleration = 1.0f;
    turboMode = false;

    rotationAngle = degToRad(2.0f);

    paddleStrength = 1.0f;
}

void Boat::applyAcceleration() {
    speed += acceleration * paddleStrength;
    if (speed > maxSpeed) {
        speed = maxSpeed;
    }
}

void Boat::applyDeceleration() {
    speed -= paddleDeceleration * paddleStrength;
    if (speed < -maxSpeed) {
        speed = -maxSpeed;
    }
}

void Boat::rotateY(float angle) {
    float cosAngle = std::cos(angle);
    float sinAngle = std::sin(angle);

    float newX = direction[0] * cosAngle - direction[2] * sinAngle;
    float newZ = direction[0] * sinAngle + direction[2] * cosAngle;

    direction[0] = newX;
    direction[2] = newZ;

    normalize(direction);
}

void Boat::paddleLeft() {
    rotateY(rotationAngle);
    applyAcceleration();
}

void Boat::paddleRight() {
    rotateY(-rotationAngle);
    applyAcceleration();
}

void Boat::paddleBackwardLeft() {
    rotateY(-rotationAngle);
    applyDeceleration();
}

void Boat::paddleBackwardRight() {
    rotateY(rotationAngle);
    applyDeceleration();
}

void Boat::toggleTurboMode() {
    turboMode = !turboMode;
    if (turboMode) {
        acceleration *= 2;
        paddleDeceleration *= 2;
        rotationAngle *= 2;
        paddleStrength *= 2;
    }
    else {
        acceleration /= 2;
        paddleDeceleration /= 2;
        rotationAngle /= 2;
        paddleStrength /= 2;
    }
}

void Boat::update(float deltaTime) {
    if (speed != 0.0f) {
        speed *= naturalDeceleration;
        if (std::abs(speed) < 0.01f) speed = 0.0f;
    }

    for (int i = 0; i < 3; ++i) {
        position[i] += direction[i] * speed * deltaTime;
    }
}

std::array<float, 3> Boat::getPosition() const {
    return position;
}

std::array<float, 3> Boat::getDirection() const {
    return direction;
}

float Boat::getSpeed() const {
    return speed;
}

void Boat::createMesh() {
    MyMesh amesh;

    float amb[] = { 0.1f, 0.05f, 0.02f, 1.0f };
    float diff[] = { 0.3f, 0.15f, 0.05f, 1.0f };
    float spec[] = { 0.2f, 0.1f, 0.05f, 1.0f };
    float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float shininess = 100.0f;
    int texcount = 0;

    amesh = createCube();
    memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
    memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
    memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
    memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
    amesh.mat.shininess = shininess;
    amesh.mat.texCount = texcount;

    amesh.xPosition = 0;
    amesh.yPosition = 0;

    amesh.name = "boat";

    myMeshes.push_back(amesh);

    mesh = amesh;
}

void Boat::render(MatrixTypes MODEL) {
    translate(MODEL, position[0], position[1], position[2]);
    float yaw = std::atan2(direction[0], direction[2]);
    rotate(MODEL, radToDeg(yaw), 0, 1, 0);
}
