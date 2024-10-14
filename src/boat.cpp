#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

Boat::Boat() {
    position = { 0.0f, 5.0f, 0.0f };
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

    max_pos_vert = { 4.0f, 6.0f, 3.0f };
    min_pos_vert = { 4.0f, 6.0f, 3.0f };
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

std::array<float, 3> Boat::getMaxPosVert() const {
    return max_pos_vert;
}

std::array<float, 3> Boat::getMinPosVert() const {
    return min_pos_vert;
}

float Boat::getSpeed() const {
    return speed;
}

MyModel Boat::createMesh() {
    MyModel model = loadModel("models/boat.glb");
    model.name = "boat";
    mesh = model;
    return model;
}

void Boat::render(MatrixTypes MODEL) {
    translate(MODEL, position[0], position[1], position[2]);
    float yaw = std::atan2(direction[0], direction[2]);
    rotate(MODEL, radToDeg(yaw), 0, 1, 0);
}

void Boat::stop() {
    speed = 0.0f;
}

void Boat::setPosition(float x, float y, float z) {
    position = { x, y, z };
}