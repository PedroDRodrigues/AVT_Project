#include "include/utils.h"
#include "include/geometry.h"
#include <cstring>
#include <utility>
#include "include/lightDemo.h"

using namespace std;

const float PI = 3.14159f;

MyMesh createTerrainMesh(float terrainSize) {
    MyMesh amesh;

    float alpha_green = 1.0f;
    float amb_green[] = { 0.0f, 0.2f, 0.0f, 1.0f };
    float diff_green[] = { 0.0f, 0.8f, 0.0f, alpha_green };
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
    return amesh;
}

MyMesh createWaterMesh(float waterSize) {
    MyMesh amesh;

    float alpha_lightblue = 1.0f;
    float amb_lightblue[] = { 0.0f, 0.2f, 0.3f, 1.0f };
    float diff_lightblue[] = { 0.4f, 0.6f, 0.8f, alpha_lightblue };
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
    return amesh;
}

vector<MyMesh> createHouseMeshes(int numberOfHouses, float terrainSize, float waterSize) {

    vector<MyMesh> houseMeshes;

    float alpha_beige = 1.0f;
    float amb_beige[] = { 0.2f, 0.15f, 0.1f, 1.0f };
    float diff_beige[] = { 0.8f, 0.6f, 0.4f, alpha_beige };
    float spec_beige[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    float emissive_beige[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float shininess_beige = 100.0f;
    int texcount_beige = 0;

    for (int i = 0; i < numberOfHouses; i++) {
        MyMesh amesh;

        amesh = createCube();
        memcpy(amesh.mat.ambient, amb_beige, 4 * sizeof(float));
        memcpy(amesh.mat.diffuse, diff_beige, 4 * sizeof(float));
        memcpy(amesh.mat.specular, spec_beige, 4 * sizeof(float));
        memcpy(amesh.mat.emissive, emissive_beige, 4 * sizeof(float));
        amesh.mat.shininess = shininess_beige;
        amesh.mat.texCount = texcount_beige;

        std::pair<float, float> point = generateRandomPosition(terrainSize, waterSize, 5.0f, false);
        amesh.xPosition = point.first;
        amesh.yPosition = 0;
        amesh.zPosition = point.second;

        amesh.max_pos_vert[0] = 2.0f;
        amesh.max_pos_vert[1] = 12.0f;
        amesh.max_pos_vert[2] = 2.0f;

        amesh.min_pos_vert[0] = 2.0f;
        amesh.min_pos_vert[1] = 12.0f;
        amesh.min_pos_vert[2] = 2.0f;

        amesh.name = "house";
        houseMeshes.push_back(amesh);
    }

    return houseMeshes;
}

vector<MyMesh> createObstacleMeshes(int numberOfObstacles, float terrainSize, float waterSize) {
    vector<MyMesh> obstacleMeshes;

    float alpha_green = 1.0f;
    float amb_green[] = { 0.0f, 0.2f, 0.0f, 1.0f };
    float diff_green[] = { 0.0f, 0.8f, 0.0f, alpha_green };
    float spec_green[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    float emissive_green[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float shininess_green = 20.0f;
    int texcount_green = 0;

    for (int i = 0; i < numberOfObstacles; i++) {
        MyMesh amesh;
        float obstacle_size = randomBetween(2.5f, 5.0f);

        amesh = createSphere(obstacle_size, 32);
        memcpy(amesh.mat.ambient, amb_green, 4 * sizeof(float));
        memcpy(amesh.mat.diffuse, diff_green, 4 * sizeof(float));
        memcpy(amesh.mat.specular, spec_green, 4 * sizeof(float));
        memcpy(amesh.mat.emissive, emissive_green, 4 * sizeof(float));
        amesh.mat.shininess = shininess_green;
        amesh.mat.texCount = texcount_green;

        std::pair<float, float> point = generateRandomPosition(terrainSize, waterSize, obstacle_size, true);
        amesh.xPosition = point.first;
        amesh.yPosition = 0;
        amesh.zPosition = point.second;

        amesh.max_pos_vert[0] = obstacle_size;
        amesh.max_pos_vert[1] = obstacle_size;
        amesh.max_pos_vert[2] = obstacle_size;

        amesh.min_pos_vert[0] = obstacle_size;
        amesh.min_pos_vert[1] = obstacle_size;
        amesh.min_pos_vert[2] = obstacle_size;

        amesh.name = "obstacle";
        obstacleMeshes.push_back(amesh);
    }

    return obstacleMeshes;
}