#include "include/utils.h"
#include "include/scenery.h"
#include "include/geometry.h"
#include <cstring>
#include <utility>

using namespace std;

extern vector<struct MyMesh> myMeshes;
const float PI = 3.14159f;

void createTerrainMesh(float terrainSize) {
    MyMesh amesh;

    float amb_green[] = { 0.0f, 0.2f, 0.0f, 1.0f };
    float diff_green[] = { 0.0f, 0.8f, 0.0f, 1.0f };
    float spec_green[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    float emissive_green[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float shininess_green = 20.0f;
    int texcount_green = 0;

    amesh = createQuad(terrainSize, terrainSize);
    memcpy(amesh.mat.ambient, amb_green, 4 * sizeof(float));
    memcpy(amesh.mat.diffuse, diff_green, 4 * sizeof(float));
    memcpy(amesh.mat.specular, spec_green, 4 * sizeof(float));
    memcpy(amesh.mat.emissive, emissive_green, 4 * sizeof(float));
    amesh.mat.shininess = shininess_green;
    amesh.mat.texCount = texcount_green;

    amesh.name = "terrain";

    myMeshes.push_back(amesh);
}

void createWaterMesh(float waterSize) {
    MyMesh amesh;

    float amb_lightblue[] = { 0.0f, 0.2f, 0.3f, 1.0f };
    float diff_lightblue[] = { 0.4f, 0.6f, 0.8f, 1.0f };
    float spec_lightblue[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    float emissive_lightblue[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float shininess_lightblue = 100.0f;
    int texcount_lightblue = 0;

    amesh = createQuad(waterSize, waterSize);
    memcpy(amesh.mat.ambient, amb_lightblue, 4 * sizeof(float));
    memcpy(amesh.mat.diffuse, diff_lightblue, 4 * sizeof(float));
    memcpy(amesh.mat.specular, spec_lightblue, 4 * sizeof(float));
    memcpy(amesh.mat.emissive, emissive_lightblue, 4 * sizeof(float));
    amesh.mat.shininess = shininess_lightblue;
    amesh.mat.texCount = texcount_lightblue;

    amesh.name = "water";

    myMeshes.push_back(amesh);
}

void createHouseMeshes(int numberOfHouses, float terrainSize, float waterSize) {
    MyMesh amesh;

    float amb_beige[] = { 0.2f, 0.15f, 0.1f, 1.0f };
    float diff_beige[] = { 0.8f, 0.6f, 0.4f, 1.0f };
    float spec_beige[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    float emissive_beige[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float shininess_beige = 100.0f;
    int texcount_beige = 0;

    for (int i = 0; i < numberOfHouses; i++) {
        amesh = createCube();
        memcpy(amesh.mat.ambient, amb_beige, 4 * sizeof(float));
        memcpy(amesh.mat.diffuse, diff_beige, 4 * sizeof(float));
        memcpy(amesh.mat.specular, spec_beige, 4 * sizeof(float));
        memcpy(amesh.mat.emissive, emissive_beige, 4 * sizeof(float));
        amesh.mat.shininess = shininess_beige;
        amesh.mat.texCount = texcount_beige;

        std::pair<float, float> point = generateRandomPosition(terrainSize, waterSize);
        amesh.xPosition = point.first;
        amesh.zPosition = point.second;

        amesh.name = "house";

        myMeshes.push_back(amesh);
    }
}

void createCreatures(int numberOfCreatures, float radius) {
    MyMesh amesh;

    float amb_darkblue[] = { 0.02f, 0.02f, 0.1f, 1.0f };
    float diff_darkblue[] = { 0.1f, 0.1f, 0.5f, 1.0f };
    float spec_darkblue[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    float emissive_darkblue[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float shininess_darkblue = 20.0f;
    int texcount_darkblue = 0;

    for (int i = 0; i < numberOfCreatures; i++) {
        float angle = randomFloat(0, 2 * PI);
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        amesh = createSphere(1, 32);
        memcpy(amesh.mat.ambient, amb_darkblue, 4 * sizeof(float));
        memcpy(amesh.mat.diffuse, diff_darkblue, 4 * sizeof(float));
        memcpy(amesh.mat.specular, spec_darkblue, 4 * sizeof(float));
        memcpy(amesh.mat.emissive, emissive_darkblue, 4 * sizeof(float));
        amesh.mat.shininess = shininess_darkblue;
        amesh.mat.texCount = texcount_darkblue;

        amesh.xPosition = x;
        amesh.zPosition = z;

        amesh.name = "creature";

        myMeshes.push_back(amesh);
    }
}