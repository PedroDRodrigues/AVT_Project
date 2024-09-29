#ifndef BOAT_H
#define BOAT_H

#include <array>
#include "include/AVTmathLib.h"
#include "include/geometry.h"
#include "include/meshUtils.h"

struct MyMesh;

class Boat {
private:
    std::array<float, 3> position;
    std::array<float, 3> direction;
    std::array<float, 3> currentForward;

    float speed;
    float maxSpeed;
    float acceleration;
    float naturalDeceleration;
    float paddleDeceleration;
    bool turboMode;
    
    float rotationAngle;
    
    float paddleStrength;

    MyModel mesh;

    void applyAcceleration();
    void applyDeceleration();
    void rotateY(float angle);

public:
    Boat();

    void paddleLeft();
    void paddleRight();
    void paddleBackwardLeft();
    void paddleBackwardRight();
    void toggleTurboMode();

    void update(float deltaTime);

    std::array<float, 3> getPosition() const;
    std::array<float, 3> getDirection() const;
    float getSpeed() const;

    MyModel createMesh();
    void render(MatrixTypes MODEL);
};

#endif
