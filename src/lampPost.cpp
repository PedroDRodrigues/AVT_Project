#include "include/lampPost.h"
#include "include/utils.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "include/geometry.h"
#include "include/cube.h"
#include <vector>
#include <array>
#include <math.h>

const float PI = 3.14159f;
using namespace std;

LampPost createLampPostMesh(int radius) {
    float alpha_darkblue = 0.1f;
    float amb_darkblue[] = { 0.02f, 0.02f, 0.1f, 1.0f };
    float diff_darkblue[] = { 0.1f, 0.1f, 0.5f, alpha_darkblue };
    float spec_darkblue[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    float emissive_darkblue[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float shininess_darkblue = 20.0f;
    int texcount_darkblue = 0;

    MyMesh cubeMesh = createCube();

    array<float, 3> pos = computeRandomPosition(radius);
    array<float, 3> v = computeRandomDirection();

    float x = pos[0];
    float y = pos[1];
    float z = pos[2];

    float vx = v[0];
    float vy = v[1];
    float vz = v[2];

    LampPost amesh = LampPost(cubeMesh, pos, v);

    memcpy(amesh.mat.ambient, amb_darkblue, 4 * sizeof(float));
    memcpy(amesh.mat.diffuse, diff_darkblue, 4 * sizeof(float));
    memcpy(amesh.mat.specular, spec_darkblue, 4 * sizeof(float));
    memcpy(amesh.mat.emissive, emissive_darkblue, 4 * sizeof(float));
    amesh.mat.shininess = shininess_darkblue;
    amesh.mat.texCount = texcount_darkblue;

    amesh.x = x;
    amesh.y = y;
    amesh.z = z;

    return amesh;
}

vector<LampPost> createLampPostMeshes(int numberOfLampPosts, int radius) {
    vector<LampPost> lampPostMeshes;
    for (int i = 0; i < numberOfLampPosts; i++) {
        LampPost amesh = createLampPostMesh(radius);
        lampPostMeshes.push_back(amesh);
    }
    return lampPostMeshes;
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