#pragma once
#ifndef SCENERY_H
#define SCENERY_H

#include "include/geometry.h"
#include <vector>

void createTerrainMesh(float terrainSize);
void createWaterMesh(float waterSize);
void createHouseMeshes(int numberOfHouses, float terrainSize, float waterSize);

extern std::vector<MyMesh> myMeshes;

#endif
