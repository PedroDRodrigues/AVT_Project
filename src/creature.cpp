#include "include/creature.h"
#include "include/utils.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "include/geometry.h"
#include <vector>
#include <array>
#include <math.h>

const float PI = 3.14159f;
using namespace std;

Creature createCreatureMesh(float radius, float initialSpeed) {
    float alpha_darkblue = 0.5f;
    float amb_darkblue[] = { 0.02f, 0.02f, 0.1f, 1.0f };
    float diff_darkblue[] = { 0.1f, 0.1f, 0.5f, alpha_darkblue };
    float spec_darkblue[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    float emissive_darkblue[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float shininess_darkblue = 20.0f;
    int texcount_darkblue = 0;

    MyMesh sphereMesh = createSphere(1, 32);

    array<float, 3> pos = computeRandomPosition(radius);
    array<float, 3> v = computeRandomDirection();

    float x = pos[0];
    float y = pos[1];
    float z = pos[2];

    float vx = v[0];
    float vy = v[1];
    float vz = v[2];

    Creature amesh = Creature(sphereMesh, pos, v, initialSpeed);
    memcpy(amesh.mat.ambient, amb_darkblue, 4 * sizeof(float));
    memcpy(amesh.mat.diffuse, diff_darkblue, 4 * sizeof(float));
    memcpy(amesh.mat.specular, spec_darkblue, 4 * sizeof(float));
    memcpy(amesh.mat.emissive, emissive_darkblue, 4 * sizeof(float));
    amesh.mat.shininess = shininess_darkblue;
    amesh.mat.texCount = texcount_darkblue;

    amesh.x = x;
    amesh.y = y;
    amesh.z = z;

    amesh.max_pos_vert[0] = 1.5f;
    amesh.max_pos_vert[1] = 1.5f;
    amesh.max_pos_vert[2] = 1.5f;

    amesh.min_pos_vert[0] = 1.5f;
    amesh.min_pos_vert[1] = 1.5f;
    amesh.min_pos_vert[2] = 1.5f;

    amesh.oscillationSpeed = randomFloat(1.0f, 2.5f);

    return amesh;
}

vector<Creature> createCreatureMeshes(int numberOfCreatures, float radius, float initialSpeed) {
    vector<Creature> creatureMeshes;
    for (int i = 0; i < numberOfCreatures; i++) {
        Creature amesh = createCreatureMesh(radius, initialSpeed);
        creatureMeshes.push_back(amesh);
    }
    return creatureMeshes;
}

array<float, 3> computeRandomPosition(float radius) {
    float angle = randomFloat(0, 2 * PI);
    float x = radius * cos(angle);
    float y = 0.0f;
    float z = radius * sin(angle);
    array<float, 3> pos = { x, y, z };

    return pos;
}

array<float, 3> computeRandomDirection() {
    float vx = randomFloat(-1.0f, 1.0f);
    float vy = 0.0f;
    float vz = randomFloat(-1.0f, 1.0f);
    float length = sqrt(vx * vx + vy * vy + vz * vz);
    vx /= length;
    vy /= length;
    vz /= length;
    array<float, 3> v = { vx, vy, vz };

    return v;
}

void Creature::update(float deltaTime, float speedMultiplier, float maxDistance, float radius) {
    x += vx * speed * speedMultiplier * deltaTime;
    z += vz * speed * speedMultiplier * deltaTime;

    max_pos_vert[0] += vx * speed * speedMultiplier * deltaTime;
    max_pos_vert[2] += vz * speed * speedMultiplier * deltaTime;

    min_pos_vert[0] += vx * speed * speedMultiplier * deltaTime;
    min_pos_vert[2] += vz * speed * speedMultiplier * deltaTime; 

    float dist = sqrt(x * x + z * z);
    if (dist > maxDistance) {
        rebirth(radius);
    }
}

void Creature::rebirth(float radius) {
    array<float, 3> pos = computeRandomPosition(radius);
    array<float, 3> v = computeRandomDirection();

    x = pos[0];
    y = pos[1];
    z = pos[2];

    vx = v[0];
    vy = v[1];
    vz = v[2];
}

void Creature::applyShakeAnimation(float time, float shakeAmplitude) {
    y = shakeAmplitude * sin(time * oscillationSpeed);
}