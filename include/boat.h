#ifndef BOAT_H
#define BOAT_H

#include <array>
#include "include/AVTmathLib.h"
#include "include/geometry.h"

struct MyMesh;

class Boat {
private:
    std::array<float, 3> position;       // Boat's position in 3D space
    std::array<float, 3> direction;      // Unit vector representing the boat's direction
    std::array<float, 3> currentForward;

    float speed;                         // Scalar representing the boat's current speed
    float maxSpeed;                      // Maximum possible speed
    float acceleration;                  // Acceleration applied when paddling
    float deceleration;                  // Rate at which the speed decreases (inertia)
    
    float currentAngle;
    float rotationAngle;                 // Angle for direction change per stroke
    
    float paddleStrength;                // Strength of each paddle stroke

    MyMesh mesh;

    void applyAcceleration();            // Applies acceleration to the boat
    void rotateY(float angle);           // Rotates the boat around the Y-axis

public:
    Boat();

    void paddleLeft();                   // Paddle the boat to the left
    void paddleRight();                  // Paddle the boat to the right
    void invertPaddle();                 // Inverts the boat's direction
    void adjustPaddleStrength(bool increase);  // Adjusts paddle strength

    void update(float deltaTime);        // Updates the boat's position based on speed and direction

    std::array<float, 3> getPosition() const;  // Returns the boat's current position
    std::array<float, 3> getDirection() const; // Returns the boat's current direction
    float getSpeed() const;                    // Returns the boat's current speed

    void createMesh();               // Creates the boat's mesh
    void render(MatrixTypes MODEL, float dt);     // Renders the boat
};

#endif
