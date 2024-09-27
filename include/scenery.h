#pragma once
#ifndef SCENERY_H
#define SCENERY_H

#include "include/geometry.h"
#include <vector>

void createTerrainMesh(float terrainSize);
void createWaterMesh(float waterSize);
void createHouseMeshes(int numberOfHouses, float terrainSize, float waterSize);
void createCreatures(int numberOfCreatures, float radius);

extern std::vector<MyMesh> myMeshes;

#endif
